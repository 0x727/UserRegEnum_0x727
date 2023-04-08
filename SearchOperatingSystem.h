#pragma once
#ifndef SEARCH_OPERATING_SYSTEM_H
#define SEARCH_OPERATING_SYSTEM_H

#include <string>

std::wstring SearchOperatingSystem(const std::wstring& domain_wstr, const std::wstring& dc_wstr, const std::wstring& computerName_wstr);

#endif // SEARCH_OPERATING_SYSTEM_H
