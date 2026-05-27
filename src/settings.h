#pragma once

#include "variable.h"

#include <windows.h>

struct Settings {
    char field_0[120];

    //0x0081CBD0
    Settings(const char *Source, const char *a3);

    int sub_81D010(const char *Source, int default_value);

    void sub_81CFA0(const char *a1, const char *a2, char *Type, DWORD cbData);

    bool sub_81CDC0(const char *Source, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData);

    bool sub_81CF80(const char *Source, BYTE Data);

    bool sub_81CC80(const char *Source, DWORD dwType, BYTE *lpData, DWORD cbData);

    bool sub_81D050(const char *a2, bool a3);

    static inline Var<float> GameSoundVolume{0x00965958};

    static inline Var<float> MusicVolume{0x00965AC0};

    static inline Var<int> SoundMode{0x00961924};

    static inline Var<bool> InvertCameraV{0x00961914};

    static inline Var<bool> InvertCameraH{0x009659A0};

    static inline Var<bool> MouseLook{0x00965BF4};
};

extern Var<Settings *> g_settings;
