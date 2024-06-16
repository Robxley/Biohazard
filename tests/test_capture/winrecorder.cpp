#include <opencv2/opencv.hpp>
#include <Windows.h>
#include <tuple>
#include "winrecorder.h"

namespace
{
    struct WinDesc
    {
        HWND m_hwnd = nullptr;
        std::string m_title = {};
        bool m_isVisible = false;
    };


    static auto enumWindows()
    {
        //hwnd, title, is visible
        thread_local static std::vector<WinDesc> windows;

        //std::function<WNDENUMPROC> 
        auto enumWindowCallback = [](HWND hWnd, LPARAM lparam) -> BOOL {
            auto length = GetWindowTextLength(hWnd);
            if (length == 0)
                return TRUE;

            const char cnull = '\0';
            std::string windowTitle(static_cast<std::size_t>(length) + 1, cnull);
            GetWindowText(hWnd, windowTitle.data(), length + 1);
      
            bool is_visible = IsWindowVisible(hWnd);
            windows.emplace_back(WinDesc{ hWnd , std::move(windowTitle), is_visible });

            return TRUE;
        };

        EnumWindows(enumWindowCallback, NULL);

        return std::move(windows);
    }
}

bool WinRecorder::findWindow(const std::string& window_title)
{

    try
    {
        if (!window_title.empty())
            m_window_title = window_title;

        m_hwnd = FindWindow(NULL, m_window_title.c_str());

        if (m_hwnd == nullptr)
        {
            auto windowList = enumWindows();
            for (auto& one : windowList) {
                if (one.m_isVisible)
                    std::cout << one.m_hwnd << ":  " << one.m_title << std::endl;
            }

            for (auto &win : windowList)
            {
                if (win.m_title.find(m_window_title) != std::string::npos)
                {
                    m_window_title = win.m_title;
                    m_hwnd = win.m_hwnd;
                    break;
                }
            }
        }
    }
    catch (...)
    {
        m_hwnd = nullptr;
    }


    return m_hwnd != nullptr;
}

namespace
{
    std::ostream& operator<<(std::ostream& os, const RECT& rect) {
        os << "Left: " << rect.left << ", Top: " << rect.top << ", Right: " << rect.right << ", Bottom: " << rect.bottom;
        return os;
    }
}


float GetMonitorScalingRatio(HMONITOR monitor)
{
    MONITORINFOEX info = { sizeof(MONITORINFOEX) };
    GetMonitorInfo(monitor, &info);
    DEVMODE devmode = {};
    devmode.dmSize = sizeof(DEVMODE);
    EnumDisplaySettings(info.szDevice, ENUM_CURRENT_SETTINGS, &devmode);
    return (info.rcMonitor.right - info.rcMonitor.left) / static_cast<float>(devmode.dmPelsWidth);
}

float GetRealDpiForMonitor(HMONITOR monitor)
{
    return GetDpiForSystem() / 96.0f / GetMonitorScalingRatio(monitor);
}

void ScreenshotWithTarget(HWND hwnd_target, cv::Mat& screenshot)
{
    try
    {
        auto target_dpi_awarenes_context = GetWindowDpiAwarenessContext(hwnd_target);
        auto previous_dpi_awareness_context = SetThreadDpiAwarenessContext(target_dpi_awarenes_context);

        auto monitor = MonitorFromWindow(hwnd_target, MONITOR_DEFAULTTONULL);
        //auto scale = 1.0/GetMonitorScalingRatio(monitor);
        auto scale = GetRealDpiForMonitor(monitor);
    
        std::cout 
            << " scale: " << scale 
            << " dpi_s: " << GetDpiForSystem()
            << " dpi_w: " << GetDpiForWindow(hwnd_target)
            << " dpi_aware: " << GetWindowDpiAwarenessContext(hwnd_target)
            << "\r";
        
        auto dpi_corr = [&](const auto& v) {return scale == 1.0 ? v : static_cast<int>(ceil(v * scale)); };

        //get hdc of desktop
        HDC hdc = GetDC(HWND_DESKTOP);
        HDC memdc = CreateCompatibleDC(hdc);

        RECT crc;
        if (GetClientRect(hwnd_target, &crc) == 0)
            std::cout << "GetClientRect" << std::endl;


        int w = dpi_corr(crc.right - crc.left);
        int h = dpi_corr(crc.bottom - crc.top);

        HBITMAP hbitmap = CreateCompatibleBitmap(hdc, w, h);
        HGDIOBJ oldbmp = SelectObject(memdc, hbitmap);

        POINT ptClientUL;              // client upper left corner 
        POINT ptClientLR;              // client lower right corner

        ptClientUL.x = dpi_corr(crc.left);
        ptClientUL.y = dpi_corr(crc.top);

        ptClientLR.x = dpi_corr(crc.right + 1);
        ptClientLR.y = dpi_corr(crc.bottom + 1);

        ClientToScreen(hwnd_target, &ptClientUL); // convert upper-left
        ClientToScreen(hwnd_target, &ptClientLR); // convert bottom-right

        LogicalToPhysicalPointForPerMonitorDPI(hwnd_target, &ptClientUL);
        LogicalToPhysicalPointForPerMonitorDPI(hwnd_target, &ptClientLR);

        BitBlt(memdc, 0, 0, w, h, hdc, ptClientUL.x, ptClientUL.y, SRCCOPY);
        SelectObject(memdc, oldbmp);
        DeleteDC(memdc);

        BITMAPINFOHEADER bi = { sizeof(bi), w, -h, 1, 32 };
        screenshot.create(h, w, CV_8UC4);
        GetDIBits(hdc, hbitmap, 0, h, screenshot.data,
            (BITMAPINFO*)&bi, DIB_RGB_COLORS);

        DeleteObject(hbitmap);
        ReleaseDC(HWND_DESKTOP, hdc);

        if (previous_dpi_awareness_context)
            SetThreadDpiAwarenessContext(previous_dpi_awareness_context);

    }
    catch (...)
    {
        std::cout << "Exception in capture !" << std::endl;
    }
}

void WinRecorder::GetWindowName(std::string& name, HWND hwnd = nullptr)
{
    if (hwnd == nullptr)
        hwnd = GetForegroundWindow();

    if (hwnd == nullptr)
        return;
    auto len = GetWindowTextLength(hwnd);
    name.resize(len + 1, '\0');
    GetWindowText(hwnd, name.data(), static_cast<int>(name.size()));
}


cv::Mat WinRecorder::screenshotWithTarget(const std::string& window_title)
{
    if (window_title.empty())
    {
        m_hwnd = GetForegroundWindow();
    }
    else if (!window_title.empty())
    {
        findWindow(window_title);
    }
    if (!m_hwnd)
    {
        return cv::Mat();
    }

    ScreenshotWithTarget(m_hwnd, m_buffer);

    return m_buffer;
}
