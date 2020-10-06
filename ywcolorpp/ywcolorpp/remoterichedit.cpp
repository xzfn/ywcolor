#include "remoterichedit.h"

#include <cassert>
#include <iostream>
#include <Richedit.h>

void set_rich_edit_background_color(HWND rich_edit, COLORREF color) {
	SendMessage(rich_edit, EM_SETBKGNDCOLOR, 0, color);
}

/*
NOTE: ��ҪΪԶ�̽��̷����ڴ棬Ȼ��������ֵַ����Զ�̽���������ʡ�
�����ڱ����̷��䣬Ȼ��������ֵַ��Զ�̽����ã���������Զ�̽��̷��������ڴ档
*/
void set_rich_edit_foreground_color(HWND rich_edit, COLORREF color) {
	bool br;

	// �����ʽ����
	CHARFORMAT fmt = { 0 };
	fmt.cbSize = sizeof(CHARFORMAT);
	fmt.dwMask = CFM_COLOR;
	fmt.dwEffects = 0;
	fmt.crTextColor = color;

	// ΪĿ����̷�������ڴ�
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
	
	// ���Ƹ�ʽ������Ŀ�����
	br = WriteProcessMemory(
		h_process,
		remote_ptr,
		(void*)&fmt,
		sizeof(CHARFORMAT),
		&bytes_written
	);
	assert(br);

	// �������ø�ʽ��Ϣ
	SendMessage(rich_edit, EM_SETCHARFORMAT, SCF_ALL, (LPARAM)remote_ptr);

	// �ͷŸղ�ΪĿ����̷�����ڴ�
	br = VirtualFreeEx(h_process, remote_ptr, 0, MEM_RELEASE);
	assert(br);
}
