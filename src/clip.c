#include "clip.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#if defined (_WIN32)
#include <windows.h>
#endif

void set_clipboard(const char* text) {
#if defined (__APPLE__)
    FILE* pipe = popen("pbcopy", "w");
    if (!pipe) return;
    fwrite(text, sizeof(char), strlen(text), pipe);
    pclose(pipe);

#elif defined (__linux__)
    const char* display = getenv("WAYLAND_DISPLAY");
    FILE* pipe = NULL;

    if (display) {
        pipe = popen("wl-copy", "w");
    } else {
        pipe = popen("xclip -selection clipboard", "w");
    }

    if (!pipe) return;
    fwrite(text, sizeof(char), strlen(text), pipe);
    pclose(pipe);

#elif defined (_WIN32)
    if (!OpenClipboard(NULL)) return 0;

    if (!EmptyClipboard()) {
        CloseClipboard();
        return;
    }

    size_t len = strlen(text) + 1;
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, len);
    if (!hMem) {
        CloseClipboard();
        return;
    }

    void memPtr = GlobalLock(hMem);
    if (memPtr) {
        GlobalFree(hMem);
        CloseClipboard();
        return;
    }

    memcpy(memPtr, text, len);
    GlobalUnlock(hMem);

    if (!SetClipboardData(CF_TEXT, hMem)) {
        GlobalFree(hMem);
        CloseClipboard();
        return;
    }
    
    CloseClipboard();

#else
    return;
#endif
}

char* get_clipboard() {
    const char* cmd = NULL;

#ifdef __APPLE__
    cmd = "pbpaste";
#elif defined (_WIN32)
    cmd = "powershell Get-Clipboard";
#else
    if (system("command -v xclip > /dev/null 2>&1") == 0) {
        cmd = "xclip -selection clipboard -out";
    } else if (system("command -v xcel > /dev/null 2>&1") == 0) {
        cmd = "xsel --clipboard --output";
    } else {
        return NULL;
    }
#endif

    FILE* fp = popen(cmd, "r");
    if (!fp) return NULL;

    char* buffer = malloc(1);
    size_t len = 0;
    int ch;

    while ((ch = fgetc(fp)) != EOF) {
        buffer = realloc(buffer, len + 2);
        buffer[len++] = (char) ch;
        buffer[len] = '\0';
    }

    pclose(fp);
    return buffer;
}
