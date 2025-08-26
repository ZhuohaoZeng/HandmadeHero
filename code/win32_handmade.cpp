


#include <windows.h>

#define internal static 
#define local_persist static //After being assigned in the first place, the next time the code reach it, it will not being reassignmend. Instead it keeps what ever it have to next line
#define global_variable static 

//TODO: Will change to something that is not global
global_variable bool Running;
global_variable BITMAPINFO BitmapInfo;
global_variable VOID *BitmapMemory;
 

internal void 
Win32ResizeDIBSection(int Width, int Height)
{
    if (BitmapMemory)
    {
        VirtualFree(BitmapMemory, 0, MEM_RELEASE);
    }
    BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader);
    BitmapInfo.bmiHeader.biWidth = Width;
    BitmapInfo.bmiHeader.biHeight = Height;
    BitmapInfo.bmiHeader.biPlanes = 1;
    BitmapInfo.bmiHeader.biBitCount = 32;
    BitmapInfo.bmiHeader.biCompression = BI_RGB;
    int BytesPerPixel = 4;
    int BitMemorySize = (Width * Height) * BytesPerPixel;
    BitmapMemory = VirtualAlloc(0, BitMemorySize, MEM_COMMIT, PAGE_READWRITE);
}


internal void 
Win32UpdateWindow(HDC DeviceContext, int X, int Y, int Width, int Height)
{
    StretchDIBits(DeviceContext,
                  X, Y, Width, Height,
                  X, Y, Width, Height,
                  BitmapMemory,
                  &BitmapInfo,
                  DIB_RGB_COLORS, SRCCOPY);
}

LRESULT CALLBACK 
Win32MainWindowCallback(HWND Window,
                   UINT Message,
                   WPARAM WParam,
                   LPARAM LParam)
{
    LRESULT Result = 0;
    switch(Message){
        case WM_SIZE: 
        {
            RECT ClientRect;
            GetClientRect(Window, &ClientRect);
            int Height = ClientRect.bottom - ClientRect.top;
            int Width = ClientRect.right - ClientRect.left;
            Win32ResizeDIBSection(Width, Height);
            OutputDebugStringA("WM_SIZE\n");
        }break;
        case WM_CLOSE:
        {
            //TODO: Handel this as a message to the user?
            Running = false;
            OutputDebugStringA("WM_CLOSE\n");
        }break;
        case WM_DESTROY:
        {
            //TODO: Handle this as an error?
            Running = false;
            OutputDebugStringA("WM_DESTROY\n");
        }break;
        case WM_ACTIVATEAPP:
        {
            OutputDebugStringA("WM_ACTIVATEAPP\n");
        }break;
        case WM_PAINT:
        {
            PAINTSTRUCT Paint;
            HDC DeviceContext = BeginPaint(Window, &Paint);
            int X = Paint.rcPaint.left;
            int Y = Paint.rcPaint.top;
            int Height = Paint.rcPaint.bottom - Paint.rcPaint.top;
            int Width = Paint.rcPaint.right - Paint.rcPaint.left;
            Win32UpdateWindow(DeviceContext, X, Y, Width, Height);
            EndPaint(Window, &Paint);
        }break;

        default:{
            // OutputDebugStringA("default\n");
            Result = DefWindowProc(Window, Message, WParam, LParam);
        }break;
    }
    return Result;
}


int CALLBACK 
WinMain(HINSTANCE Instance,
        HINSTANCE PrevInstance,
        LPSTR     CommandLine,
        int       ShowCode)
{
    WNDCLASS WindowClass = {};
    WindowClass.style = CS_OWNDC|CS_HREDRAW|CS_VREDRAW;
    WindowClass.lpfnWndProc = Win32MainWindowCallback;
    WindowClass.hInstance = Instance;
//    WindowClass.hIcon;
    WindowClass.lpszClassName = "HandmadeHeroWindowClass";
    if(RegisterClass(&WindowClass)){
        HWND WindowHandle = 
            CreateWindowEx(
                0, 
                WindowClass.lpszClassName, 
                "Handmade Hero", 
                WS_OVERLAPPEDWINDOW|WS_VISIBLE, 
                CW_USEDEFAULT, 
                CW_USEDEFAULT, 
                CW_USEDEFAULT, 
                CW_USEDEFAULT, 
                0, 
                0, 
                Instance, 
                0);
        if(WindowHandle)
        {
            Running = true;
            while(Running)
            {
                MSG Message;
                BOOL MessageResult = GetMessageA(&Message, 0 , 0, 0);
                if(MessageResult > 0)
                {
                    TranslateMessage(&Message);
                    DispatchMessage(&Message);
                }
                else 
                {
                    break;
                }
            }
            
        }
        else 
        {
            //TODO: LOGGING
        }
    }
    else {
        //TODO: LOGGING
    }
    return (0);
}