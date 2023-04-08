#include "SearchDnsHostname.h"
#include "RegEnumKey.h"
#include "RegConnect.h"

int MAX_THREADS = 1000;

// ���ע�������
void checkRegistryConnection(const std::wstring& domain_wstr, const std::wstring& dc_wstr, std::ofstream& outputFile_of, const std::vector<std::wstring>& computerNames_wstr, int start, int end, std::mutex& mutex) {
    for (int i = start; i < end; i++) {
        // ��������ע���
        HKEY hKeyUsers;
        LONG result = RegConnectRegistryW(computerNames_wstr[i].c_str(), HKEY_USERS, &hKeyUsers);
        if (result == ERROR_SUCCESS) {
            // ������ӳɹ�, ��ö���û���, ���ڻ������ı�����������
            std::lock_guard<std::mutex> lock(mutex);
            EnumerateUserKeys(hKeyUsers, domain_wstr, dc_wstr, outputFile_of, computerNames_wstr[i].c_str());
            RegCloseKey(hKeyUsers);
        }
    }
}

void RegConnect(const std::wstring& domain_wstr, const std::wstring& dc_wstr, std::ofstream& outputFile_of, int numThreads) {
    
    // ���ݴ���� numThreads �޸� MAX_THREADS ��ֵ
    if (numThreads > 0) {
        MAX_THREADS = numThreads;
        //std::cout << "numThreads: " << MAX_THREADS << std::endl;  // �������
    }

    // ���ļ�����ȡ�������
    std::vector<std::wstring> computerNames_wstr;
    std::wstring computerName_wstr;
    std::wistringstream iss(SearchDnsHostname(domain_wstr, dc_wstr));

    while (iss >> computerName_wstr) {
        computerNames_wstr.emplace_back(computerName_wstr);
    }

    std::mutex mutex;
    std::vector<std::future<void>> futures;

    // ����������б�ָ�����ɸ���������ÿ����������Ϊһ������
    int n = computerNames_wstr.size();
    int batchSize = n / MAX_THREADS + 1;
    //std::cout << MAX_THREADS << std::endl;  // �������
    //std::cout << batchSize << std::endl;  // �������
    for (int i = 0; i < n; i += batchSize) {
        int end = (i + batchSize < n) ? i + batchSize : n;
        // ���̳߳��������첽����
        futures.emplace_back(std::async(std::launch::async, checkRegistryConnection, std::cref(domain_wstr), std::cref(dc_wstr), std::ref(outputFile_of), std::cref(computerNames_wstr), i, end, std::ref(mutex)));
    }

    // �ȴ������������
    for (auto& future : futures) {
        future.wait();
    }
}
