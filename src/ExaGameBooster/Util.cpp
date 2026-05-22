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
#include <winhttp.h>

#pragma comment(lib, "winhttp.lib")

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
    // Read and print the response
    string response = "";

    HINTERNET hSession = WinHttpOpen(
        L"ExaGameBooster/1.0",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS,
        0);
    if (!hSession)
        return response;

    HINTERNET hConnect = WinHttpConnect(
        hSession,
        L"cdn.exatek.de",
        INTERNET_DEFAULT_HTTPS_PORT,
        0);
    if (!hConnect) {
        WinHttpCloseHandle(hSession);
        return response;
    }

    HINTERNET hRequest = WinHttpOpenRequest(
        hConnect,
        L"GET",
        L"/exagamebooster/version.txt",
        NULL,
        WINHTTP_NO_REFERER,
        WINHTTP_DEFAULT_ACCEPT_TYPES,
        WINHTTP_FLAG_SECURE);
    if (!hRequest) {
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return response;
    }

    if (WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0) &&
        WinHttpReceiveResponse(hRequest, NULL))
    {
        char buffer[1024];
        DWORD bytesRead = 0;
        while (WinHttpReadData(hRequest, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0)
        {
            response.append(buffer, bytesRead);
        }
        std::cout << "success!\n";
    }

    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);

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
    std::string localVersion = "3.0.2.0";

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