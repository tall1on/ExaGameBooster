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

// https://stackoverflow.com/questions/47554388/winhttpsendrequest-fails-with-error-winhttp-secure-failure
string GetCurrentVersion() {
     // Open a session
    HINTERNET hSession = WinHttpOpen(L"ExaGameBooster Update Agent/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hSession) {
        std::cerr << "Failed to open session" << std::endl;
        DWORD dwError = GetLastError();
        std::cerr << "WinHttpOpen failed with error code: " << dwError << std::endl;
        return "";
    }

    HINTERNET hConnect = WinHttpConnect( hSession, L"cdn.exatek.de", INTERNET_DEFAULT_HTTP_PORT, 0);
    if (!hConnect) {
        std::cerr << "Failed to open connection" << std::endl;
        DWORD dwError = GetLastError();
        std::cerr << "WinHttpConnect failed with error code: " << dwError << std::endl;
        return "";
    }

    // Open a connection
    HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", L"/exagamebooster/version.txt", NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
    if (!hRequest) {
        std::cerr << "Failed to open request" << std::endl;
        DWORD dwError = GetLastError();
        std::cerr << "WinHttpOpenRequest failed with error code: " << dwError << std::endl;
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return "";
    }

    // Send the request
    if (!WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0)) {
        std::cerr << "Failed to send request" << std::endl;
        DWORD dwError = GetLastError();
        std::cerr << "WinHttpSendRequest failed with error code: " << dwError << std::endl;
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return "";
    }

    // Receive the response
    if (!WinHttpReceiveResponse(hRequest, NULL)) {
        std::cerr << "Failed to receive response" << std::endl;
        DWORD dwError = GetLastError();
        std::cerr << "WinHttpReceiveResponse failed with error code: " << dwError << std::endl;
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return "";
    }

    // Read and print the response
    string response;
    char buffer[1024];
    DWORD bytesRead = 0;
    while (WinHttpReadData(hRequest, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
        response.append(buffer, bytesRead);
    }

    // Clean up
    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hSession);

    return response;
}

LRESULT CALLBACK CustomDialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
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

int CheckUpdate()
{
    freopen("update.log", "w", stdout);
    freopen("update_error.log", "w", stderr);

    cout << "update popup! " << endl;

    // Get the current version number from the web server
    string currentVersion = GetCurrentVersion();
    if ("" == currentVersion) {
        cout << "get version error! " << endl;
        return 1;  // Error occurred while fetching version
    }
    cout << currentVersion << endl;

    // Your software's current version
    std::string localVersion = "2.0.0.0";

    cout << currentVersion << endl;
    cout << localVersion << endl;

    // Compare versions
    if (currentVersion > localVersion) {
        HINSTANCE hInstance = GetModuleHandle(NULL);

        cout << "update popup! " << endl;
        // Display the message in a popup window
        DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, CustomDialogProc);
    }

    return 0;
}