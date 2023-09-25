#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <Windows.h>
#include "Util.h"
#include <algorithm>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <windows.h>
#include <vector>
#include "resource.h"
#include <WinINet.h>

#pragma comment(lib, "WinINet.lib")

using namespace std;

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

string GetCurrentVersion() {
    const wchar_t* url = L"https://cdn.exatek.de/exagamebooster/version.txt";
    // Read and print the response
    string response = "";
    
    HINTERNET hopen = InternetOpen(L"ExaGameBooster Update Agent/1.0", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
    if (hopen)
    {
        DWORD flags = INTERNET_FLAG_DONT_CACHE;
        if (wcsstr(url, L"https://") == url) {
            flags |= INTERNET_FLAG_SECURE;
        }

        HINTERNET hinternet = InternetOpenUrl(hopen, url, NULL, 0, flags, 0);
        if (hinternet)
        {
            char buffer[1024];
            DWORD bytesRead = 0;
            while (InternetReadFile(hinternet, buffer, sizeof(buffer), &bytesRead))
            {
                if (!bytesRead) break;
                response.append(buffer, bytesRead);
            }
            std::cout << "success!\n";
            InternetCloseHandle(hinternet);
        }
        InternetCloseHandle(hopen);
    }

    return response;
}

void CenterDialog(HWND hwndDlg) {
    // Get the dimensions of the primary monitor
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    // Calculate the position to center the dialog
    int dlgWidth, dlgHeight;
    RECT rcDlg;

    GetWindowRect(hwndDlg, &rcDlg);
    dlgWidth = rcDlg.right - rcDlg.left;
    dlgHeight = rcDlg.bottom - rcDlg.top;

    int x = (screenWidth - dlgWidth) / 2;
    int y = (screenHeight - dlgHeight) / 2;

    // Set the dialog's position
    SetWindowPos(hwndDlg, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

LRESULT CALLBACK CustomDialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_INITDIALOG:
            CenterDialog(hwnd);
            SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1)));
            SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1)));
            return TRUE;
        case WM_COMMAND:
            if (LOWORD(wParam) == IDC_BUTTON1) {
                // Handle the link click event here (e.g., open a web browser)
                ShellExecute(NULL, L"open", L"https://github.com/tall1on/ExaGameBooster/releases", NULL, NULL, SW_SHOWNORMAL);
            }
            break;

        case WM_CLOSE:
            EndDialog(hwnd, 0);
            break;
    }

    return FALSE;
}

void Parse(int result[4], const std::string& input)
{
    std::istringstream parser(input);
    parser >> result[0];
    for (int idx = 1; idx < 4; idx++)
    {
        parser.get(); //Skip period
        parser >> result[idx];
    }
}

bool LessThanVersion(const std::string& a, const std::string& b)
{
    int parsedA[4], parsedB[4];
    Parse(parsedA, a);
    Parse(parsedB, b);
    return std::lexicographical_compare(parsedA, parsedA + 4, parsedB, parsedB + 4);
}

int CheckUpdate()
{
    //freopen("update.log", "w", stdout);
    //freopen("update_error.log", "w", stderr);

    cout << "update popup! " << endl;

    // Get the current version number from the web server
    string currentVersion = GetCurrentVersion();
    if ("" == currentVersion) {
        cout << "get version error! " << endl;
        return 1;  // Error occurred while fetching version
    }
    cout << currentVersion << endl;

    // Your software's current version
    std::string localVersion = "3.0.0.0";

    cout << currentVersion << endl;
    cout << localVersion << endl;

    // Compare versions
    if (LessThanVersion(localVersion, currentVersion)) {
        HINSTANCE hInstance = GetModuleHandle(NULL);

        cout << "update popup! " << endl;
        // Display the message in a popup window
        DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, CustomDialogProc);
    }

    return 0;
}