#include "findhwnd.h"

struct FindHwndsByPidParam {
	DWORD pid = 0;
	std::vector<HWND> hwnds;
};


// typedef BOOL(CALLBACK* WNDENUMPROC)(HWND, LPARAM);
BOOL CALLBACK hwnd_finder(HWND hwnd, LPARAM lParam) {
	FindHwndsByPidParam* p_param = (FindHwndsByPidParam*)lParam;
	DWORD curpid = 0;
	GetWindowThreadProcessId(hwnd, &curpid);
	//printf("xx%ld", curpid);
	if (curpid == p_param->pid) {
		p_param->hwnds.push_back(hwnd);
	}
	return TRUE;
}

std::vector<HWND> find_hwnds_by_pid(DWORD pid) {
	FindHwndsByPidParam param;
	param.pid = pid;
	EnumWindows(hwnd_finder, (LPARAM)&param);
	return param.hwnds;
}

struct FindChildHwndsByClassNameParam{
	const TCHAR* class_name = NULL;
	std::vector<HWND> child_hwnds;
};

BOOL CALLBACK child_hwnd_finder(HWND hwnd, LPARAM lParam) {
	FindChildHwndsByClassNameParam* p_param = (FindChildHwndsByClassNameParam*)lParam;
	TCHAR current_class_name[MAX_CLASS_NAME];
	GetClassName(hwnd, current_class_name, MAX_CLASS_NAME);
	if (wcscmp(p_param->class_name, current_class_name) == 0) {
		p_param->child_hwnds.push_back(hwnd);
	}
	return TRUE;
}

std::vector<HWND> find_child_hwnds_by_class(HWND parent_hwnd, const TCHAR* class_name) {
	FindChildHwndsByClassNameParam param;
	param.class_name = class_name;
	EnumChildWindows(parent_hwnd, child_hwnd_finder, (LPARAM)&param);
	return param.child_hwnds;
}
