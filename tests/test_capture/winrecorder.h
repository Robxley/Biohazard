#pragma once

#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include <thread>
#include <Windows.h>

class WinRecorder
{
   
public:

    WinRecorder() = default;
    ~WinRecorder() {
        release();
    }

    std::string m_window_title;

    operator bool() {
        return m_hwnd != nullptr;
    }

    bool findWindow(const std::string& window_title = {});
    cv::Mat screenshotWithTarget(const std::string& window_title = {});

    std::string GetWindowName() const {
        return GetWindowName(this->m_hwnd);
    }


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
 

public:
    static void GetWindowName(std::string & name, HWND hwnd);
    static std::string GetWindowName(HWND hwnd) {
        std::string name; GetWindowName(name, hwnd);
        return name;
    }
};


class WinRecorderAsyncLoop
{
    std::atomic_bool m_recording = false;
    std::thread m_recorder_thread;
    cv::Mat m_last_screenshoot;
    mutable std::mutex m_mutex;

public:
    std::string_view m_filename;  //location where to save the video
    int m_tick_frequency_ms = 1000 / 30;  // fps = 30
    
    std::string m_last_screenshoot_time;
    std::size_t m_tick_count = 0;

    std::string m_window_title;

    WinRecorder m_winRecoreder;

    auto get_last_screenshoot() const {
        auto lock = std::lock_guard(m_mutex);
        return m_last_screenshoot;
    }


    std::function<void(cv::Mat&)> m_preprocessing = [](cv::Mat& in) {
        float scale = 0.5;
        int rows = static_cast<int>(in.rows * scale);
        int cols = static_cast<int>(in.cols * scale);
        if ((rows == 0) || (cols == 0)) return;
        cv::resize(in, in, { cols, rows }, 0, 0, cv::INTER_AREA);
    };

    void stop() {
        m_recording = false;
        if (m_recorder_thread.joinable())
            m_recorder_thread.join();
    }

    static auto current_date_time() {
        std::time_t result = std::time(nullptr);
        return std::asctime(std::localtime(&result));
    }

    void start() {
        stop();
      
        using clock = std::chrono::high_resolution_clock; //std::chrono::system_clock
        using duration_ms = std::chrono::duration<double, std::milli>;

        m_tick_count = 0;
        m_recording = true;
        m_recorder_thread = std::thread([&, filename = std::string(m_filename), tick_duration = duration_ms(m_tick_frequency_ms)]
        {
            std::size_t count = 0;
            while (m_recording) {
                auto start = clock::now();
                auto sn = m_winRecoreder.screenshotWithTarget(m_window_title);
                if (m_preprocessing) {
                    m_preprocessing(sn);
                }
                {
                    auto lg = std::lock_guard(m_mutex);
                    cv::swap(sn, m_last_screenshoot);
                }
                duration_ms elapsed = clock::now() - start;
                if (tick_duration > elapsed)
                    std::this_thread::sleep_for(tick_duration - elapsed);
                count++;
            };

            m_last_screenshoot_time = current_date_time();
        });
    }

    ~WinRecorderAsyncLoop() { this->stop(); }
};