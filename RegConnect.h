#pragma once
#ifndef REGCONNECT_HPP
#define REGCONNECT_HPP

#include <vector>
#include <string>
#include <future>
#include <mutex>
#include <fstream>
#include <Windows.h>

extern int MAX_THREADS;

void checkRegistryConnection(const std::wstring& domain_wstr, const std::wstring& dc_wstr, std::ofstream& outputFile_of, const std::vector<std::wstring>& computerNames_wstr, int start, int end, std::mutex& mutex);
void RegConnect(const std::wstring& domain_wstr, const std::wstring& dc_wstr, std::ofstream& outputFile_of, int numThreads);

#endif // REGCONNECT_HPP
