#include <iostream>
#include <Windows.h>
#include "Util.h"
#include <algorithm>

std::string wchar2string(wchar_t* str)
{
    std::string mystring;
    while (*str)
        mystring += (char)*str++;
    return  mystring;
}

void HideConsole()
{
    ::ShowWindow(::GetConsoleWindow(), SW_HIDE);
}

void ShowConsole()
{
    ::ShowWindow(::GetConsoleWindow(), SW_SHOW);
}

bool IsConsoleVisible()
{
    return ::IsWindowVisible(::GetConsoleWindow()) != FALSE;
}

bool isEqualTo(const std::string& a, const std::string& b)
{
    return std::equal(a.begin(), a.end(),
        b.begin(), b.end(),
        [](char a, char b) {
            return tolower(a) == tolower(b);
        });
}