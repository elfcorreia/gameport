#include "framebuffer.h"
#include <iostream>
#include <vector>
#include <windows.h>
#include <sstream>
#include <gdiplus.h>

using std::cerr;
using std::cout;
using std::stringstream;

static void panic(const std::string& message) {
    LPTSTR errorText = NULL;
    FormatMessage(
        // use system message tables to retrieve error text
        FORMAT_MESSAGE_FROM_SYSTEM
        // allocate buffer on local heap for error text
        |FORMAT_MESSAGE_ALLOCATE_BUFFER
        // Important! will fail otherwise, since we're not 
        // (and CANNOT) pass insertion parameters
        |FORMAT_MESSAGE_IGNORE_INSERTS,  
        NULL,    // unused with FORMAT_MESSAGE_FROM_SYSTEM
        GetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &errorText,  // output 
        0, // minimum size for output buffer
        NULL   // arguments - see note    
    );

    if (errorText != nullptr) {
        stringstream ss;
        ss << message << "\n" << errorText;   
        MessageBox(NULL, ss.str().c_str(), "Error", MB_ICONERROR | MB_OK);
        LocalFree(errorText);
        errorText = NULL;
    }
    exit(1);
}

LRESULT CALLBACK winproc(HWND hwnd, UINT wm, WPARAM wp, LPARAM lp);

int* fb_init(framebuffer_options* options, framebuffer* instance) {
    instance->options = *options;

    if (instance->options.verbose) {
        cerr << "fb_init: GdiplusStartup()\n";
    }
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken; 
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);

    if (instance->options.verbose) {
        cerr << "fb_init: GetModuleHandle(nullptr)\n";
    }
    HINSTANCE modulo = GetModuleHandle(nullptr);

    // registra uma classe de janela
    if (instance->options.verbose) {
        cerr << "fb_init: RegisterClassEx()\n";
    }
    WNDCLASSEX classeJanela;
    classeJanela.cbSize = sizeof(WNDCLASSEX);
    classeJanela.style = 0;
    classeJanela.lpfnWndProc = winproc; 
    classeJanela.cbClsExtra = 0;
    classeJanela.cbWndExtra = 0;
    classeJanela.hInstance = modulo;
    classeJanela.hIcon = nullptr;
    classeJanela.hCursor = LoadCursor(nullptr, IDC_ARROW);
    classeJanela.hbrBackground = nullptr;
    classeJanela.lpszMenuName = nullptr;
    classeJanela.lpszClassName = "janela";
    classeJanela.hIconSm = nullptr;
    
    if (!RegisterClassEx(&classeJanela)) {
        panic("Não foi possível registrar a classe da janela!");
    }
    
    if (instance->options.verbose) {
        cerr << "fb_init: CreateWindowEx()\n";
    }

    instance->scaled_width = instance->options.width * instance->options.pixel_scale;
    instance->scaled_height = instance->options.height * instance->options.pixel_scale;
    instance->buffer_size = instance->scaled_width * instance->scaled_height;

    HWND janela = CreateWindowEx(
        0,
        "janela",
        nullptr,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        instance->scaled_width,
        instance->scaled_height,
        nullptr,
        nullptr,
        modulo,
        0
    );
    if (!janela) {
        panic("Não foi possível criar a janela!");        
    }
    
    if (instance->options.verbose) {
        cerr << "fb_init: creating pixel buffer for " << instance->buffer_size << " pixel(s)\n";
    }
    instance->buffer = new int[instance->buffer_size];

    if (instance->options.verbose) {
        cerr << "fb_init: ShowWindow()\n";
    }
    ShowWindow(janela, SW_NORMAL);
    if (instance->options.verbose) {
        cerr << "fb_init: UpdateWindow()\n";
    }
    UpdateWindow(janela);
    instance->gui_thread = std::thread([instance, &gdiplusToken]() {
        if (instance->options.verbose) {
            cerr << "main_loop: entering main loop\n";
        }
        MSG msg = {};
        while (GetMessage(&msg, nullptr, 0, 0) > 0) {            
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        Gdiplus::GdiplusShutdown(gdiplusToken);
    });
    return instance->buffer;
}

void fb_update(framebuffer* instance) {
    cout << "fb_update()\n";
}

void fb_close(framebuffer* instance) {
    cout << "fb_close()\n";
    if (instance->gui_thread.joinable()) {
        instance->gui_thread.join();
    }
}

LRESULT CALLBACK winproc(HWND hwnd, UINT wm, WPARAM wp, LPARAM lp) {
    HDC hdc;
    PAINTSTRUCT ps;

    switch (wm) {
        case WM_PAINT:
            hdc = BeginPaint(hwnd, &ps);
            {
                Gdiplus::Graphics graphics(hdc);                
                // Image image();
                // graphics.DrawImage(&image, 0, 0);
            }
            EndPaint(hwnd, &ps);            
            cerr << "WM_PAINT\n";
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        default:
            return DefWindowProc(hwnd, wm, wp, lp);
    }
}