#pragma once
#ifndef SEARCH_DNS_HOSTNAME_H
#define SEARCH_DNS_HOSTNAME_H
#include <string>

std::wstring SearchDnsHostname(const std::wstring& domain_wstr, const std::wstring& dc_wstr);

#endif // SEARCH_DNS_HOSTNAME_H