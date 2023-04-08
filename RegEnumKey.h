#pragma once
#ifndef ENUMERATE_USER_KEYS_H
#define ENUMERATE_USER_KEYS_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <Windows.h>
#include <Aclapi.h>
#include <sddl.h>

std::wstring GetComputerName(const std::wstring& domain_wstr, const std::wstring& fullyQualifiedDomainName_wstr);
bool EnumerateUserKeys(HKEY hKeyUsers, const std::wstring& domain_wstr, const std::wstring& dc_wstr, std::ofstream& outputFile_of, const std::wstring& fullyQualifiedDomainName_wstr);

#endif // ENUMERATE_USER_KEYS_H
