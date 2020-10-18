#include "remoterichedit.h"

#include <cassert>
#include <iostream>
#include <Richedit.h>

void set_rich_edit_background_color(HWND rich_edit, COLORREF color) {
	SendMessage(rich_edit, EM_SETBKGNDCOLOR, 0, color);
}

/*
NOTE: 需要为远程进程分配内存，然后把这个地址值传给远程进程让其访问。
不能在本进程分配，然后把这个地址值给远程进程用，这样导致远程进程访问垃圾内存。
*/
void set_rich_edit_foreground_color(HWND rich_edit, COLORREF color) {
	bool br;

	// 构造格式参数
	CHARFORMAT fmt = { 0 };
	fmt.cbSize = sizeof(CHARFORMAT);
	fmt.dwMask = CFM_COLOR;
	fmt.dwEffects = 0;
	fmt.crTextColor = color;

	// 为目标进程分配参数内存
	DWORD pid = 0;
	GetWindowThreadProcessId(rich_edit, &pid);
	HANDLE h_process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	void* remote_ptr = VirtualAllocEx(
		h_process,
		0,
		sizeof(CHARFORMAT),
		MEM_COMMIT | MEM_RESERVE,
		PAGE_READWRITE
	);
	if (remote_ptr == NULL) {
		std::cout << "VirtualAllocEx failed\n";
		return;
	}
	SIZE_T bytes_written = 0;
	
	// 复制格式参数到目标进程
	br = WriteProcessMemory(
		h_process,
		remote_ptr,
		(void*)&fmt,
		sizeof(CHARFORMAT),
		&bytes_written
	);
	assert(br);

	// 发送设置格式消息
	SendMessage(rich_edit, EM_SETCHARFORMAT, SCF_ALL, (LPARAM)remote_ptr);

	// 释放刚才为目标进程分配的内存
	br = VirtualFreeEx(h_process, remote_ptr, 0, MEM_RELEASE);
	assert(br);
}
