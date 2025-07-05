#include "clip.h"

#include <stdio.h>
#include <string.h>

#if defined (_WIN32)
#include <windows.h>
#endif

int set_clipboard(const char* text) {
#if defined (__APPLE__)
    FILE* pipe = popen("pbcopy", "w");
    if (!pipe) return 0;
    fwrite(text, sizeof(char), strlen(text), pipe);
    return pclose(pipe) == 0;

#elif defined (__linux__)
    FILE* pipe = popen("xclip -selection clipboard", "w");
    if (!pipe) return 0;
    fwrite(text, sizeof(char), strlen(text), pipe);
    return pclose(pipe) == 0;

#elif defined (_WIN32)
    if (!OpenClipboard(NULL)) return 0;

    if (!EmptyClipboard()) {
        CloseClipboard();
        return 0;
    }

    size_t len = strlen(text) + 1;
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, len);
    if (!hMem) {
        CloseClipboard();
        return 0;
    }

    void memPtr = GlobalLock(hMem);
    if (memPtr) {
        GlobalFree(hMem);
        CloseClipboard();
        return 0;
    }

    memcpy(memPtr, text, len);
    GlobalUnlock(hMem);

    if (!SetClipboardData(CF_TEXT, hMem)) {
        GlobalFree(hMem);
        CloseClipboard();
        return 0;
    }
    
    CloseClipboard();
    return 1;

#else
    return 0;
#endif
}
