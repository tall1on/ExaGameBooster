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
DWORD_PTR intel8p12ecoremask_rl = 268369920; // (8+12) 14700 - E-cores at logical 16-27
DWORD_PTR intel8p16ecoremask = 4294901760; // (8+16) 13900 + 14900

// Intel Arrow Lake (Core Ultra 200 series) - NO Hyper-Threading on P-cores or E-cores
// P-cores come first in the logical processor numbering, then E-cores
// Matches all variants: K, KF, F, T, non-suffix (e.g. 245K, 245KF, 245, 245T)
// 225: 6P+4E = 10 threads
DWORD_PTR intelAL6pcoremask = 63;      // P-cores: logical 0-5  (bits 0-5)
DWORD_PTR intelAL6p4ecoremask = 960;   // E-cores: logical 6-9  (bits 6-9)   - 225
// 235 / 245: 6P+8E = 14 threads
DWORD_PTR intelAL6p8ecoremask = 16320; // E-cores: logical 6-13 (bits 6-13)
// 250: 6P+12E = 18 threads
DWORD_PTR intelAL6p12ecoremask = 262080; // E-cores: logical 6-17 (bits 6-17)
// 265: 8P+12E = 20 threads
DWORD_PTR intelAL8pcoremask = 255;       // P-cores: logical 0-7  (bits 0-7)
DWORD_PTR intelAL8p12ecoremask = 1048320; // E-cores: logical 8-19 (bits 8-19)
// 270 / 285: 8P+16E = 24 threads
DWORD_PTR intelAL8p16ecoremask = 16776960; // E-cores: logical 8-23 (bits 8-23)

bool vcacheInUse = false;
string cpuBrandStringCache = "";
DWORD_PTR systemAffinityMaskCache = 0;

map <string, DWORD_PTR> gameMaskCache;
map <string, DWORD_PTR> serviceMaskCache;

// Returns a mask covering all logical processors the OS exposes, minus core 0 (logical processor 0).
// Core 0 is heavily used by Windows for DPC/interrupt handling.
// Core 1 (SMT sibling of core 0) is NOT specifically reserved by Windows and is kept for the game.
DWORD_PTR getDefaultGameAffinityMask()
{
    if (0 == systemAffinityMaskCache)
    {
        DWORD_PTR dwProcessAffinity, dwSystemAffinity;
        GetProcessAffinityMask(GetCurrentProcess(), &dwProcessAffinity, &dwSystemAffinity);
        systemAffinityMaskCache = dwSystemAffinity;
    }

    // Exclude logical processor 0 (bit 0) - reserved for Windows DPC/interrupt handling
    DWORD_PTR mask = systemAffinityMaskCache & ~(DWORD_PTR)1;

    // Safety: if somehow all bits were cleared (single-core system), fall back to all cores
    if (0 == mask)
    {
        mask = systemAffinityMaskCache;
    }

    return mask;
}

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
    // desktop - 12th/13th gen (Alder/Raptor Lake)
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
    // desktop - 14th gen (Raptor Lake Refresh) - P-cores have HT, E-cores do not
    else if (CPUBrandString.find("14900") != string::npos || CPUBrandString.find("14700") != string::npos)
    {
        // 14900: 8P+16E=32T, 14700: 8P+12E=28T - P-cores = logical 0-15 (8P x 2HT)
        cout << "found! apply intel8pcoremask (14900/14700)" << endl;
        gameMaskCache[game] = intel8pcoremask;
        return intel8pcoremask;
    }
    else if (CPUBrandString.find("14600") != string::npos || CPUBrandString.find("14500") != string::npos || CPUBrandString.find("14400") != string::npos)
    {
        // 14600/14500: 6P+8E=20T, 14400: 6P+4E=16T - P-cores = logical 0-11 (6P x 2HT)
        cout << "found! apply intel6pcoremask (14600/14500/14400)" << endl;
        gameMaskCache[game] = intel6pcoremask;
        return intel6pcoremask;
    }
    else if (CPUBrandString.find("14100") != string::npos)
    {
        // 14100: 4P+0E=8T - P-cores = logical 0-7 (4P x 2HT)
        cout << "found! apply intel4pcoremask (14100)" << endl;
        gameMaskCache[game] = intel4pcoremask;
        return intel4pcoremask;
    }
    // Intel Arrow Lake (Core Ultra 200 series) - no Hyper-Threading, P-cores only for games
    // Must be checked before the generic "Intel" fallback
    // Matches all variants: K, KF, F, T, non-suffix (e.g. 285K, 285KF, 285T, 285)
    else if (CPUBrandString.find("285") != string::npos || CPUBrandString.find("270") != string::npos)
    {
        // 8P+16E: P-cores = logical 0-7
        cout << "found! apply intelAL8pcoremask (285/270)" << endl;
        gameMaskCache[game] = intelAL8pcoremask;
        return intelAL8pcoremask;
    }
    else if (CPUBrandString.find("265") != string::npos)
    {
        // 8P+12E: P-cores = logical 0-7
        cout << "found! apply intelAL8pcoremask (265)" << endl;
        gameMaskCache[game] = intelAL8pcoremask;
        return intelAL8pcoremask;
    }
    else if (CPUBrandString.find("250") != string::npos)
    {
        // 6P+12E: P-cores = logical 0-5
        cout << "found! apply intelAL6pcoremask (250)" << endl;
        gameMaskCache[game] = intelAL6pcoremask;
        return intelAL6pcoremask;
    }
    else if (CPUBrandString.find("245") != string::npos || CPUBrandString.find("235") != string::npos)
    {
        // 6P+8E: P-cores = logical 0-5
        cout << "found! apply intelAL6pcoremask (245/235)" << endl;
        gameMaskCache[game] = intelAL6pcoremask;
        return intelAL6pcoremask;
    }
    else if (CPUBrandString.find("225") != string::npos)
    {
        // 6P+4E: P-cores = logical 0-5
        cout << "found! apply intelAL6pcoremask (225)" << endl;
        gameMaskCache[game] = intelAL6pcoremask;
        return intelAL6pcoremask;
    }

    // Ryzen 9000 X3D: always run games on X3D (vcache) cores, no cacheGames check
    if (CPUBrandString.find("9950X3D2") != string::npos || CPUBrandString.find("9950X3D") != string::npos)
    {
        cout << "found! apply vcachemask (Ryzen 9000 X3D always on vcache)" << endl;
        vcacheInUse = true;
        gameMaskCache[game] = vcachemask;
        return vcachemask;
    }
    else if (CPUBrandString.find("9900X3D") != string::npos)
    {
        cout << "found! apply vcachemask12c (Ryzen 9000 X3D always on vcache)" << endl;
        vcacheInUse = true;
        gameMaskCache[game] = vcachemask12c;
        return vcachemask12c;
    }
    // Ryzen 7000 X3D: use cacheGames array to decide vcache vs non-cache die
    else if (CPUBrandString.find("7950X3D") != string::npos || CPUBrandString.find("7945HX3D") != string::npos)
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

        cout << "found! apply ccd1mask" << endl;
        gameMaskCache[game] = ccd1mask;
        return ccd1mask;
    }
    else if (CPUBrandString.find("7900X3D") != string::npos)
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

        cout << "found! apply ccd1mask12c" << endl;
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

    // Unknown/unsupported CPU: use all cores except core 0 (Windows DPC/interrupt core)
    // Core 1 (SMT sibling of core 0) is kept since Windows does not specifically reserve it
    DWORD_PTR defaultMask = getDefaultGameAffinityMask();
    cout << "unknown CPU, apply default mask (all cores except core 0): " << defaultMask << endl;
    gameMaskCache[game] = defaultMask;
    return defaultMask;
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
    // 14th gen (Raptor Lake Refresh) - services go to E-cores (no HT on E-cores)
    else if (CPUBrandString.find("14900") != string::npos)
    {
        // 14900: 8P+16E=32T - E-cores = logical 16-31
        cout << "found! apply intel8p16ecoremask (14900)" << endl;
        serviceMaskCache[service] = intel8p16ecoremask;
        return intel8p16ecoremask;
    }
    else if (CPUBrandString.find("14700") != string::npos)
    {
        // 14700: 8P+12E=28T - E-cores = logical 16-27
        cout << "found! apply intel8p12ecoremask_rl (14700)" << endl;
        serviceMaskCache[service] = intel8p12ecoremask_rl;
        return intel8p12ecoremask_rl;
    }
    else if (CPUBrandString.find("14600") != string::npos || CPUBrandString.find("14500") != string::npos)
    {
        // 14600/14500: 6P+8E=20T - E-cores = logical 12-19
        cout << "found! apply intel6p8ecoremask (14600/14500)" << endl;
        serviceMaskCache[service] = intel6p8ecoremask;
        return intel6p8ecoremask;
    }
    else if (CPUBrandString.find("14400") != string::npos)
    {
        // 14400: 6P+4E=16T - E-cores = logical 12-15
        cout << "found! apply intel6p4ecoremask (14400)" << endl;
        serviceMaskCache[service] = intel6p4ecoremask;
        return intel6p4ecoremask;
    }
    else if (CPUBrandString.find("14100") != string::npos)
    {
        // 14100: 4P+0E=8T - no E-cores, trust scheduler
        cout << "found! 14100 no E-cores, trust scheduler" << endl;
        serviceMaskCache[service] = 0;
        return 0;
    }
    // Intel Arrow Lake (Core Ultra 200 series) - services go to E-cores
    // Matches all variants: K, KF, F, T, non-suffix
    else if (CPUBrandString.find("285") != string::npos || CPUBrandString.find("270") != string::npos)
    {
        // 8P+16E: E-cores = logical 8-23
        cout << "found! apply intelAL8p16ecoremask (285/270)" << endl;
        serviceMaskCache[service] = intelAL8p16ecoremask;
        return intelAL8p16ecoremask;
    }
    else if (CPUBrandString.find("265") != string::npos)
    {
        // 8P+12E: E-cores = logical 8-19
        cout << "found! apply intelAL8p12ecoremask (265)" << endl;
        serviceMaskCache[service] = intelAL8p12ecoremask;
        return intelAL8p12ecoremask;
    }
    else if (CPUBrandString.find("250") != string::npos)
    {
        // 6P+12E: E-cores = logical 6-17
        cout << "found! apply intelAL6p12ecoremask (250)" << endl;
        serviceMaskCache[service] = intelAL6p12ecoremask;
        return intelAL6p12ecoremask;
    }
    else if (CPUBrandString.find("245") != string::npos || CPUBrandString.find("235") != string::npos)
    {
        // 6P+8E: E-cores = logical 6-13
        cout << "found! apply intelAL6p8ecoremask (245/235)" << endl;
        serviceMaskCache[service] = intelAL6p8ecoremask;
        return intelAL6p8ecoremask;
    }
    else if (CPUBrandString.find("225") != string::npos)
    {
        // 6P+4E: E-cores = logical 6-9
        cout << "found! apply intelAL6p4ecoremask (225)" << endl;
        serviceMaskCache[service] = intelAL6p4ecoremask;
        return intelAL6p4ecoremask;
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
