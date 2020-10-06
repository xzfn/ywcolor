#pragma once

#include <vector>
#include <windows.h>

std::vector<HWND> find_hwnds_by_pid(DWORD pid);
std::vector<HWND> find_child_hwnds_by_class(HWND parent_hwnd, const TCHAR* class_name);
