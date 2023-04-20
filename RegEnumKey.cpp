#include <codecvt>
#include <locale>
#include "SearchOperatingSystem.h"
#include "RegEnumKey.h"

// ��ȡ computerName, fullyQualifiedDomainName = test.missyou.com -> ���� test
std::wstring GetComputerName(const std::wstring& domain_wstr, const std::wstring& fullyQualifiedDomainName_wstr)
{
    //std::wstring domain = domain_wstr;
    std::wstring::size_type pos = fullyQualifiedDomainName_wstr.find(domain_wstr);
    if (pos != std::wstring::npos) {
        return fullyQualifiedDomainName_wstr.substr(0, pos - 1);
    }
    return fullyQualifiedDomainName_wstr;
}

// ��ָ���� Windows ע�����ö���û����Ӽ�
bool EnumerateUserKeys(HKEY hKeyUsers, const std::wstring& domain_wstr, const std::wstring& dc_wstr, std::ofstream& outputFile_of, const std::wstring& fullyQualifiedDomainName_wstr)
{
    // ��ȡ���������
    std::wstring computerName_wstr = GetComputerName(domain_wstr, fullyQualifiedDomainName_wstr);

    DWORD index = 0;
    TCHAR subKeyName[MAX_PATH];
    DWORD subKeyNameSize = MAX_PATH;

    while (RegEnumKeyEx(hKeyUsers, index++, subKeyName, &subKeyNameSize, NULL, NULL, NULL, NULL) == ERROR_SUCCESS) {
        LPWSTR sid = subKeyName;
        PSID pSid = nullptr;

        if (ConvertStringSidToSidW(sid, &pSid)) {
            // ������ SID �������û��˻���Ϣ
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
                //std::wcout << computerName_wstr << std::endl;  //�������
                // ��ȡ����ϵͳ��Ϣ
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