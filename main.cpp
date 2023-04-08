#include <iostream>
#include "SearchDnsHostname.h"
#include "RegConnect.h"

void print_help() {
    std::wcout << "Usage example: \n"
        << "  program.exe -d missyou.com -dc dc.missyou.com -o result.csv\n"
        << "Options:\n"
        << "  -d, --domain=Domain               domain name to search\n"
        << "  -dc, --dc=domainControllers       domain controller to query\n"
        << "  -t, --threads=Theads              number of threads, default: 1000\n"
        << "  -o, --output=outputResult         output result file\n"
        << "  -h, --help                        display this help and exit\n";
}

int main(int argc, char** argv) {
    std::string domain;
    std::string dc;
    int threads = 0;
    std::string output;
    bool dcFlag = false;

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-h" || arg == "--help") {
            print_help();
            return 0;
        }
        else if (arg == "-d" || arg == "--domain") {
            if (i + 1 >= argc) {
                std::cout << "Missing argument for " << arg << std::endl;
                return 1;
            }
            domain = argv[++i];
        }
        else if (arg == "-dc" || arg == "--dc") {
            if (i + 1 >= argc) {
                std::cout << "Missing argument for " << arg << std::endl;
                return 1;
            }
            dc = argv[++i];
            dcFlag = true;
        }
        else if (arg == "-t" || arg == "--threads") {
            if (i + 1 >= argc) {
                std::cout << "Missing argument for " << arg << std::endl;
                return 1;
            }
            threads = std::stoi(argv[++i]);
        }
        else if (arg == "-o" || arg == "--output") {
            if (i + 1 >= argc) {
                std::cout << "Missing argument for " << arg << std::endl;
                return 1;
            }
            output = argv[++i];
        }
        else {
            std::cout << "Unknown option: " << arg << std::endl;
            return 1;
        }
    }

    std::wstring dc_wstr;
    if (dcFlag) {
        dc_wstr = std::wstring(dc.begin(), dc.end());
    }
    else
    {
        std::cout << "Missing required argument: -dc or --dc" << std::endl;
        return 1;
    }

    std::wstring domain_wstr;
    if (!domain.empty()) {
        domain_wstr = std::wstring(domain.begin(), domain.end());
    }
    else
    {
        std::cout << "Missing required argument: -d or --domain" << std::endl;
        return 1;
    }

    std::wstring outputFile_wstr;
    if (!output.empty()) {
        outputFile_wstr = std::wstring(output.begin(), output.end());
    }
    else
    {
        std::cout << "Missing required argument: -o or --output" << std::endl;
        return 1;
    }

    // 设置 console 的 CodePage, 这段代码只影响控制台的中文输出
    //SetConsoleOutputCP(CP_UTF8);
   
    // 打开文件, 并设置为追加模式
    std::ofstream outputFile_of(outputFile_wstr, std::ios::app);
    if (!outputFile_of.is_open()) {
        std::cerr << "Failed to open output file." << std::endl;
        return false;
    }
    
    // 输出 UTF-8 BOM
    outputFile_of << "\xef\xbb\xbf";

    outputFile_of << "Computer,OS,SID,Logon User\n";
    RegConnect(domain_wstr, dc_wstr, outputFile_of, threads);
    outputFile_of.close();
    std::cout << "Success!" << std::endl;

    return 0;
}
