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

#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

using namespace std;

string games[] = {"AssettoCorsa.exe", "Bannerlord.exe", "Bannerlord.Native.exe", "cod.exe", "Cyberpunk2077.exe", "DCS.exe", "FarCry6.exe", "FactoryGame.exe", "FarmingSimulator2022.exe", "ffxiv.exe", "FIFA23.exe", "F1_22.exe", "F1Manager22.exe", "ForzaHorizon5.exe", "GenshinImpact.exe", "GoW.exe", "GTA5.exe", "Hearthstone.exe", "HITMAN3.exe", "hlvr.exe", "hops.exe", "minecraft.exe", "Minecraft.exe", "League of Legends.exe", "MonsterHunterRise.exe", "MonsterHunterWorld.exe", "Prison Architect64.exe", "ReadyOrNot.exe", "RDR2.exe", "ShadowOfWar.exe", "starwarsjedifallenorder.exe", "Skyrim.exe", "SkyrimSE.exe", "StarCitizen.exe", "sottr.exe", "SwGame-Win64-Shipping.exe", "TheAscent.exe", "TransportFever2.exe", "TS2Prototype-Win64-Shipping.exe", "Universe Sandbox VR.exe", "Universe Sandbox x64.exe", "VALORANT-Win64-Shipping.exe", "Warframe.x64.exe", "Warhammer3.exe", "witcher3.exe", "Wow.exe", "X4.exe"};
string allCoreNoVCacheGames[] = { "GTA5.exe", "HITMAN3.exe" };
string cacheGames[] = { "cod.exe", "Cyberpunk2077.exe", "FactoryGame.exe", "FarCry6.exe", "ffxiv.exe", "F1_22.exe", "GTA5.exe", "HITMAN3.exe", "VALORANT-Win64-Shipping.exe", "Warhammer3.exe", "RDR2.exe", "sottr.exe", "Wow.exe"};
string services[] = {"chrome.exe", "Discord.exe", "firefox.exe", "L-Connect 3.exe", "msedge.exe", "obs64.exe", "Spotify.exe", "steamwebhelper.exe", "ts3client_win64.exe", "wallpaper64.exe"};

// AMD Ryzen (5950X, 7950X, 7950X3D P-Die)
DWORD_PTR ccd0mask = 65535; // Core0 - Core7 (with SMT)
DWORD_PTR ccd1mask = 4294901760;  // Core8 - Core15 (with SMT)

// AMD Ryzen (7950X3D Cache-Die)
DWORD_PTR vcachemask = 4294901760;  // Core8 - Core15 (with SMT)

// AMD Ryzen (5900X, 7900X, 7900X3D P-Die)
DWORD_PTR ccd0mask12c = 4095; // Core0 - Core7 (with SMT)
DWORD_PTR ccd1mask12c = 16773120; // Core0 - Core7 (with SMT)

// AMD Ryzen (7900X3D Cache-Die)
DWORD_PTR vcachemask12c = 16773120;  // Core8 - Core15 (with SMT)

//Intel Alder-Lake/Raptor-Lake (P-Cores)
DWORD_PTR intel4pcoremask = 255; // Core0 - Core5 (with HT)
DWORD_PTR intel6pcoremask = 4095; // Core0 - Core5 (with HT)
DWORD_PTR intel8pcoremask = 65535; // Core0 - Core7 (with HT)

//Intel Alder-Lake/Raptor-Lake (E-Cores)
DWORD_PTR intel6p4ecoremask = 61440; // (8+4) 12600 + 13400
DWORD_PTR intel6p8ecoremask = 1044480; // (6+8) 13600 + 13500
DWORD_PTR intel8p4ecoremask = 983040; // (8+4) 12700
DWORD_PTR intel8p8ecoremask = 16711680; // (8+8) 12900 + 13700
DWORD_PTR intel8p16ecoremask = 4294901760; // (8+16) 13900

string getCpuBrandString()
{
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

    return string(CPUBrandString);
}

DWORD_PTR getGameProcessAffinityMask(string game)
{
    string CPUBrandString = getCpuBrandString();

    // intel is ez, just take p-cores
    // mobile
    if (CPUBrandString.find("13980HX") != string::npos || CPUBrandString.find("13950HX") != string::npos || CPUBrandString.find("13900HX") != string::npos || CPUBrandString.find("13850HX") != string::npos || CPUBrandString.find("13700HX") != string::npos || CPUBrandString.find("12950HX") != string::npos || CPUBrandString.find("12900HX") != string::npos || CPUBrandString.find("12850HX") != string::npos || CPUBrandString.find("12800HX") != string::npos)
    {
        cout << "found! apply intel8pcoremask" << endl;
        return intel8pcoremask;
    }
    else if (CPUBrandString.find("13900HK") != string::npos || CPUBrandString.find("13905H") != string::npos || CPUBrandString.find("13900H") != string::npos || CPUBrandString.find("13800H") != string::npos || CPUBrandString.find("13705H") != string::npos || CPUBrandString.find("13700H") != string::npos || CPUBrandString.find("13620H") != string::npos || CPUBrandString.find("13650HX") != string::npos || CPUBrandString.find("13600HX") != string::npos || CPUBrandString.find("13500HX") != string::npos || CPUBrandString.find("13450HX") != string::npos || CPUBrandString.find("12900HK") != string::npos || CPUBrandString.find("12900H") != string::npos || CPUBrandString.find("12800H") != string::npos || CPUBrandString.find("12700H") != string::npos || CPUBrandString.find("12650H") != string::npos || CPUBrandString.find("1370P") != string::npos || CPUBrandString.find("1280P") != string::npos)
    {
        cout << "found! apply intel6pcoremask" << endl;
        return intel6pcoremask;
    }
    else if (CPUBrandString.find("13600H") != string::npos || CPUBrandString.find("13505H") != string::npos || CPUBrandString.find("13500H") != string::npos || CPUBrandString.find("3420H") != string::npos || CPUBrandString.find("12600H") != string::npos || CPUBrandString.find("12500H") != string::npos || CPUBrandString.find("12450H") != string::npos || CPUBrandString.find("1360P") != string::npos || CPUBrandString.find("1350P") != string::npos || CPUBrandString.find("1340P") != string::npos || CPUBrandString.find("1270P") != string::npos || CPUBrandString.find("1260P") != string::npos || CPUBrandString.find("1250P") != string::npos || CPUBrandString.find("1240P") != string::npos)
    {
        cout << "found! apply intel4pcoremask" << endl;
        return intel4pcoremask;
    }
    // desktop
    else if (CPUBrandString.find("13900") != string::npos || CPUBrandString.find("12900") != string::npos || CPUBrandString.find("13700") != string::npos || CPUBrandString.find("12700") != string::npos)
    {
        cout << "found! apply intel8pcoremask" << endl;
        return intel8pcoremask;
    }
    else if (CPUBrandString.find("13600") != string::npos || CPUBrandString.find("13500") != string::npos || CPUBrandString.find("13400") != string::npos || CPUBrandString.find("12600K") != string::npos)
    {
        cout << "found! apply intel6pcoremask" << endl;
        return intel6pcoremask;
    }
    else if (CPUBrandString.find("Intel") != string::npos)
    {
        // trust the scheduler :P
        return 0;
    }

    if (CPUBrandString.find("7950X3D") != string::npos)
    {
        for (string cacheGame : cacheGames) {
            if (cacheGame == game)
            {
                cout << "found! apply vcachemask" << endl;
                return vcachemask;
            }
        }

        cout << "found! apply ccd0mask" << endl;
        return ccd0mask;
    }
    else if (CPUBrandString.find("7900X3D") != string::npos)
    {
        for (string cacheGame : cacheGames) {
            if (cacheGame == game)
            {
                cout << "found! apply vcachemask12c" << endl;
                return vcachemask12c;
            }
        }

        cout << "found! apply ccd0mask12c" << endl;
        return ccd0mask12c;
    }

    // only apply vache masks => return to stay all core for other CPUs
    for (string allCoreGame : allCoreNoVCacheGames) {
        if (allCoreGame == game)
        {
            return 0;
        }
    }

    if (CPUBrandString.find("7950X") != string::npos || CPUBrandString.find("5950X") != string::npos || CPUBrandString.find("3950X") != string::npos)
    {
        cout << "found! apply ccd0mask" << endl;
        return ccd0mask;
    }
    else if (CPUBrandString.find("7900") != string::npos || CPUBrandString.find("5900") != string::npos || CPUBrandString.find("3900") != string::npos)
    {
        cout << "found! apply ccd0mask12c" << endl;
        return ccd0mask12c;
    }

    // dont handle other AMD SKUs (no latency penalty issues)
    return 0;
}

DWORD_PTR getServiceProcessAffinityMask(string service)
{
    string CPUBrandString = getCpuBrandString();

    if (CPUBrandString.find("7950X3D") != string::npos)
    {
        cout << "found! apply ccd0mask" << endl;
        return ccd0mask;
    }
    else if (CPUBrandString.find("7950X") != string::npos || CPUBrandString.find("5950X") != string::npos || CPUBrandString.find("3950X") != string::npos)
    {
        cout << "found! apply ccd1mask" << endl;
        return ccd1mask;
    }
    else if (CPUBrandString.find("7900X3D") != string::npos)
    {
        cout << "found! apply ccd0mask12c" << endl;
        return ccd0mask12c;
    }
    else if (CPUBrandString.find("7900") != string::npos || CPUBrandString.find("5900") != string::npos || CPUBrandString.find("3900") != string::npos)
    {
        cout << "found! apply ccd1mask12c" << endl;
        return ccd1mask12c;
    }

    // intel e-core shitshow
    if (CPUBrandString.find("13900") != string::npos)
    {
        cout << "found! apply intel8p16ecoremask" << endl;
        return intel8p16ecoremask;
    }
    else if (CPUBrandString.find("12900") != string::npos || CPUBrandString.find("13700") != string::npos)
    {
        cout << "found! apply intel8p8ecoremask" << endl;
        return intel8p8ecoremask;
    }
    else if (CPUBrandString.find("12700") != string::npos)
    {
        cout << "found! apply intel8p4ecoremask" << endl;
        return intel8p4ecoremask;
    }
    else if (CPUBrandString.find("13600") != string::npos || CPUBrandString.find("13500") != string::npos)
    {
        cout << "found! apply intel6p8ecoremask" << endl;
        return intel6p8ecoremask;
    }
    else if (CPUBrandString.find("12600K") != string::npos || CPUBrandString.find("13400") != string::npos)
    {
        cout << "found! apply intel6p4ecoremask" << endl;
        return intel6p4ecoremask;
    }
    else if (CPUBrandString.find("Intel") != string::npos)
    {
        // trust the scheduler :P
        return 0;
    }

    return 0;
}

int main()
{
    //freopen("output.log", "w", stdout);
    //freopen("error.log", "w", stderr);

    while (true)
    {
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

                        if (0 != mask)
                        {
                            HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, entry.th32ProcessID);

                            BOOL success = SetProcessAffinityMask(hProcess, mask);

                            CloseHandle(hProcess);
                        }
                    }
                }

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

        chrono::milliseconds timespan(1000);
        this_thread::sleep_for(timespan);
    }
}