﻿#define LONGUI_WITH_DEFAULT_HEADER
#define _CRT_SECURE_NO_WARNINGS
#include "../../LongUI/LongUI.h"

constexpr char* hello_world =
u8R"longui_demo_xml(<?xml version="1.0" encoding="utf-8"?>
<Window>
    <Label text="Hello, world!"/>
</Window>
)longui_demo_xml";

int WINAPI WinMain(HINSTANCE, HINSTANCE, char*, int) {
    if (SUCCEEDED(::OleInitialize(nullptr))) {
        UIManager.Initialize();
        UIManager.CreateUIWindow<>(hello_world);
        UIManager.Run();
        UIManager.UnInitialize();
        ::OleUninitialize();
    }
    return 0;
}


#ifdef _DEBUG
#   pragma comment(lib, "../../Debug/longui")
#else
#   pragma comment(lib, "../../Release/longui")
#endif


#ifndef PUGIXML_HEADER_ONLY
#ifdef _DEBUG
#   pragma comment(lib, "../../Debug/pugixml")
#else
#   pragma comment(lib, "../../Release/pugixml")
#endif
#endif

#ifdef _DEBUG
#   pragma comment(lib, "../../Debug/dlmalloc")
#else
#   pragma comment(lib, "../../Release/dlmalloc")
#endif
