// ExaGameBooster.cpp
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <Windows.h>
#include <tlhelp32.h>
#include <cstdio>
#include <tchar.h>
#include <stdint.h>
#include <intrin.h>
#include <map>
#include <strsafe.h>
#include "ExaGameBooster.h"
#include <chrono>
#include <thread>
#include "Util.h"
#include "Config.h"
#include <thread>

#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

using namespace std;

// AMD Ryzen (5950X, 7950X, 7950X3D P-Die, 9950X, 9950X3D P-Die, 9950X3D2 P-Die)
DWORD_PTR ccd0mask = 65535; // Core0 - Core7 (with SMT)
DWORD_PTR ccd1mask = 4294901760;  // Core8 - Core15 (with SMT)

// AMD Ryzen (7950X3D Cache-Die, 9950X3D Cache-Die, 9950X3D2 Cache-Die)
DWORD_PTR vcachemask = ccd0mask;  // Core0 - Core7 (with V-Cache, with SMT)

// AMD Ryzen (5900X, 7900X, 7900X3D P-Die, 9900X, 9900X3D P-Die)
DWORD_PTR ccd0mask12c = 4095; // Core0 - Core5 (with SMT)
DWORD_PTR ccd1mask12c = 16773120; // Core6 - Core11 (with SMT)

// AMD Ryzen (7900X3D Cache-Die, 9900X3D Cache-Die)
DWORD_PTR vcachemask12c = ccd0mask12c;  // Core0 - Core5 (with V-Cache, with SMT)

//Intel Alder-Lake/Raptor-Lake (P-Cores)
DWORD_PTR intel4pcoremask = 255; // Core0 - Core3 (with HT)
DWORD_PTR intel6pcoremask = 4095; // Core0 - Core5 (with HT)
DWORD_PTR intel8pcoremask = 65535; // Core0 - Core7 (with HT)

//Intel Alder-Lake/Raptor-Lake (E-Cores)
DWORD_PTR intel6p4ecoremask = 61440; // (8+4) 12600 + 13400
DWORD_PTR intel6p8ecoremask = 1044480; // (6+8) 13600 + 13500
DWORD_PTR intel8p4ecoremask = 983040; // (8+4) 12700
DWORD_PTR intel8p8ecoremask = 16711680; // (8+8) 12900 + 13700
DWORD_PTR intel8p16ecoremask = 4294901760; // (8+16) 13900

bool vcacheInUse = false;
string cpuBrandStringCache = "";

map <string, DWORD_PTR> gameMaskCache;
map <string, DWORD_PTR> serviceMaskCache;

string getCpuBrandString()
{
    if ("" != cpuBrandStringCache)
    {
        return cpuBrandStringCache;
    }

    int CPUInfo[4] = { -1 };
    unsigned   nExIds, i = 0;
    char CPUBrandString[0x40];

    // Get the information associated with each extended ID.
    __cpuid(CPUInfo, 0x80000000);

    nExIds = CPUInfo[0];
    for (i = 0x80000000; i <= nExIds; ++i)
    {
        __cpuid(CPUInfo, i);

        // Interpret CPU brand string
        if (i == 0x80000002)
        {
            memcpy(CPUBrandString, CPUInfo, sizeof(CPUInfo));
        }
        else if (i == 0x80000003)
        {
            memcpy(CPUBrandString + 16, CPUInfo, sizeof(CPUInfo));
        }
        else if (i == 0x80000004)
        {
            memcpy(CPUBrandString + 32, CPUInfo, sizeof(CPUInfo));
        }
    }

    cout << "CPU Type: " << CPUBrandString << endl;

    cpuBrandStringCache = string(CPUBrandString);

    return string(CPUBrandString);
}

DWORD_PTR getGameProcessAffinityMask(string game)
{
    if (gameMaskCache.find(game) != gameMaskCache.end())
    {
        return gameMaskCache[game];
    }

    cout << "cache bypass! " << game << endl;

    string CPUBrandString = getCpuBrandString();

    // intel is ez, just take p-cores
    // mobile
    if (CPUBrandString.find("13980HX") != string::npos || CPUBrandString.find("13950HX") != string::npos || CPUBrandString.find("13900HX") != string::npos || CPUBrandString.find("13850HX") != string::npos || CPUBrandString.find("13700HX") != string::npos || CPUBrandString.find("12950HX") != string::npos || CPUBrandString.find("12900HX") != string::npos || CPUBrandString.find("12850HX") != string::npos || CPUBrandString.find("12800HX") != string::npos)
    {
        cout << "found! apply intel8pcoremask" << endl;
        gameMaskCache[game] = intel8pcoremask;
        return intel8pcoremask;
    }
    else if (CPUBrandString.find("13900HK") != string::npos || CPUBrandString.find("13905H") != string::npos || CPUBrandString.find("13900H") != string::npos || CPUBrandString.find("13800H") != string::npos || CPUBrandString.find("13705H") != string::npos || CPUBrandString.find("13700H") != string::npos || CPUBrandString.find("13620H") != string::npos || CPUBrandString.find("13650HX") != string::npos || CPUBrandString.find("13600HX") != string::npos || CPUBrandString.find("13500HX") != string::npos || CPUBrandString.find("13450HX") != string::npos || CPUBrandString.find("12900HK") != string::npos || CPUBrandString.find("12900H") != string::npos || CPUBrandString.find("12800H") != string::npos || CPUBrandString.find("12700H") != string::npos || CPUBrandString.find("12650H") != string::npos || CPUBrandString.find("1370P") != string::npos || CPUBrandString.find("1280P") != string::npos)
    {
        cout << "found! apply intel6pcoremask" << endl;
        gameMaskCache[game] = intel6pcoremask;
        return intel6pcoremask;
    }
    else if (CPUBrandString.find("13600H") != string::npos || CPUBrandString.find("13505H") != string::npos || CPUBrandString.find("13500H") != string::npos || CPUBrandString.find("3420H") != string::npos || CPUBrandString.find("12600H") != string::npos || CPUBrandString.find("12500H") != string::npos || CPUBrandString.find("12450H") != string::npos || CPUBrandString.find("1360P") != string::npos || CPUBrandString.find("1350P") != string::npos || CPUBrandString.find("1340P") != string::npos || CPUBrandString.find("1270P") != string::npos || CPUBrandString.find("1260P") != string::npos || CPUBrandString.find("1250P") != string::npos || CPUBrandString.find("1240P") != string::npos)
    {
        cout << "found! apply intel4pcoremask" << endl;
        gameMaskCache[game] = intel4pcoremask;
        return intel4pcoremask;
    }
    // desktop
    else if (CPUBrandString.find("13900") != string::npos || CPUBrandString.find("12900") != string::npos || CPUBrandString.find("13700") != string::npos || CPUBrandString.find("12700") != string::npos)
    {
        cout << "found! apply intel8pcoremask" << endl;
        gameMaskCache[game] = intel8pcoremask;
        return intel8pcoremask;
    }
    else if (CPUBrandString.find("13600") != string::npos || CPUBrandString.find("13500") != string::npos || CPUBrandString.find("13400") != string::npos || CPUBrandString.find("12600K") != string::npos)
    {
        cout << "found! apply intel6pcoremask" << endl;
        gameMaskCache[game] = intel6pcoremask;
        return intel6pcoremask;
    }
    else if (CPUBrandString.find("Intel") != string::npos)
    {
        // trust the scheduler :P
        gameMaskCache[game] = 0;
        return 0;
    }

    if (CPUBrandString.find("9950X3D2") != string::npos || CPUBrandString.find("9950X3D") != string::npos || CPUBrandString.find("7950X3D") != string::npos || CPUBrandString.find("7945HX3D") != string::npos)
    {
        for (string cacheGame : cacheGames) {
            // todo: check if compare needs to be lowercased
            if (isEqualTo(cacheGame, game))
            {
                cout << "found! apply vcachemask" << endl;
                vcacheInUse = true;
                return vcachemask;
            }
        }

        cout << "found! apply ccd0mask" << endl;
        gameMaskCache[game] = ccd1mask;
        return ccd1mask;
    }
    else if (CPUBrandString.find("7900X3D") != string::npos || CPUBrandString.find("9900X3D") != string::npos)
    {
        for (string cacheGame : cacheGames) {
            // todo: check if compare needs to be lowercased
            if (isEqualTo(cacheGame, game))
            {
                cout << "found! apply vcachemask12c" << endl;
                vcacheInUse = true;
                return vcachemask12c;
            }
        }

        cout << "found! apply ccd0mask12c" << endl;
        gameMaskCache[game] = ccd1mask12c;
        return ccd1mask12c;
    }

    // only apply vache masks => return to stay all core for other CPUs
    for (string allCoreGame : allCoreNoVCacheGames) {
        if (isEqualTo(allCoreGame, game))
        {
            gameMaskCache[game] = 0;
            return 0;
        }
    }

    if (CPUBrandString.find("9950X") != string::npos || CPUBrandString.find("7950X") != string::npos || CPUBrandString.find("7945HX") != string::npos || CPUBrandString.find("5950X") != string::npos || CPUBrandString.find("3950X") != string::npos)
    {
        cout << "found! apply ccd0mask" << endl;
        gameMaskCache[game] = ccd0mask;
        return ccd0mask;
    }
    else if (CPUBrandString.find("9900") != string::npos || CPUBrandString.find("7900") != string::npos || CPUBrandString.find("5900") != string::npos || CPUBrandString.find("3900") != string::npos)
    {
        cout << "found! apply ccd0mask12c" << endl;
        gameMaskCache[game] = ccd0mask12c;
        return ccd0mask12c;
    }

    // dont handle other AMD SKUs (no latency penalty issues)
    gameMaskCache[game] = 0;
    return 0;
}

DWORD_PTR getServiceProcessAffinityMask(string service)
{
    if (serviceMaskCache.find(service) != serviceMaskCache.end())
    {
        return serviceMaskCache[service];
    }

    cout << "cache bypass! " << service << endl;

    string CPUBrandString = getCpuBrandString();

    if (CPUBrandString.find("9950X3D2") != string::npos || CPUBrandString.find("9950X3D") != string::npos || CPUBrandString.find("7950X3D") != string::npos)
    {
        cout << "found! apply ccd0mask" << endl;
        if (vcacheInUse)
        {
            return ccd1mask;
        }
        return ccd0mask;
    }
    else if (CPUBrandString.find("9950X") != string::npos || CPUBrandString.find("7950X") != string::npos || CPUBrandString.find("5950X") != string::npos || CPUBrandString.find("3950X") != string::npos)
    {
        cout << "found! apply ccd1mask" << endl;
        serviceMaskCache[service] = ccd1mask;
        return ccd1mask;
    }
    else if (CPUBrandString.find("9900X3D") != string::npos || CPUBrandString.find("7900X3D") != string::npos)
    {
        cout << "found! apply ccd0mask12c" << endl;
        if (vcacheInUse)
        {
            return ccd1mask12c;
        }
        return ccd0mask12c;
    }
    else if (CPUBrandString.find("9900") != string::npos || CPUBrandString.find("7900") != string::npos || CPUBrandString.find("5900") != string::npos || CPUBrandString.find("3900") != string::npos)
    {
        cout << "found! apply ccd1mask12c" << endl;
        serviceMaskCache[service] = ccd1mask12c;
        return ccd1mask12c;
    }

    // intel e-core shitshow
    if (CPUBrandString.find("13900") != string::npos)
    {
        cout << "found! apply intel8p16ecoremask" << endl;
        serviceMaskCache[service] = intel8p16ecoremask;
        return intel8p16ecoremask;
    }
    else if (CPUBrandString.find("12900") != string::npos || CPUBrandString.find("13700") != string::npos)
    {
        cout << "found! apply intel8p8ecoremask" << endl;
        serviceMaskCache[service] = intel8p8ecoremask;
        return intel8p8ecoremask;
    }
    else if (CPUBrandString.find("12700") != string::npos)
    {
        cout << "found! apply intel8p4ecoremask" << endl;
        serviceMaskCache[service] = intel8p4ecoremask;
        return intel8p4ecoremask;
    }
    else if (CPUBrandString.find("13600") != string::npos || CPUBrandString.find("13500") != string::npos)
    {
        cout << "found! apply intel6p8ecoremask" << endl;
        serviceMaskCache[service] = intel6p8ecoremask;
        return intel6p8ecoremask;
    }
    else if (CPUBrandString.find("12600K") != string::npos || CPUBrandString.find("13400") != string::npos)
    {
        cout << "found! apply intel6p4ecoremask" << endl;
        serviceMaskCache[service] = intel6p4ecoremask;
        return intel6p4ecoremask;
    }
    else if (CPUBrandString.find("Intel") != string::npos)
    {
        // trust the scheduler :P
        serviceMaskCache[service] = 0;
        return 0;
    }

    serviceMaskCache[service] = 0;
    return 0;
}

int main()
{
    std::thread myThread(CheckUpdate);

    //freopen("output.log", "w", stdout);
    //freopen("error.log", "w", stderr);

    bool checkRunning = false;

    while (true)
    {
        if (checkRunning)
        {
            chrono::milliseconds timespan(15000);
            this_thread::sleep_for(timespan);

            continue;
        }

        checkRunning = true;
        vcacheInUse = false;
        PROCESSENTRY32 entry;
        entry.dwSize = sizeof(PROCESSENTRY32);

        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

        if (Process32First(snapshot, &entry) == TRUE)
        {
            while (Process32Next(snapshot, &entry) == TRUE)
            {
                for (string game : games) {
                    if (_wcsicmp(entry.szExeFile, wstring(game.begin(), game.end()).c_str()) == 0)
                    {
                        cout << "found! " << game << endl;

                        DWORD_PTR mask = getGameProcessAffinityMask(wchar2string(entry.szExeFile));

                        HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, entry.th32ProcessID);

                        DWORD_PTR oldPriorityClass = GetPriorityClass(hProcess);

                        if (ABOVE_NORMAL_PRIORITY_CLASS != oldPriorityClass)
                        {
                            SetPriorityClass(hProcess, ABOVE_NORMAL_PRIORITY_CLASS);
                        }

                        if (0 == mask)
                        {
                            CloseHandle(hProcess);
                            continue;
                        }

                        DWORD_PTR dwProcessAffinity, dwSystemAffinity;
                        GetProcessAffinityMask(hProcess, &dwProcessAffinity, &dwSystemAffinity);

                        if (mask != dwProcessAffinity)
                        {
                            BOOL success = SetProcessAffinityMask(hProcess, mask);
                        }

                        CloseHandle(hProcess);
                    }
                }
            }
        }

        CloseHandle(snapshot);

        snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

        if (Process32First(snapshot, &entry) == TRUE)
        {
            while (Process32Next(snapshot, &entry) == TRUE)
            {
                for (string service : services) {
                    if (_wcsicmp(entry.szExeFile, wstring(service.begin(), service.end()).c_str()) == 0)
                    {
                        cout << "found! " << service << endl;
                        DWORD_PTR mask = getServiceProcessAffinityMask(wchar2string(entry.szExeFile));

                        if (0 != mask)
                        {
                            HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, entry.th32ProcessID);

                            BOOL success = SetProcessAffinityMask(hProcess, mask);

                            CloseHandle(hProcess);
                        }
                    }
                }
            }
        }

        CloseHandle(snapshot);

        chrono::milliseconds timespan(15000);
        this_thread::sleep_for(timespan);

        checkRunning = false;
    }
}
