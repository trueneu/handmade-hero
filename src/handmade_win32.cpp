#include <cstdio>
#include <stdint.h>
#include <windows.h>

#define internal static
#define local_persist static
#define global_variable static

typedef uint8_t  uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef int8_t  int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

// todo: this is a global for now
global_variable bool       Running;
global_variable BITMAPINFO BitmapInfo;
global_variable void      *BitmapMemory;
global_variable int        BitmapWidth;
global_variable int        BitmapHeight;

global_variable int BytesPerPixel = 4;

internal void RenderWeirdGradient(int XOffset, int YOffset) {
  int Width = BitmapWidth;
  int Height = BitmapHeight;

  int    Pitch = Width * BytesPerPixel;
  uint8 *Row = (uint8 *)BitmapMemory;

  for (int Y = 0; Y < BitmapHeight; ++Y) {
    uint32 *Pixel = (uint32 *)Row;
    for (int X = 0; X < BitmapWidth; ++X) {
      uint8 Blue = (X + XOffset);
      uint8 Green = (Y + YOffset);
      uint8 Red = 0;
      uint8 Pad = 0;

      *Pixel++ = Blue | (Green << 8) | (Red << 16) | (Pad << 24);
    }
    Row += Pitch;
  }
}

internal void Win32ResizeDIBSection(int Width, int Height) {
  // todo: bulletproof this
  // maybe don't free first, free after, then free first if that fails

  if (BitmapMemory) {
    VirtualFree(BitmapMemory, 0, MEM_RELEASE);
  }

  BitmapWidth = Width;
  BitmapHeight = Height;

  BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader);
  BitmapInfo.bmiHeader.biWidth = BitmapWidth;
  BitmapInfo.bmiHeader.biHeight = -BitmapHeight; // top-down if negative
  BitmapInfo.bmiHeader.biPlanes = 1;
  BitmapInfo.bmiHeader.biBitCount = 32;
  BitmapInfo.bmiHeader.biCompression = BI_RGB;

  int BitmapMemorySize = BytesPerPixel * BitmapWidth * BitmapHeight;
  BitmapMemory = VirtualAlloc(0, BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);

  // todo: probably want to clear this to black
}

internal void Win32UpdateWindow(HDC DeviceContext, RECT *WindowRect, int X, int Y, int Width, int Height) {

  int WindowWidth = WindowRect->right - WindowRect->left;
  int WindowHeight = WindowRect->bottom - WindowRect->top;
  StretchDIBits(DeviceContext,
                0,
                0,
                BitmapWidth,
                BitmapHeight,
                0,
                0,
                WindowWidth,
                WindowHeight,
                BitmapMemory,
                &BitmapInfo,
                DIB_RGB_COLORS,
                SRCCOPY);
}

LRESULT Win32MainWindowCallback(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam) {
  LRESULT Result = 0;

  switch (Message) {
  case WM_SIZE: {
    RECT ClientRect;
    GetClientRect(Window, &ClientRect);
    int Width = ClientRect.right - ClientRect.left;
    int Height = ClientRect.bottom - ClientRect.top;

    Win32ResizeDIBSection(Width, Height);
  } break;
  case WM_DESTROY: {
    // todo: handle this as an error - re-create window?
    Running = false;
  } break;
  case WM_CLOSE: {
    // todo: handle this with a message to the user?
    Running = false;
  } break;
  case WM_ACTIVATEAPP: {
    OutputDebugStringA("WM_ACTIVATEAPP\n");
  } break;
  case WM_PAINT: {
    PAINTSTRUCT Paint;
    HDC         DeviceContext = BeginPaint(Window, &Paint);
    int         X = Paint.rcPaint.left;
    int         Y = Paint.rcPaint.top;
    int         Width = Paint.rcPaint.right - Paint.rcPaint.left;
    int         Height = Paint.rcPaint.bottom - Paint.rcPaint.top;

    RECT ClientRect;
    GetClientRect(Window, &ClientRect);

    Win32UpdateWindow(DeviceContext, &ClientRect, X, Y, Width, Height);
    EndPaint(Window, &Paint);
  } break;

  default: {
    //    OutputDebugStringA("default\n");
    Result = DefWindowProc(Window, Message, WParam, LParam);
  } break;
  }
  return (Result);
}

int CALLBACK WinMain(HINSTANCE Instance, HINSTANCE PrevInstance, LPSTR CommandLine, int ShowCode) {
  WNDCLASS WindowClass = {};

  // todo: check if hredraw/vredraw/owndc still matter
  WindowClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
  WindowClass.lpfnWndProc = Win32MainWindowCallback;
  WindowClass.hInstance = Instance;
  //  WindowClass.hIcon = ;
  //  WindowClass.hCursor = ;
  WindowClass.lpszClassName = "HandmadeHeroWindowClass";

  if (RegisterClass(&WindowClass)) {
    HWND Window = CreateWindowEx(0,
                                 WindowClass.lpszClassName,
                                 "Handmade Hero",
                                 WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                                 CW_USEDEFAULT,
                                 CW_USEDEFAULT,
                                 CW_USEDEFAULT,
                                 CW_USEDEFAULT,
                                 0,
                                 0,
                                 Instance,
                                 0);
    if (Window) {
      Running = true;
      int XOffset = 0;
      int YOffset = 0;
      while (Running) {
        MSG Message;
        while (PeekMessage(&Message, 0, 0, 0, PM_REMOVE)) {
          if (Message.message == WM_QUIT) {
            Running = false;
          }

          TranslateMessage(&Message);
          DispatchMessage(&Message);
        }
        RenderWeirdGradient(XOffset, YOffset);

        HDC DeviceContext = GetDC(Window);

        RECT ClientRect;
        GetClientRect(Window, &ClientRect);

        int WindowWidth = ClientRect.right - ClientRect.left;
        int WindowHeight = ClientRect.bottom - ClientRect.top;
        Win32UpdateWindow(DeviceContext, &ClientRect, 0, 0, WindowWidth, WindowHeight);

        ReleaseDC(Window, DeviceContext);

        XOffset++;
        YOffset++;
      }

    } else {
      // todo: logging
    }
  } else {
    // todo: logging
  }

  return (0);
}
