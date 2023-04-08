#include "LDAPQuery.h"

#pragma comment(lib, "wldap32.lib")

// 构造函数，初始化 LDAP 查询对象
LDAPQuery::LDAPQuery(const std::wstring& dc_wstr) : ldap(nullptr), version(LDAP_VERSION3), errorCode(0), dc_wstr(dc_wstr), searchResult(nullptr) {}

// 析构函数，释放资源
LDAPQuery::~LDAPQuery() {
    if (searchResult) {
        ldap_msgfree(searchResult);
    }
    if (ldap) {
        ldap_unbind_s(ldap);
    }
}

// 初始化 LDAP 查询对象
bool LDAPQuery::Initialize() {
    // 初始化 LDAP 连接
    //ldap = ldap_initW(const_cast<wchar_t*>(dc.c_str()), LDAP_PORT); // 这样更简洁, 但是进行了类型转换

    // 动态分配 wchar_t 数组来存储 std::wstring 字符串的宽字符, 这样可以避免使用 const_cast 来去除 const 属性
    wchar_t* dcW = new wchar_t[dc_wstr.length() + 1];
    wcscpy_s(dcW, dc_wstr.length() + 1, dc_wstr.c_str());
    ldap = ldap_initW(dcW, LDAP_PORT);
    // 释放内存
    delete[] dcW;

    if (!ldap) {
        std::cerr << "Failed to initialize LDAP." << std::endl;
        return false;
    }

    // 设置 LDAP 协议版本
    errorCode = ldap_set_option(ldap, LDAP_OPT_PROTOCOL_VERSION, &version);
    if (errorCode != LDAP_SUCCESS) {
        std::cerr << "Failed to set LDAP option." << std::endl;
        ldap_unbind_s(ldap);
        return false;
    }

    return true;
}

// 绑定 LDAP 连接
bool LDAPQuery::Bind() {
    errorCode = ldap_bind_sW(ldap, nullptr, nullptr, LDAP_AUTH_NEGOTIATE);
    if (errorCode != LDAP_SUCCESS) {
        std::cerr << "Failed to bind LDAP." << std::endl;
        ldap_unbind_s(ldap);
        return false;
    }

    return true;
}


// 执行 LDAP 查询, 接受三个参数, 分别是 LDAP 目录名称 (dn), 查询过滤器 (filter), 和要返回的属性 (attribute)
//bool LDAPQuery::Search(const std::wstring& dn, const std::wstring& filter, const std::wstring& attribute) {
//    PWSTR attributes[] = { const_cast<wchar_t*>(attribute.c_str()), nullptr };
//    //PWSTR attributes[] = { attribute.data(), nullptr };
//    PWSTR filterPtr = const_cast<wchar_t*>(filter.c_str());
//    PWSTR dnPtr = const_cast<wchar_t*>(dn.c_str());
//
//    errorCode = ldap_search_sW(ldap, dnPtr, LDAP_SCOPE_SUBTREE, filterPtr, attributes, 0, &searchResult);
//    if (errorCode != LDAP_SUCCESS) {
//        std::cerr << "Failed to search LDAP." << std::endl;
//        ldap_unbind_s(ldap);
//        return false;
//    }
//
//    return true;
//}

// 执行 LDAP 查询, 接受三个参数, 分别是 LDAP 目录名称 (dn_wstr), 查询过滤器 (filter_wstr), 和要返回的属性 (attribute_wstr)
bool LDAPQuery::Search(const std::wstring& dn_wstr, const std::wstring& filter_wstr, const std::wstring& attribute_wstr) {
    // 动态分配 wchar_t 数组来存储 std::wstring 字符串的宽字符, 这样可以避免使用 const_cast 来去除 const 属性
    wchar_t* dnPtr = new wchar_t[dn_wstr.length() + 1];
    wcscpy_s(dnPtr, dn_wstr.length() + 1, dn_wstr.c_str());

    wchar_t* filterPtr = new wchar_t[filter_wstr.length() + 1];
    wcscpy_s(filterPtr, filter_wstr.length() + 1, filter_wstr.c_str());

    wchar_t* attributePtr = new wchar_t[attribute_wstr.length() + 1];
    wcscpy_s(attributePtr, attribute_wstr.length() + 1, attribute_wstr.c_str());

    // Set search parameters
    PWSTR attributes[] = { attributePtr, nullptr };

    // Perform LDAP search
    errorCode = ldap_search_sW(ldap, dnPtr, LDAP_SCOPE_SUBTREE, filterPtr, attributes, 0, &searchResult);
    if (errorCode != LDAP_SUCCESS) {
        std::wcerr << L"Failed to search LDAP." << std::endl;
        ldap_unbind_s(ldap);

        // Free dynamically allocated memory
        delete[] dnPtr;
        delete[] filterPtr;
        delete[] attributePtr;

        return false;
    }

    // Free dynamically allocated memory
    delete[] dnPtr;
    delete[] filterPtr;
    delete[] attributePtr;

    return true;
}

// 在 LDAP 查询结果中提取 "dNSHostName" 属性的值
std::wstring LDAPQuery::GetdNSHostName() {
    std::wstringstream output_wstringStream;

    // 将输出重定向到字符串流
    std::wstreambuf* outputBuffer = std::wcout.rdbuf();
    std::wcout.rdbuf(output_wstringStream.rdbuf());

    // 遍历 LDAP 查询结果并输出到字符串流
    for (LDAPMessage* entry = ldap_first_entry(ldap, searchResult); entry != nullptr; entry = ldap_next_entry(ldap, entry))
    {
        //PWSTR* values = ldap_get_valuesW(ldap, entry, const_cast<wchar_t*>(L"dNSHostName"));

        std::wstring attrName_wstr = L"dNSHostName";
        wchar_t* attrNameW = new wchar_t[attrName_wstr.length() + 1];
        wcscpy_s(attrNameW, attrName_wstr.length() + 1, attrName_wstr.c_str());

        PWSTR* values = ldap_get_valuesW(ldap, entry, attrNameW);
        delete[] attrNameW;

        if (values != nullptr)
        {
            std::wcout << values[0] << std::endl;
            ldap_value_freeW(values);
        }
    }

    std::wcout.rdbuf(outputBuffer);

    return output_wstringStream.str();
}


// 在 LDAP 查询结果中提取 "operatingSystem" 属性的值
std::wstring LDAPQuery::GetOperatingSystem() {
    std::wostringstream output_wostringstream;
    std::wstreambuf* outputBuffer = std::wcout.rdbuf();
    std::wcout.rdbuf(output_wostringstream.rdbuf());

    for (LDAPMessage* entry = ldap_first_entry(ldap, searchResult); entry != nullptr; entry = ldap_next_entry(ldap, entry))
    {
        //PWSTR* values = ldap_get_valuesW(ldap, entry, const_cast<wchar_t*>(L"operatingSystem"));

        std::wstring attrName_wstr = L"operatingSystem";
        wchar_t* attrNameW = new wchar_t[attrName_wstr.length() + 1];
        wcscpy_s(attrNameW, attrName_wstr.length() + 1, attrName_wstr.c_str());

        PWSTR* values = ldap_get_valuesW(ldap, entry, attrNameW);
        delete[] attrNameW;

        if (values != nullptr)
        {
            output_wostringstream << values[0] << std::endl;
            ldap_value_freeW(values);
        }
    }

    std::wcout.rdbuf(outputBuffer);
    return output_wostringstream.str();
}
