#include <locale.h>
#include "Hack.h"

static HWND hWnd = nullptr;
static const wchar_t* title = L"TIM-C语言";

void close()
{
    ShowWindow(hWnd, SW_SHOWDEFAULT);
    Sleep(2000);
}

int main()
{
    setlocale(LC_ALL, "");
    atexit(close);
    SetConsoleTitleW(title);
    Sleep(1000);
    hWnd = FindWindowW(nullptr, title);
    ShowWindow(hWnd, SW_HIDE);
    Hack h;
    return h.Run();
}