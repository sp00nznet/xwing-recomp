/*
 * settings: the remaster's options, kept in xwing.ini beside the program.
 *
 *   F10            the settings menu (also right-click outside the picture)
 *   F11, Alt+Enter borderless fullscreen
 *   F12            scaling of the 2D picture: sharp, smooth, CRT, nearest, integer
 */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>

#include "gpu.h"
#include "settings.h"

int g_flight_mouse = 1;     /* fly with the mouse when no joystick is plugged in */
int g_flight_pad = 1;       /* an Xbox pad drives the joystick */
int g_mods = 1;             /* mods/files overlay the game's files */
int g_hw3d = 1;             /* the game's 3D hardware mode, drawn by gpu.c at the window's resolution */

static char g_ini[MAX_PATH];

static int get(const char* k, int def) { return GetPrivateProfileIntA("xwing", k, def, g_ini); }
static void put(const char* k, int v) { char b[16]; _snprintf(b, sizeof b, "%d", v); WritePrivateProfileStringA("xwing", k, b, g_ini); }

void settings_load(void) {
    GetModuleFileNameA(NULL, g_ini, MAX_PATH);
    char* slash = strrchr(g_ini, '\\');
    lstrcpyA(slash ? slash + 1 : g_ini, "xwing.ini");
    g_gpu.mode = get("scaling", g_gpu.mode) % GPU_NMODES;
    g_gpu.filter = get("filter", g_gpu.filter);
    g_gpu.widescreen = get("widescreen", g_gpu.widescreen);
    g_gpu.hd_textures = get("hd_textures", g_gpu.hd_textures);
    g_gpu.msaa = get("msaa", g_gpu.msaa);
    g_gpu.fullscreen = get("fullscreen", 0);
    g_flight_mouse = get("flight_mouse", g_flight_mouse);
    g_flight_pad = get("flight_pad", g_flight_pad);
    g_mods = get("mods", g_mods);
    g_hw3d = get("hires_3d", g_hw3d);
}

void settings_save(void) {
    put("scaling", g_gpu.mode);
    put("filter", g_gpu.filter);
    put("widescreen", g_gpu.widescreen);
    put("hd_textures", g_gpu.hd_textures);
    put("msaa", g_gpu.msaa);
    put("fullscreen", g_gpu.fullscreen);
    put("flight_mouse", g_flight_mouse);
    put("flight_pad", g_flight_pad);
    put("mods", g_mods);
    put("hires_3d", g_hw3d);
}

enum { M_MODE = 100, M_FILTER = 200, M_MSAA = 300, M_WIDE = 400, M_HD, M_DUMP, M_FULL, M_MOUSE, M_PAD, M_MODS, M_HW3D };

void settings_menu(HWND w, int x, int y) {
    HMENU m = CreatePopupMenu(), sc = CreatePopupMenu(), tf = CreatePopupMenu(), aa = CreatePopupMenu();
    for (int i = 0; i < GPU_NMODES; i++)
        AppendMenuA(sc, MF_STRING | (g_gpu.mode == i ? MF_CHECKED : 0), M_MODE + i, gpu_mode_names[i]);
    static const char* const filters[] = { "Nearest (the original)", "Bilinear", "Anisotropic" };
    for (int i = 0; i < 3; i++)
        AppendMenuA(tf, MF_STRING | (g_gpu.filter == i ? MF_CHECKED : 0), M_FILTER + i, filters[i]);
    static const int ms[] = { 1, 2, 4, 8 };
    for (int i = 0; i < 4; i++) {
        char t[16];
        _snprintf(t, sizeof t, ms[i] == 1 ? "Off" : "%dx", ms[i]);
        AppendMenuA(aa, MF_STRING | (g_gpu.msaa == ms[i] ? MF_CHECKED : 0), M_MSAA + ms[i], t);
    }
    AppendMenuA(m, MF_STRING | (g_hw3d ? MF_CHECKED : 0), M_HW3D, "High-resolution 3D (next flight)");
    AppendMenuA(m, MF_POPUP, (UINT_PTR)sc, "Scaling (F12)");
    AppendMenuA(m, MF_POPUP, (UINT_PTR)tf, "Texture filtering");
    AppendMenuA(m, MF_POPUP, (UINT_PTR)aa, "Antialiasing");
    AppendMenuA(m, MF_STRING | (g_gpu.widescreen ? MF_CHECKED : 0), M_WIDE, "Widescreen 3D");
    AppendMenuA(m, MF_STRING | (g_gpu.hd_textures ? MF_CHECKED : 0), M_HD, "HD textures (mods\\textures)");
    AppendMenuA(m, MF_STRING | (g_gpu.dump_textures ? MF_CHECKED : 0), M_DUMP, "Dump textures (mods\\dump)");
    AppendMenuA(m, MF_STRING | (g_mods ? MF_CHECKED : 0), M_MODS, "Mod files (mods\\files)");
    AppendMenuA(m, MF_SEPARATOR, 0, NULL);
    AppendMenuA(m, MF_STRING | (g_flight_mouse ? MF_CHECKED : 0), M_MOUSE, "Fly with the mouse");
    AppendMenuA(m, MF_STRING | (g_flight_pad ? MF_CHECKED : 0), M_PAD, "Fly with a gamepad");
    AppendMenuA(m, MF_SEPARATOR, 0, NULL);
    AppendMenuA(m, MF_STRING | (g_gpu.fullscreen ? MF_CHECKED : 0), M_FULL, "Fullscreen (F11)");
    POINT p = { x, y };
    ClientToScreen(w, &p);
    int c = TrackPopupMenu(m, TPM_RETURNCMD | TPM_NONOTIFY, p.x, p.y, 0, w, NULL);
    DestroyMenu(m);
    if (!c) return;
    if (c >= M_MODE && c < M_MODE + GPU_NMODES) g_gpu.mode = c - M_MODE;
    else if (c >= M_FILTER && c < M_FILTER + 3) g_gpu.filter = c - M_FILTER;
    else if (c > M_MSAA && c <= M_MSAA + 8) g_gpu.msaa = c - M_MSAA;
    else if (c == M_WIDE) g_gpu.widescreen ^= 1;
    else if (c == M_HD) g_gpu.hd_textures ^= 1;
    else if (c == M_DUMP) g_gpu.dump_textures ^= 1;
    else if (c == M_MODS) g_mods ^= 1;
    else if (c == M_HW3D) { g_hw3d ^= 1; settings_apply_prefs(); }
    else if (c == M_MOUSE) g_flight_mouse ^= 1;
    else if (c == M_PAD) g_flight_pad ^= 1;
    else if (c == M_FULL) gpu_set_fullscreen(!g_gpu.fullscreen);
    gpu_resize();
    settings_save();
}

/* The game keeps its display mode in prefs.cfg (byte 0x1B: 0 320x200, 1 640x480x8,
 * 2 640x480x16, 3 3D hardware) but never writes a changed one back: the mode
 * is set there before the game reads it. */
void settings_apply_prefs(void) {
    HANDLE f = CreateFileA("prefs.cfg", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (f == INVALID_HANDLE_VALUE) return;
    unsigned char b[0x60];
    DWORD n = 0;
    if (ReadFile(f, b, sizeof b, &n, NULL) && n > 0x1B) {
        unsigned char want = g_hw3d ? 3 : 2;
        if (b[0x1B] != want) {
            b[0x1B] = want;
            SetFilePointer(f, 0, NULL, FILE_BEGIN);
            WriteFile(f, b, n, &n, NULL);
        }
    }
    CloseHandle(f);
}

int settings_key(HWND w, UINT vk, int alt) {
    if (vk == VK_F11 || (alt && vk == VK_RETURN)) { gpu_set_fullscreen(!g_gpu.fullscreen); settings_save(); return 1; }
    if (vk == VK_F12) { g_gpu.mode = (g_gpu.mode + 1) % GPU_NMODES; gpu_resize(); settings_save(); return 1; }
    if (vk == VK_F10) { RECT r; GetClientRect(w, &r); settings_menu(w, r.right / 2, r.bottom / 2); return 1; }
    return 0;
}
