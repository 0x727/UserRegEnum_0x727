#include "LDAPQuery.h"
#include "SearchOperatingSystem.h"

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

    std::wstring domain = domain_wstr.substr(0, domain_wstr.find('.'));
    std::wstring dn_wstr = L"DC=" + domain + L"," + L"DC=" + domain_wstr.substr(domain_wstr.find('.') + 1);
    std::wstring filter_wstr = L"(&(objectClass=computer)(cn=" + computerName_wstr + L"))";
    std::wstring attribute_wstr = L"operatingSystem";

    if (!ldapQuery.Search(dn_wstr, filter_wstr, attribute_wstr)) {
        std::cerr << "Failed to perform LDAP search." << std::endl;
        exit(1);
    }

    return ldapQuery.GetOperatingSystem();
}
