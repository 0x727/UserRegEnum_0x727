#include "SearchDnsHostname.h"
#include "RegEnumKey.h"
#include "RegConnect.h"

int MAX_THREADS = 1000;

// 检查注册表连接
void checkRegistryConnection(const std::wstring& domain_wstr, const std::wstring& dc_wstr, std::ofstream& outputFile_of, const std::vector<std::wstring>& computerNames_wstr, int start, int end, std::mutex& mutex) {
    for (int i = start; i < end; i++) {
        // 尝试连接注册表
        HKEY hKeyUsers;
        LONG result = RegConnectRegistryW(computerNames_wstr[i].c_str(), HKEY_USERS, &hKeyUsers);
        if (result == ERROR_SUCCESS) {
            // 如果连接成功, 则枚举用户键, 并在互斥锁的保护下输出结果
            std::lock_guard<std::mutex> lock(mutex);
            EnumerateUserKeys(hKeyUsers, domain_wstr, dc_wstr, outputFile_of, computerNames_wstr[i].c_str());
            RegCloseKey(hKeyUsers);
        }
    }
}

void RegConnect(const std::wstring& domain_wstr, const std::wstring& dc_wstr, std::ofstream& outputFile_of, int numThreads) {
    
    // 根据传入的 numThreads 修改 MAX_THREADS 的值
    if (numThreads > 0) {
        MAX_THREADS = numThreads;
        //std::cout << "numThreads: " << MAX_THREADS << std::endl;  // 输出测试
    }

    // 从文件流读取计算机名
    std::vector<std::wstring> computerNames_wstr;
    std::wstring computerName_wstr;
    std::wistringstream iss(SearchDnsHostname(domain_wstr, dc_wstr));

    while (iss >> computerName_wstr) {
        computerNames_wstr.emplace_back(computerName_wstr);
    }

    std::mutex mutex;
    std::vector<std::future<void>> futures;

    // 将计算机名列表分割成若干个子向量，每个子向量作为一个任务
    int n = computerNames_wstr.size();
    int batchSize = n / MAX_THREADS + 1;
    //std::cout << MAX_THREADS << std::endl;  // 输出测试
    //std::cout << batchSize << std::endl;  // 输出测试
    for (int i = 0; i < n; i += batchSize) {
        int end = (i + batchSize < n) ? i + batchSize : n;
        // 在线程池中启动异步任务
        futures.emplace_back(std::async(std::launch::async, checkRegistryConnection, std::cref(domain_wstr), std::cref(dc_wstr), std::ref(outputFile_of), std::cref(computerNames_wstr), i, end, std::ref(mutex)));
    }

    // 等待所有任务完成
    for (auto& future : futures) {
        future.wait();
    }
}
