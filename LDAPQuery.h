#ifndef LDAPQUERY_H
#define LDAPQUERY_H

#include <windows.h>
#include <winldap.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

class LDAPQuery {
public:
    LDAPQuery(const std::wstring& dc_wstr);
    ~LDAPQuery();
    bool Initialize();
    bool Bind();
    bool Search(const std::wstring& dn_wstr, const std::wstring& filter_wstr, const std::wstring& attribute_wstr);
    std::wstring GetdNSHostName();
    std::wstring GetOperatingSystem();

private:
    LDAP* ldap;
    int version;
    ULONG errorCode;
    std::wstring dc_wstr;
    LDAPMessage* searchResult;
};

#endif // LDAPQUERY_H
