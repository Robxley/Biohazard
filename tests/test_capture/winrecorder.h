#pragma once

#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include <Windows.h>

namespace
{
    enum DPI_MODE
    {
        DPI_IGNORE,
        DPI_FOR_WINDOW,
        DPI_FOR_SYSTEM,
        AWARENESS_CONTEXT,
        DPI_MODE_COUNT
    };
    
}

class WinRecorder
{
   
public:
    WinRecorder() = default;
    ~WinRecorder() {
        release();
    }

    std::string m_window_title;

    int m_mode = DPI_MODE::AWARENESS_CONTEXT;

    operator bool() {
        return m_hwnd != nullptr;
    }

    bool findWindow(const std::string& window_title = {});
    cv::Mat captureWindow(const std::string& window_title = {});
    cv::Mat screenshot();
    cv::Mat screenshotWithTarget(const std::string& window_title = {}, int dpi = -1);

private:

    void release()
    {
        //clean up
        if (m_bitmap)                DeleteObject(m_bitmap);
        if (m_memoryDeviceContext)   DeleteDC(m_memoryDeviceContext); //delete not release!
        if (m_deviceContext)         ReleaseDC(m_hwnd, m_deviceContext);
    }


    cv::Rect win2cvRect(RECT& rect) {
        return cv::Rect(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
    }

    HWND m_hwnd = nullptr;;
    HDC m_deviceContext = nullptr;
    HDC m_memoryDeviceContext = nullptr;
    HBITMAP m_bitmap = nullptr;
    cv::Rect m_window_rect = {};
    cv::Mat m_buffer = {};
    std::atomic_int m_status = 0;  //0 : not recording, 1: recording, -1: stop is required
};
