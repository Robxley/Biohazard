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


void Screenshot(cv::Mat & mat, HWND m_hwnd = HWND_DESKTOP)
{
    auto w = GetSystemMetrics(SM_CXFULLSCREEN);
    auto h = GetSystemMetrics(SM_CYFULLSCREEN);
    auto hdc = GetDC(m_hwnd);
    auto hbitmap = CreateCompatibleBitmap(hdc, w, h);
    auto memdc = CreateCompatibleDC(hdc);
    auto oldbmp = SelectObject(memdc, hbitmap);
    BitBlt(memdc, 0, 0, w, h, hdc, 0, 0, SRCCOPY);

    mat.create(h, w, CV_8UC4);
    BITMAPINFOHEADER bi = { sizeof(bi), w, -h, 1, 32, BI_RGB };
    GetDIBits(hdc, hbitmap, 0, h, mat.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);

    SelectObject(memdc, oldbmp);
    DeleteDC(memdc);
    DeleteObject(hbitmap);
    ReleaseDC(m_hwnd, hdc);
}

namespace
{
    std::ostream& operator<<(std::ostream& os, const RECT& rect) {
        os << "Left: " << rect.left << ", Top: " << rect.top << ", Right: " << rect.right << ", Bottom: " << rect.bottom;
        return os;
    }
}

void CaptureWindow(HWND targetWindow, cv::Mat& screenshot) {

    if (IsWindowVisible(targetWindow) == FALSE)
        return;


    auto target_dpi_awarenes_context = GetWindowDpiAwarenessContext(targetWindow);
    auto previous_dpi_awareness_context = SetThreadDpiAwarenessContext(target_dpi_awarenes_context);

    RECT windowRect;
    GetClientRect(targetWindow, &windowRect);
    int width = windowRect.right - windowRect.left;
    int height = windowRect.bottom - windowRect.top;

    if (width <= 0 || height <= 0)
        return;

    HDC hdcWindow = GetDC(targetWindow);
    if (hdcWindow)
    {
        HDC hdcScreen = GetDC(HWND_DESKTOP);
        HDC hdcMem = CreateCompatibleDC(hdcScreen);

        HBITMAP hBitmap = CreateCompatibleBitmap(hdcScreen, width, height);
        HGDIOBJ hOldBitmap = SelectObject(hdcMem, hBitmap);

        BitBlt(hdcMem, 0, 0, width, height, hdcWindow, 0, 0, SRCCOPY);

        screenshot.create(height, width, CV_8UC4);
        GetBitmapBits(hBitmap, width * height * 4, screenshot.data);

        cv::cvtColor(screenshot, screenshot, cv::COLOR_BGRA2BGR);

        SelectObject(hdcMem, hOldBitmap);
        DeleteObject(hBitmap);
        DeleteDC(hdcMem);
        ReleaseDC(targetWindow, hdcWindow);
        ReleaseDC(NULL, hdcScreen);
    }
    else
    {
        screenshot.create(512, 125, CV_8UC4);
    }

    if (previous_dpi_awareness_context)
        SetThreadDpiAwarenessContext(previous_dpi_awareness_context);
}



void ScreenshotWithTarget(HWND hwnd_target, cv::Mat& screenshot, int dpi = DPI_MODE::DPI_FOR_SYSTEM)
{
    try
    {
        const auto DPI_DEFAULT = 96.0f;

        if (dpi == DPI_MODE::DPI_FOR_SYSTEM)
            dpi = GetDpiForSystem();
        else if (dpi == DPI_MODE::DPI_FOR_WINDOW)
            dpi = GetDpiForWindow(hwnd_target);
        
   
        float dpi_factor = dpi / DPI_DEFAULT;
        auto dpi_corr = [&](const auto& v) {return dpi <= DPI_MODE::DPI_IGNORE ? v : static_cast<int>(ceil(v * dpi_factor)); };

        //get hdc of desktop
        HDC hdc = GetDC(HWND_DESKTOP);
        HDC memdc = CreateCompatibleDC(hdc);

        RECT crc;
        if (GetClientRect(hwnd_target, &crc) == 0)
            std::cout << "GetClientRect" << std::endl;


        int w = (crc.right - crc.left);
        int h = (crc.bottom - crc.top);

        HBITMAP hbitmap = CreateCompatibleBitmap(hdc, w, h);
        HGDIOBJ oldbmp = SelectObject(memdc, hbitmap);

        POINT ptClientUL;              // client upper left corner 
        POINT ptClientLR;              // client lower right corner

        ptClientUL.x = (crc.left);
        ptClientUL.y = (crc.top);

        ptClientLR.x = (crc.right + 1);
        ptClientLR.y = (crc.bottom + 1);

        ClientToScreen(hwnd_target, &ptClientUL); // convert upper-left
        ClientToScreen(hwnd_target, &ptClientLR); // convert bottom-right


        SetRect(&crc, ptClientUL.x, ptClientUL.y,
            ptClientLR.x, ptClientLR.y);

        BitBlt(memdc, 0, 0, w, h, hdc, crc.left, crc.top, SRCCOPY);
        SelectObject(memdc, oldbmp);
        DeleteDC(memdc);

        BITMAPINFOHEADER bi = { sizeof(bi), w, -h, 1, 32 };
        screenshot.create(h, w, CV_8UC4);
        GetDIBits(hdc, hbitmap, 0, h, screenshot.data,
            (BITMAPINFO*)&bi, DIB_RGB_COLORS);

        DeleteObject(hbitmap);
        ReleaseDC(HWND_DESKTOP, hdc);

    }
    catch (...)
    {
        std::cout << "Exception in capture !" << std::endl;
    }
}

void ScreenshotWithTargetDpiAwareness(HWND hwnd_target, cv::Mat& screenshot)
{
    try
    {

        auto target_dpi_awarenes_context = GetWindowDpiAwarenessContext(hwnd_target);
        auto previous_dpi_awareness_context = SetThreadDpiAwarenessContext(target_dpi_awarenes_context);

        ScreenshotWithTarget(hwnd_target, screenshot, DPI_MODE::DPI_IGNORE);

        if (previous_dpi_awareness_context)
            SetThreadDpiAwarenessContext(previous_dpi_awareness_context);
    }
    catch (...)
    {
        std::cout << "Exception in capture !" << std::endl;
    }
}

cv::Mat WinRecorder::captureWindow(const std::string& window_title)
{
    if (m_window_title.empty())
    {
        m_hwnd = GetForegroundWindow();
    }
    else if (!m_hwnd)
    {
        findWindow(window_title);
    }
    if (!m_hwnd)
    {
        return cv::Mat();
    }
    CaptureWindow(m_hwnd, m_buffer);

    return m_buffer;
}

cv::Mat WinRecorder::screenshot()
{
    Screenshot(m_buffer);
    return m_buffer;
}


cv::Mat WinRecorder::screenshotWithTarget(const std::string& window_title, int dpi)
{
    if (m_window_title.empty())
    {
        m_hwnd = GetForegroundWindow();
    }
    else if (!m_hwnd)
    {
        findWindow(window_title);
    }
    if (!m_hwnd)
    {
        return cv::Mat();
    }

    if (dpi == DPI_MODE::AWARENESS_CONTEXT)
        ScreenshotWithTargetDpiAwareness(m_hwnd, m_buffer);
    else
        ScreenshotWithTarget(m_hwnd, m_buffer, dpi);


    return m_buffer;
}
