import ctypes
import ctypes.wintypes

import tkinter as tk
import tkinter.colorchooser

import subprocess

import win32gui, win32api, win32process, win32con


def find_exe_pid(imagename):
    cmd = """tasklist /fi "IMAGENAME eq {}""".format(imagename)
    result = subprocess.run(cmd, stdout=subprocess.PIPE)
    output = result.stdout.decode('gbk')
    index = output.find(imagename)
    if index == -1:
        return -1
    pid = output[index:].split()[1]
    return int(pid)

def find_hwnds_by_pid(target_pid):
    hwnds = []
    def _finder(hwnd, lparam):
        thread_process_id = win32process.GetWindowThreadProcessId(hwnd)
        process_id = thread_process_id[1]
        print('ddd', process_id, target_pid)
        if process_id == target_pid:
            hwnds.append(hwnd)
        return True
    win32gui.EnumWindows(_finder, None)
    return hwnds

def find_hwnds_by_exe(imagename):
    target_pid = find_exe_pid(imagename)
    return find_hwnds_by_pid(target_pid)

def find_child_window(parent_hwnd, target_class_name):
    child_hwnds = []
    def _finder(hwnd, lparam):
        class_name = win32gui.GetClassName(hwnd)
        if class_name == target_class_name:
            child_hwnds.append(hwnd)
        return True
    win32gui.EnumChildWindows(parent_hwnd, _finder, None)
    return child_hwnds

def set_rich_edit_color(rich_edit, backgroud_color):
    WM_USER = 0x0400
    EM_SETBKGNDCOLOR = WM_USER + 67
    win32gui.SendMessage(rich_edit, EM_SETBKGNDCOLOR, 0, backgroud_color)

def tk_select_color():
    tk_color = tk.colorchooser.askcolor()
    print('tk_color', tk_color)
    if tk_color[1] is not None:
        rrggbb = tk_color[1][1:]
        rr = rrggbb[0:2]
        gg = rrggbb[2:4]
        bb = rrggbb[4:6]
        return int('0x' + bb + gg + rr, 16)
    else:
        return 0x00ffffff

LF_FACESIZE = 32
class CHARFORMAT(ctypes.Structure):
    _fields_ = [
        ('cbSize', ctypes.wintypes.UINT),
        ('dwMask', ctypes.wintypes.DWORD),
        ('dwEffects', ctypes.wintypes.DWORD),
        ('yHeight', ctypes.wintypes.LONG),
        ('yOffset', ctypes.wintypes.LONG),
        ('crTextColor', ctypes.wintypes.COLORREF),
        ('bCharSet', ctypes.wintypes.BYTE),
        ('bPitchAndFamily', ctypes.wintypes.BYTE),
        ('szFaceName', ctypes.wintypes.WCHAR * LF_FACESIZE)
    ]

def set_rich_edit_text_color(hprocess, rich_edit, text_color):
    WM_USER = 0x0400
    EM_SETCHARFORMAT = WM_USER + 68
    SCF_ALL = 0x0004
    CFM_COLOR = 0x40000000

    char_format = CHARFORMAT()
    char_format.cbSize = ctypes.sizeof(CHARFORMAT)
    char_format.dwMask = CFM_COLOR
    char_format.dwEffects = 0
    char_format.crTextColor = text_color

    CHARFORMAT_BUFFER = ctypes.POINTER(ctypes.c_char * ctypes.sizeof(CHARFORMAT))
    buffer = ctypes.cast(ctypes.addressof(char_format), CHARFORMAT_BUFFER).contents.raw
    try:
        remote_ptr = win32process.VirtualAllocEx(
            hprocess,
            0,
            len(buffer),
            win32con.MEM_COMMIT | win32con.MEM_RESERVE,
            win32con.PAGE_READWRITE
        )
        size_written = win32process.WriteProcessMemory(
            hprocess,
            remote_ptr,
            buffer
        )
        print('buffer', 'size', len(buffer), 'written', size_written)
        win32gui.SendMessage(rich_edit, EM_SETCHARFORMAT, SCF_ALL, remote_ptr)
    finally:
        win32process.VirtualFreeEx(hprocess, remote_ptr, 0, win32con.MEM_RELEASE)

def main():
    imagename = 'seperate.exe'
    hwnds = find_hwnds_by_exe(imagename)
    pid = find_exe_pid(imagename)
    print('hwnds', hwnds)
    targets = []
    for hwnd in hwnds:
        child_hwnds = find_child_window(hwnd, 'RichEdit20A')
        targets.extend(child_hwnds)
    if len(targets) == 0:
        print('ERROR no ya wei exe found')
        return
    if len(targets) != 1:
        print('WARN more than one rich edit')
    rich_edit = targets[0]
    target_color = tk_select_color()
    set_rich_edit_color(rich_edit, target_color)
    
    hprocess = win32api.OpenProcess(win32con.PROCESS_ALL_ACCESS, False, pid)
    target_text_color = tk_select_color()
    set_rich_edit_text_color(hprocess, rich_edit, target_text_color)

if __name__ == '__main__':
    main()
