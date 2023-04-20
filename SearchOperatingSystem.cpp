#include "LDAPQuery.h"
#include "SearchOperatingSystem.h"
#include <vector>

// 执行 LDAP 搜索, 检索指定计算机对象的 operatingSystem 属性, 即操作系统信息
std::wstring SearchOperatingSystem(const std::wstring& domain_wstr, const std::wstring& dc_wstr, const std::wstring& computerName_wstr) {
    LDAPQuery ldapQuery(dc_wstr);

    if (!ldapQuery.Initialize()) {
        std::cerr << "Failed to initialize LDAP connection." << std::endl;
        exit(1);
    }

    if (!ldapQuery.Bind()) {
        std::cerr << "Failed to bind to LDAP server." << std::endl;
        exit(1);
    }

    std::wstring dn_wstr;
    std::vector<std::wstring> components;

    // 按照点号分割 domain 字符串
    size_t start = 0;
    size_t end = domain_wstr.find('.');
    while (end != std::wstring::npos)
    {
        components.push_back(domain_wstr.substr(start, end - start));
        start = end + 1;
        end = domain_wstr.find('.', start);
    }
    components.push_back(domain_wstr.substr(start));

    // 依次将子域名拼接成 LDAP DN 字符串
    for (size_t i = 0; i < components.size(); i++)
    {
        dn_wstr += L"DC=";
        dn_wstr += components[i];
        if (i < components.size() - 1)
        {
            dn_wstr += L",";
        }
    }

    std::wstring filter_wstr = L"(&(objectClass=computer)(cn=" + computerName_wstr + L"))";
    //std::wcout << filter_wstr << std::endl;  //输出测试
    std::wstring attribute_wstr = L"operatingSystem";

    if (!ldapQuery.Search(dn_wstr, filter_wstr, attribute_wstr)) {
        std::cerr << "Failed to perform LDAP search." << std::endl;
        exit(1);
    }

    return ldapQuery.GetOperatingSystem();
}
