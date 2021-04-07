#include <utility>
#define UNICODE

#include "canvas.h"
#include <windows.h>
#include <gdiplus.h>
#include <stringapiset.h>
#include <string>
#include <thread>
#include <iostream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <functional>
#include <fcntl.h>
#include <io.h>

using std::function;
using std::string;
using std::wstring;
using std::thread;
using std::wstringstream;
using std::vector;
using std::unordered_map;
using std::getline;
using std::ios;
using std::wcerr;

using Gdiplus::Pen;
using Gdiplus::Graphics;
using Gdiplus::Color;
using Gdiplus::FontFamily;
using Gdiplus::Font;
using Gdiplus::PointF;
using Gdiplus::SolidBrush;
using Gdiplus::GraphicsPath;

struct canvas {        
    wstring title = L"Canvas";
    int top = 0;
    int left = 0;
    int width = 350;
    int height = 350;
    unsigned char current_color_red = 0;
    unsigned char current_color_green = 0;
    unsigned char current_color_blue = 0;
    unsigned char current_color_alpha = 255;
    unsigned char current_line_width = 1;
    wstring current_font_family  = L"Arial";
    int current_font_size = 12;
    int current_font_style  = Gdiplus::FontStyleRegular;

    vector<wstring> cmds;    
    wstringstream current_cmd;
    Graphics* graphics = nullptr;
    GraphicsPath* current_path = nullptr;
};

// variáveis globais
static const wchar_t CLASS_NAME[]  = L"Canvas";
static HINSTANCE hInstance;
static WNDCLASS window_class = {
    .style = CS_GLOBALCLASS | CS_HREDRAW | CS_VREDRAW,
    .lpfnWndProc = NULL, 
    .hInstance = NULL,   
    .lpszClassName = CLASS_NAME
 };
static HWND window_handler = NULL;
static thread window_thread; 
static canvas context;
static wstringstream path_buffer;

static const unordered_map<wstring, function<void()>> command_handlers = {
    { L"text", []() {
        int x, y;
        context.current_cmd >> x >> y;
        wstring text;
        getline(context.current_cmd, text);
        
        PointF      point(x, y);
        SolidBrush  solidBrush(Color(context.current_color_alpha, context.current_color_red, context.current_color_green, context.current_color_blue));
        FontFamily font_family = {context.current_font_family.c_str()};
        Font font(&font_family, context.current_font_size, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
        context.graphics->DrawString(text.c_str(), -1, &font, point, &solidBrush);
    }},
    {L"stroke_rect", []() {
        int x, y, width, height;
        context.current_cmd >> x >> y >> width >> height;

        Pen pen(Color(context.current_color_alpha, context.current_color_red, context.current_color_green, context.current_color_blue), context.current_line_width);
        context.graphics->DrawRectangle(&pen, x, y, width, height);
    }},
    {L"fill_rect", []() {
        int x, y, width, height;
        context.current_cmd >> x >> y >> width >> height;

        //Pen pen(Color(context.current_color_alpha, context.current_color_red, context.current_color_green, context.current_color_blue), context.current_line_width);
        SolidBrush solidBrush(Color(context.current_color_alpha, context.current_color_red, context.current_color_green, context.current_color_blue));
        context.graphics->SetCompositingQuality(Gdiplus::CompositingQualityGammaCorrected);
        context.graphics->FillRectangle(&solidBrush, x, y, width, height);
    }},   
    {L"path", []() {        
        GraphicsPath current_path;
        PointF last_point {0, 0};        
        while (!context.current_cmd.eof()) {
            wchar_t op;
            context.current_cmd >> op;
            switch (op) {
                case 'L': {
                    int x, y;
                    context.current_cmd >> x >> y;
                    current_path.AddLine(last_point.X, last_point.Y, (float) x, (float) y);
                    last_point.X = x;
                    last_point.Y = y;
                    wcerr << "L " << x << " " << y << "\n";
                    break;
                } 
                case 'M': {
                    int x, y;
                    context.current_cmd >> x >> y;
                    last_point.X = x;
                    last_point.Y = y;
                    wcerr << "M " << x << " " << y << "\n";
                    break;
                }
                case 'C': {
                    current_path.CloseFigure();
                }
            }
        }
        context.current_path = current_path.Clone();

        // int center_x, center_y, radius;
        // float start_angle, end_angle;
        // context.current_cmd >> center_x >> center_y >> radius >> start_angle >> end_angle;

        // int x = center_x - radius;
        // int y = center_y - radius;
        // int width = 2*radius;
        // float sweep_angle = fmod(end_angle, 360) - fmod(start_angle, 360);     
        // GraphicsPath arcPath;
        // Gdiplus::Rect arcRect(x, y, width, width);
        // arcPath.AddArc(arcRect, start_angle, -sweep_angle);        
        // SolidBrush solidBrush(Color(context.current_color_alpha, context.current_color_red, context.current_color_green, context.current_color_blue));        
        // context.graphics->FillPath(&solidBrush, &arcPath);
    }},
    {L"stroke", []() {
        Pen pen(Color(context.current_color_alpha, context.current_color_red, context.current_color_green, context.current_color_blue), context.current_line_width);        
        wcerr << "stroke\n";
        context.graphics->DrawPath(&pen, context.current_path);
    }},
    {L"fill", []() {
        SolidBrush solidBrush(Color(context.current_color_alpha, context.current_color_red, context.current_color_green, context.current_color_blue));        
        context.graphics->FillPath(&solidBrush, context.current_path);
    }},
    {L"set_line_width", []() {
        short width;
        context.current_cmd >> width;
        context.current_line_width = width;        
    }},
    {L"set_color", []() {
        short red, green, blue;
        context.current_cmd >> red >> green >> blue;
        context.current_color_red = red;
        context.current_color_green = green;
        context.current_color_blue = blue;
    }},
    {L"set_alpha", []() {
        short alpha;
        context.current_cmd >> alpha;
        context.current_color_alpha = alpha;
    }}
};

static void panic(const std::wstring& message) {
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
        (LPTSTR)&errorText,  // output 
        0, // minimum size for output buffer
        NULL);   // arguments - see note    

    if (errorText != nullptr) {
        wstringstream ss;
        ss << message << "\n" << errorText;   
        MessageBox(NULL, ss.str().c_str(), L"Error", MB_ICONERROR | MB_OK);
        LocalFree(errorText);
        errorText = NULL;
    }
    exit(1);
}

inline static void str_to_wstr(const string& src, wstring &out) {
    int nchars = MultiByteToWideChar(CP_UTF8, 0, src.c_str(), src.length(), NULL, 0);
    out.resize(nchars);
    MultiByteToWideChar(CP_UTF8, 0, src.c_str(), src.length(), &out[0], nchars);
}

LRESULT __stdcall window_procedure(HWND handle, unsigned windowMessage, WPARAM wParam, LPARAM lParam) {
    switch (windowMessage) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(handle, &ps);            
            context.graphics = new Graphics(hdc);
            //context.graphics->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
            
            // reset            
            context.current_color_red = 0;
            context.current_color_green = 0;
            context.current_color_blue = 0;
            context.current_color_alpha = 255;
            context.current_line_width = 1;

            // clearRect
            RECT clientRect;
            GetClientRect(handle, &clientRect);
            Gdiplus::Region region(Gdiplus::Rect(clientRect.top, clientRect.left, clientRect.right, clientRect.bottom));
            SolidBrush brush(Color(255, 255, 255, 255));
            context.graphics->FillRegion(&brush, &region);

            for (auto& cmd: context.cmds) {
                wcerr << "[window_procedure] cmd=" << cmd << "\n";
                context.current_cmd.clear();
                context.current_cmd.str(cmd);
                wstring op;
                context.current_cmd >> op;
                wcerr << "[window_procedure] op=" << op << "\n";
                if (command_handlers.find(op) == command_handlers.end()) {
                    wstring aux = L"No handler for ";                    
                    aux.append(op);
                    MessageBox(handle, aux.c_str(), L"", MB_ICONERROR + MB_OK);
                } else {
                    auto h = command_handlers.at(op);
                    h();
                }                
            }
            delete context.graphics;
            context.graphics = nullptr;            
            EndPaint(handle, &ps);
            return 0;
        }
        default:
            return DefWindowProc(handle, windowMessage, wParam, lParam);
    }    
}

/**
 * Implementação das funções definidas em canvas.h
 */

void canvas_create() {
    window_thread = thread([]() {
        Gdiplus::GdiplusStartupInput gdiplusStartupInput;
        ULONG_PTR gdiplusToken; 
        GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

        // get file module
        hInstance = GetModuleHandle(NULL);

        // register the window class
        window_class.hInstance = hInstance;
        window_class.lpfnWndProc = window_procedure;
        if (!RegisterClass(&window_class)) {
            panic(L"RegisterClass error!\n");
        }
        
        window_handler = CreateWindowEx(
            0,      
            CLASS_NAME, 
            context.title.c_str(), 
            WS_OVERLAPPEDWINDOW,
            context.top, context.left, context.width, context.height, 
            NULL, 
            NULL, 
            hInstance, 
            NULL
        );
        if (!window_handler) {
            panic(L"CreateWindow() error!");
        }

        ShowWindow(window_handler, SW_NORMAL);

        MSG msg = { };
        while (GetMessage(&msg, NULL, 0, 0) > 0)
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        
        Gdiplus::GdiplusShutdown(gdiplusToken);
    });
    //UnregisterClass(CLASS_NAME, hInstance);
}

void canvas_destroy() {
    if (window_thread.joinable()) {
        window_thread.join();
    }    
}

unsigned short canvas_top() {
    return context.top;
}

void canvas_set_top(unsigned short top) {
    context.top = top;
}

unsigned short canvas_left() {
    return context.left;
}
void canvas_set_left(unsigned short left) {
    context.left = left;
}

std::pair<unsigned short, unsigned short> canvas_pos() {
    return std::make_pair(context.top, context.left);
}

void canvas_set_pos(unsigned short top, unsigned short left) {
    context.top = top;
    context.left = left;
}

unsigned short canvas_width() {
    return context.width;
}

void canvas_set_width(unsigned short width) {
    context.width = width;
}

unsigned short canvas_height() {
    return context.height;
}

void canvas_set_height(unsigned short height) {
    context.height = height;
}

std::pair<unsigned short, unsigned short> canvas_size() {
    return std::make_pair(context.width, context.height);
}

void canvas_set_size(unsigned short width, unsigned short height) {
    context.width = width;
    context.height = height;
}

void canvas_set_title(const std::string& title) {
    str_to_wstr(title, context.title);
}

void set_line_width(unsigned char width) {
    wstringstream cmd;    
    cmd << L"set_line_width " << (short) width << L"\n";
    context.cmds.push_back(cmd.str().c_str());    
}

void set_color(unsigned char red, unsigned char green, unsigned char blue) {    
    wstringstream cmd;    
    cmd << L"set_color " << (short) red << " " << (short) green << " " << (short) blue << L"\n";
    context.cmds.push_back(cmd.str().c_str());
}

void set_alpha(unsigned char alpha) {
    wstringstream cmd;
    cmd << L"set_alpha " << (short) alpha << L"\n";
    context.cmds.push_back(cmd.str().c_str());
}

int text(int x, int y, const std::string& text) {
    wstring wtext;
    str_to_wstr(text, wtext);
    wstringstream cmd;    
    cmd << L"text " << x << " " << y << " " << wtext.c_str() << L"\n";
    context.cmds.push_back(cmd.str().c_str());
    return 0;
}

int line(int x1, int y1, int x2, int y2) {
    wstringstream cmd;    
    cmd << L"line " << x1 << " " << y1 << " " << x2 << " " << y2 << L"\n";
    context.cmds.push_back(cmd.str().c_str());
    return 0;
}

int stroke_rect(int x, int y, int width, int height) {
    wstringstream cmd;    
    cmd << L"stroke_rect " << x << " " << y << " " << width << " " << height << L"\n";
    context.cmds.push_back(cmd.str().c_str());
    return 0;
}

int fill_rect(int x, int y, int width, int height) {
    wstringstream cmd;    
    cmd << L"fill_rect " << x << " " << y << " " << width << " " << height << L"\n";
    context.cmds.push_back(cmd.str().c_str());
    return 0;
}

int begin_path() {
    path_buffer << "path";
    return 0;
}

int line_to(int x, int y) {
    path_buffer << " L " << x << " " << y;
    return 0;
}

int move_to(int x, int y) {
    path_buffer << " M " << x << " " << y;
    return 0;
}

int arc(int center_x, int center_y, int radius, float start_angle, float end_angle) {
    path_buffer << " A " << center_x << " " << center_y << " "  << radius << " " << start_angle << " " << end_angle;
    return 0;
}

int ellipse(int x, int y, int radius_x, int radius_y, float start_angle, float end_angle) {
    path_buffer << " E " << x << " " << y << " "  << radius_x << " " << radius_y << " " << start_angle << " " << end_angle;
    return 0;
}

int close_path() {        
    path_buffer << " C";
    return 0;
}

int fill() {
    if (!path_buffer.str().empty()) {
        context.cmds.push_back(path_buffer.str());
        path_buffer.str(wstring());
    }
    context.cmds.push_back(L"fill");
    return 0;
}

int stroke() {
    if (!path_buffer.str().empty()) {
        context.cmds.push_back(path_buffer.str());
        path_buffer.str(wstring());
    }
    context.cmds.push_back(L"stroke\n");
    return 0;
}

int stroke_arc(int center_x, int center_y, int radius, float start_angle, float end_angle) {
    wstringstream cmd;    
    cmd << L"stroke_arc " << center_x << " " << center_y << " " << radius << " " << start_angle << " " << end_angle << L"\n";
    context.cmds.push_back(cmd.str().c_str());
    return 0;
}

int fill_arc(int center_x, int center_y, int radius, float start_angle, float end_angle) {
    wstringstream cmd;    
    cmd << L"fill_arc " << center_x << " " << center_y << " " << radius << " " << start_angle << " " << end_angle << L"\n";
    context.cmds.push_back(cmd.str().c_str());
    return 0;
}