/*
MIT license - wangyueheng
1025067301@qq.com

改变亚伟速录机软件的背景颜色和字体颜色，保护视力。
Change the background and foreground color of a top level RichEdit 
in a running program, using SendMessage.
*/


#include <iostream>
#include <vector>
#include <string>

#include <Windows.h>

#include "findpid.h"
#include "findhwnd.h"
#include "remoterichedit.h"
#include "win32choosecolor.h"


struct ArgOption {
    // 是否为命令行模式，默认是否，/console
    bool is_console = false;
    // 可执行文件名字，默认为seperate.exe，/image seperate.exe
    std::string image_name = "seperate.exe";
    // RichEdit名字，默认为RichEdit20A，/class RichEdit20A
    std::string rich_edit_name = "RichEdit20A";
    // 背景颜色，/background #0000ff
    COLORREF background_color = RGB(253, 246, 227);
    // 前景颜色，/foreground #00ffff
    COLORREF foreground_color = RGB(51, 51, 51);
};

void fill_default_option(ArgOption& option) {
    option.is_console = false;
    option.image_name = "seperate.exe";
    option.rich_edit_name = "RichEdit20A";
    option.background_color = RGB(253, 246, 227);
    option.foreground_color = RGB(51, 51, 51);
}

COLORREF parse_color(std::string s) {
    if (!(s.size() == 7 && s[0] == '#')) {
        std::cout << "Bad color format, use #RRGGBB, like #ff0000 for pure red\n";
        std::cout << "use \\#RRGGBB in powershell, use #RRGGBB in cmd\n";
        return RGB(255, 255, 255);
    }
    std::string hex_string = std::string("0x") + s.substr(5, 2) + s.substr(3, 2) + s.substr(1, 2);
    std::cout << "color input:" << hex_string << " " << atoi(hex_string.c_str()) << "\n";
    return strtol(hex_string.c_str(), NULL, 0);
}

ArgOption parse_args(int argc, char* argv[]) {
    ArgOption option;
    fill_default_option(option);
    std::vector<std::string> args;
    for (int i = 1; i < argc; i++) {
        args.push_back(argv[i]);
    }
    args.push_back("");
    for (std::size_t i = 0; i < args.size()-1; ++i) {
        std::string arg = args[i];
        std::string next_arg = args[i + 1];
        if (arg == "/console") {
            option.is_console = true;
        }
        else if (arg == "/image") {
            option.image_name = next_arg;
            ++i;
        }
        else if (arg == "/class") {
            option.rich_edit_name = next_arg;
            ++i;
        }
        else if (arg == "/foreground") {
            option.foreground_color = parse_color(next_arg);
            ++i;
        }
        else if (arg == "/background") {
            option.background_color = parse_color(next_arg);
            ++i;
        }
        else {
            std::cout << "Unexpected arg:" << arg << "\n";
        }
    }

    return option;
}

void print_usage() {
    std::cout << "===================================================\n";
    std::cout << "ya wei su lu color changer by wangyueheng.\n";
    std::cout << "checkout www.github.com/xzfn for the source code.\n";
    std::cout << "you can also use it in command line:\n";
    std::cout << "> ywcolorpp.exe /console /foreground #101010 /background #e0f0e0\n";
    std::cout << "or\n";
    std::cout << "> ywcolorpp.exe /console /image seperate.exe /class RichEdit20A /foreground #101010 /background #e0f0e0\n";
    std::cout << "===================================================\n";
}

int main(int argc, char* argv[])
{
    print_usage();

    // 解析命令行参数
    ArgOption option = parse_args(argc, argv);

    bool is_console = option.is_console;
    TCHAR image_name[MAX_CLASS_NAME];
    TCHAR rich_edit_name[MAX_PATH];
    size_t converted;
    mbstowcs_s(&converted, image_name, option.image_name.c_str(), MAX_CLASS_NAME);
    mbstowcs_s(&converted, rich_edit_name, option.rich_edit_name.c_str(), MAX_PATH);
    COLORREF background_color = option.background_color;
    COLORREF foreground_color = option.foreground_color;

    // 找到pid
    DWORD pid = find_pid_by_exe(image_name);
    if (pid == 0) {
        // 没有找到
        std::cout << "cannot find process for exe\n";
        if (!is_console) {
            MessageBox(0, TEXT("Please launch yawei seperate.exe first\n请先打开亚伟速录机程序，然后再执行本程序"),
                TEXT("提示"), MB_OK | MB_ICONINFORMATION);
        }
        return -1;
    }
    std::cout << "the pid is " << pid << "\n";

    // 找到hwnds
    std::vector<HWND> hwnds = find_hwnds_by_pid(pid);

    // 找到RichEditor的HWND
    HWND rich_edit = 0;
    std::vector<HWND> child_hwnds;
    for (HWND hwnd : hwnds) {
        std::vector<HWND> cur_child_hwnds = find_child_hwnds_by_class(hwnd, rich_edit_name);
        child_hwnds.insert(child_hwnds.end(), cur_child_hwnds.begin(), cur_child_hwnds.end());
    }
    if (child_hwnds.size() == 0) {
        // 没有找到
        std::cout << "cannot find child rich edit window\n";
        return -1;
    }
    if (child_hwnds.size() > 1) {
        // 找到了多个，打印警告，并且只处理第一个
        std::cout << "warning, more than one rich edit found, use first\n";
    }
    rich_edit = child_hwnds[0];
    std::cout << "rich edit hwnd handle " << rich_edit << "\n";

    if (!is_console) {
        // 如果是窗口模式，就用系统的color picker获取颜色
        // 获取背景色
        std::cout << "select background color\n";
        background_color = choose_color(background_color);
        // 获取字体颜色
        std::cout << "select foreground(text) color\n";
        foreground_color = choose_color(foreground_color);
    }
    // 设置背景色
    set_rich_edit_background_color(rich_edit, background_color);
    // 设置字体颜色
    set_rich_edit_foreground_color(rich_edit, foreground_color);
    std::cout << "exiting\n";
    return 0;
}
