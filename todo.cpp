#include <windows.h>
#include "resource.h"

// C:\MinGW\bin\windres.exe C:\to_do_list_c++\resource.rc -O coff -o C:\to_do_list_c++\resource.o
// C:\MinGW\bin\g++.exe -o C:\to_do_list_c++\myprogram.exe C:\to_do_list_c++\todo.cpp C:\to_do_list_c++\resource.o -lgdi32

MSG Message;
HWND g_hButtonAdd; // deklaracja globalnego uchwytu (hwnd to wskaźnik do struktury)
HWND g_hButtonDelete;
HWND hText;
LPSTR Bufor;
HWND hListBox;
HFONT hFont;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void AddToTodoList();

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    char name[] = "Class";
    LPSTR ClassName = name;
    // WYPEŁNIANIE STRUKTURY
    WNDCLASSEX wc;

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = CreateSolidBrush(RGB(225, 240, 238));
    wc.lpszMenuName = NULL;
    wc.lpszClassName = ClassName;
    wc.hIconSm = LoadIcon(hInstance, IDI_APPLICATION);
    int width = 512;
    int height = 620;
    int buttonWidth = 221;
    int buttonHeight = 30;

    // REJESTROWANIE KLASY OKNA
    if (!RegisterClassEx(&wc))
    {
        MessageBox(NULL, "Something went wrong...", NULL, MB_ICONEXCLAMATION + MB_OK); // uchwyt, tekst, tytuł, kombinacja stylów okienka
        return 1;
    }

    // TWORZENIE OKNA
    HWND hwnd; // uchwyt do okna (menu - typ uchwytu HMENU, czcionka - HFONT, okno - HWND, blok pamięci- HGLOBAL lub HLOCAL itd.)
    hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
    // hFont = CreateFont(18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, TEXT("Arial"));

    // WYZNACZENIE ŚRODKA EKRANU (do użycia w CreateWindowsEx)
    RECT rect;
    GetClientRect(GetDesktopWindow(), &rect); // funkcja zwraca lokalne koordynaty rozmiaru okna (?)
    // uchwyt do okna (który pobiera się getdesktopwindow()), wskaźnik który będzie zawierał wymiary okna (left i top są równe 0, right i bottom są równe wymiarom okna)
    rect.left = (rect.right / 2) - (width / 2);
    rect.top = (rect.bottom / 2) - (height / 2);

    hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, ClassName, "To-do list", WS_OVERLAPPEDWINDOW, rect.left, rect.top, width, height, NULL, NULL, hInstance, NULL);

    // DODATKOWE KOMPONENTY
    g_hButtonAdd = CreateWindowEx(0, "BUTTON", "Add", WS_CHILD | WS_VISIBLE, 25, 520, buttonWidth, buttonHeight, hwnd, NULL, hInstance, NULL);
    SendMessage(g_hButtonAdd, WM_SETFONT, (WPARAM)hFont, 0);
    g_hButtonDelete = CreateWindowEx(0, "BUTTON", "Delete", WS_CHILD | WS_VISIBLE, buttonWidth + 25, 520, buttonWidth, buttonHeight, hwnd, NULL, hInstance, NULL);
    SendMessage(g_hButtonDelete, WM_SETFONT, (WPARAM)hFont, 0);
    hText = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL, 25, 487, 2 * buttonWidth, 25, hwnd, NULL, hInstance, NULL);
    SendMessage(hText, WM_SETFONT, (WPARAM)hFont, 0);

    // TEKST W POLU
    HWND hStatic = CreateWindowEx(0, "STATIC", NULL, WS_CHILD | WS_VISIBLE | SS_CENTER, 25, 457, 2 * buttonWidth, 30, hwnd, NULL, hInstance, NULL);
    SetWindowText(hStatic, "What do you have to do?... ");
    SendMessage(hStatic, WM_SETFONT, (WPARAM)hFont, 0);

    HWND hStaticTitle = CreateWindowEx(0, "STATIC", NULL, WS_CHILD | WS_VISIBLE | SS_CENTER, 5, 0, width - 40, 20, hwnd, NULL, hInstance, NULL);
    SetWindowText(hStaticTitle, "Your to-do list");
    SendMessage(hStaticTitle, WM_SETFONT, (WPARAM)hFont, 0);

    // LISTA
    hListBox = CreateWindowEx(0, "LISTBOX", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 5, 20, 241, 2 * height / 3, hwnd, NULL, hInstance, NULL);
    SendMessage(hListBox, WM_SETFONT, (WPARAM)hFont, 0);

    if (hwnd == NULL)
    {
        MessageBox(NULL, "Something went wrong...", NULL, MB_ICONEXCLAMATION + MB_OK);
        return 1;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // Pętla komunikatów
    while (GetMessage(&Message, NULL, 0, 0))
    {
        TranslateMessage(&Message);
        DispatchMessage(&Message);
        DeleteObject(hFont);
    }
    return Message.wParam;
}

// OBSŁUGA ZDARZEŃ
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static HBITMAP hBitmap;
    switch (msg)
    {
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    case WM_COMMAND: // WYWOŁANIE AKCJI PO WCIŚNIĘCIU PRZYCISKU
        if ((HWND)lParam == g_hButtonAdd)
        {
            AddToTodoList();
        }
        else if ((HWND)lParam == g_hButtonDelete)
        {
            int index = SendMessage(hListBox, LB_GETCURSEL, 0, 0);
            if (index != LB_ERR)
            {
                SendMessage(hListBox, LB_DELETESTRING, index, 0);
            }
        }
        break;
    case WM_KEYDOWN: // Jeszcze nie działa (wciśnięcie klawisza enter ma skutkować wciśnięciem przycisku add)
        if (wParam == VK_RETURN && GetFocus() == hText)
        {
            AddToTodoList();
        }
        break;

    // OBRAZEK
    case WM_CREATE:
        hBitmap = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP1));
        break;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        HDC hdcMem = CreateCompatibleDC(hdc);
        HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, hBitmap);

        BITMAP bm;
        GetObject(hBitmap, sizeof(bm), &bm);

        int posPicX = 251;
        int posPicY = 30;
        int picWidth = bm.bmWidth / 2;
        int picHeight = bm.bmHeight / 2;
        // Przed przeskalowaniem i zmianą położenia - BitBlt(hdc, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);
        StretchBlt(hdc, posPicX, posPicY, picWidth, picHeight, hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);

        SelectObject(hdcMem, hbmOld);
        DeleteDC(hdcMem);

        EndPaint(hwnd, &ps);
    }
    break;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    return 0;
}

void AddToTodoList()
{
    //  Pobieranie tekstu
    DWORD todoLength = GetWindowTextLength(hText);
    Bufor = (LPSTR)GlobalAlloc(GPTR, todoLength + 1);
    GetWindowText(hText, Bufor, todoLength + 1);
    SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)Bufor); // Element dodany do listy
    GlobalFree(Bufor);                                     // Wyczyszczenie zmiennej
    SetWindowText(hText, "");                              // Wyczyszczenie pola tekstowego
}
