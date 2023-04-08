#include "LDAPQuery.h"
#include "SearchDnsHostname.h"

// 执行 LDAP 搜索, 检索所有计算机对象的 dNSHostName 属性
std::wstring SearchDnsHostname(const std::wstring& domain_wstr, const std::wstring& dc_wstr)
{
    LDAPQuery ldapQuery(dc_wstr);

    if (!ldapQuery.Initialize()) {
        std::cerr << "Failed to initialize LDAP connection.\n";
        std::exit(1);
    }

    if (!ldapQuery.Bind()) {
        std::cerr << "Failed to bind to LDAP server.\n";
        std::exit(1);
    }

    std::wstring domain = domain_wstr.substr(0, domain_wstr.find('.'));
    std::wstring dn_wstr = L"DC=" + domain + L"," + L"DC=" + domain_wstr.substr(domain_wstr.find('.') + 1);
    std::wstring filter_wstr = L"(&(objectClass=computer))";
    std::wstring attribute_wstr = L"dNSHostName";

    if (!ldapQuery.Search(dn_wstr, filter_wstr, attribute_wstr)) {
        std::cerr << "Failed to perform LDAP search.\n";
        std::exit(1);
    }

    return ldapQuery.GetdNSHostName();
}
