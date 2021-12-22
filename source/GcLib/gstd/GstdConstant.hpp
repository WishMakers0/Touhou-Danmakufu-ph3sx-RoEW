#pragma once
#include "../pch.h"

constexpr const int STANDARD_FPS = 60;

const std::wstring DNH_EXE_NAME = L"th_roew.exe";
const std::wstring DNH_VERSION = L"v0.01a"; //v.1.32a

constexpr const uint64_t _GAME_VERSION_RESERVED = /*PC*/98;		//OWO!!!!! //naudiz sinned :c i fixed it
constexpr const uint64_t _GAME_VERSION_MAJOR = 0;
constexpr const uint64_t _GAME_VERSION_SUBMAJOR = 0;
constexpr const uint64_t _GAME_VERSION_MINOR = 1;
constexpr const uint64_t _GAME_VERSION_REVISION = 0;

//00000000 00000000 | 00000000 00000000 | 00000000 00000000 | 00000000 00000000
//<-RESERVED--><----MAJOR----> <-----SUBMAJOR----> <------MINOR------> <REVIS->
constexpr const uint64_t GAME_VERSION_NUM = ((_GAME_VERSION_RESERVED & 0xfff) << 52)
| ((_GAME_VERSION_MAJOR & 0xfff) << 40) | ((_GAME_VERSION_SUBMAJOR & 0xffff) << 24)
| ((_GAME_VERSION_MINOR & 0xffff) << 8) | (_GAME_VERSION_REVISION & 0xff);