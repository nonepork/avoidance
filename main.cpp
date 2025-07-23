#include <Windows.h>
#include <cmath>
#include <winuser.h>

bool fromX = false;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam,
                            LPARAM lParam) {
#define IDT_TIMER1 1
  switch (uMsg) {
  case WM_DESTROY:
    PostQuitMessage(0);
    return 0;
  case WM_CREATE:
    SetTimer(hwnd, IDT_TIMER1, 16, NULL); // ~60 FPS
    return 0;
  case WM_PAINT: {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);
    RECT rect;
    GetClientRect(hwnd, &rect);
    const char *text = "Close me then,\n I dare ya.";
    DrawTextA(hdc, text, strlen(text), &rect, DT_CENTER | DT_VCENTER);
    EndPaint(hwnd, &ps);
    return 0;
  }
  case WM_TIMER: {
    POINT cursorPos;
    GetCursorPos(&cursorPos);

    RECT rect;
    GetWindowRect(hwnd, &rect);
    int windowHeight = rect.bottom - rect.top;
    int windowWidth = rect.right - rect.left;

    RECT workArea;
    SystemParametersInfo(SPI_GETWORKAREA, 0, &workArea, 0);
    int screenHeight = workArea.bottom - workArea.top;
    int screenWidth = workArea.right - workArea.left;

    int closeX = rect.right - 20; // X button width
    int closeY = rect.top + 10;   // X button height

    int dx = cursorPos.x - closeX;
    int dy = cursorPos.y - closeY;

    float dist = sqrtf(dx * dx + dy * dy);

    if (dist < 50.0f) {
      float strength = 30.0f;
      int moveX =
          -(int)((dx / dist) * strength); // A cool way to use trigonometric
      int moveY = -(int)((dy / dist) * strength);

      int newX = rect.left + moveX;
      int newY = rect.top + moveY;

      if (newX <= 0) {
        newX = windowWidth + 6;
      } else if (newX + windowWidth >= screenWidth) {
        newX = screenWidth - windowWidth;
      }

      if (newY <= 0) {
        newY = windowHeight + 6;
      } else if (newY + windowHeight >= screenHeight) {
        newY = screenHeight - windowHeight;
      }

      SetWindowPos(hwnd, HWND_TOPMOST, newX, newY, 0, 0,
                   SWP_NOSIZE | SWP_NOZORDER);
    }
    return 0;
  }
  }
  return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow) {
  WNDCLASSW wc = {};
  wc.lpfnWndProc = WindowProc;
  wc.hInstance = hInstance;
  wc.lpszClassName = L"AvoidanceClass";

  RegisterClassW(&wc);

  HWND hwnd = CreateWindowExW(WS_EX_TOPMOST,              // Extended style
                              L"AvoidanceClass",          // Class name
                              L"avoidance",               // Window title
                              WS_OVERLAPPED | WS_SYSMENU, // Style
                              CW_USEDEFAULT, CW_USEDEFAULT, 150, 100, NULL,
                              NULL, hInstance, NULL);

  ShowWindow(hwnd, SW_SHOW);
  UpdateWindow(hwnd);

  MSG msg = {};
  while (GetMessage(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
}
