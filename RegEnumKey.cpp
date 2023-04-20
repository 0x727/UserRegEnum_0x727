#include <codecvt>
#include <locale>
#include "SearchOperatingSystem.h"
#include "RegEnumKey.h"

// 获取 computerName, fullyQualifiedDomainName = test.missyou.com -> 返回 test
std::wstring GetComputerName(const std::wstring& domain_wstr, const std::wstring& fullyQualifiedDomainName_wstr)
{
    //std::wstring domain = domain_wstr;
    std::wstring::size_type pos = fullyQualifiedDomainName_wstr.find(domain_wstr);
    if (pos != std::wstring::npos) {
        return fullyQualifiedDomainName_wstr.substr(0, pos - 1);
    }
    return fullyQualifiedDomainName_wstr;
}

// 从指定的 Windows 注册表中枚举用户的子键
bool EnumerateUserKeys(HKEY hKeyUsers, const std::wstring& domain_wstr, const std::wstring& dc_wstr, std::ofstream& outputFile_of, const std::wstring& fullyQualifiedDomainName_wstr)
{
    // 获取计算机名称
    std::wstring computerName_wstr = GetComputerName(domain_wstr, fullyQualifiedDomainName_wstr);

    DWORD index = 0;
    TCHAR subKeyName[MAX_PATH];
    DWORD subKeyNameSize = MAX_PATH;

    while (RegEnumKeyEx(hKeyUsers, index++, subKeyName, &subKeyNameSize, NULL, NULL, NULL, NULL) == ERROR_SUCCESS) {
        LPWSTR sid = subKeyName;
        PSID pSid = nullptr;

        if (ConvertStringSidToSidW(sid, &pSid)) {
            // 查找与 SID 关联的用户账户信息
            WCHAR szUserName[256] = { 0 };
            DWORD cchUserName = sizeof(szUserName) / sizeof(WCHAR);
            WCHAR szDomainName[256] = { 0 };
            DWORD cchDomainName = sizeof(szDomainName) / sizeof(WCHAR);
            SID_NAME_USE snu;

            if (!LookupAccountSidW(fullyQualifiedDomainName_wstr.c_str(), pSid, szUserName, &cchUserName, szDomainName, &cchDomainName, &snu)) {
                std::cerr << "Failed to lookup account SID: " << GetLastError() << std::endl;
                LocalFree(pSid);
            }
            else {
                //std::wcout << computerName_wstr << std::endl;  //输出测试
                // 获取操作系统信息
                std::wstring os_wstr = SearchOperatingSystem(domain_wstr, dc_wstr, computerName_wstr);
                os_wstr.erase(std::remove(os_wstr.begin(), os_wstr.end(), '\n'), os_wstr.end());

                std::wstringstream csvLineStream_wstringstream;
                csvLineStream_wstringstream << fullyQualifiedDomainName_wstr << L","
                    << os_wstr << L","
                    << subKeyName << L",\""
                    << szDomainName << L"\\" << szUserName << L"\""
                    << std::endl;

                std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
                std::string csvLineString = converter.to_bytes(csvLineStream_wstringstream.str());
                outputFile_of << csvLineString;

                LocalFree(pSid);
            }
        }

        subKeyNameSize = MAX_PATH;
    }

    return true;
}