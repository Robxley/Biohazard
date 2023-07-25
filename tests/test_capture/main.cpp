
#include "BHG_GuiHazard.h"
#include <iostream>
#include <opencv2/opencv.hpp>
#include <Windows.h>


int my_gui(auto&& path)
{
    //std::filesystem::path path = argc >= 2 ? argv[1] : std::filesystem::path{};

    bhd::GuiModHazard gui;
    gui.Init(path);
    gui.Run();
}


class ProgramRecorder
{
public:
    ProgramRecorder() = default;
    std::string m_window_title;

    operator bool() {
        return m_hwnd != nullptr;
    }

    auto findWindow(const std::string& window_title = {})
    {
        try
        {
            if (!window_title.empty())
                m_window_title = window_title;
            m_hwnd = FindWindow(NULL, m_window_title.c_str());
        }
        catch (...)
        {
            m_hwnd = nullptr;
        }

        return m_hwnd;
    }

    cv::Mat record(const std::string& window_title = {})
    {
        if (m_hwnd)
            findWindow(window_title);

        if (!m_hwnd)
        {
            return cv::Mat();
        }

        // Get the dimensions of the window
        RECT rect;
        GetWindowRect(m_hwnd, &rect);
		cv::Rect current_rect = win2cvRect(rect);
		if (current_rect != m_window_rect)
		{
			m_window_rect = current_rect;
            release();

            m_deviceContext = GetDC(m_hwnd);
            m_memoryDeviceContext = CreateCompatibleDC(m_deviceContext);
            m_bitmap = CreateCompatibleBitmap(m_deviceContext, current_rect.width, current_rect.height);
            SelectObject(m_memoryDeviceContext, m_bitmap);

            m_buffer.create(current_rect.height, current_rect.width, CV_8UC4); // 8 bit unsigned ints 4 Channels -> RGBA
		}

        BitBlt(m_memoryDeviceContext, 0, 0, current_rect.width, current_rect.height, m_deviceContext, 0, 0, SRCCOPY);


        //specify format by using bitmapinfoheader!
        BITMAPINFOHEADER bi;
        bi.biSize = sizeof(BITMAPINFOHEADER);
        bi.biWidth = current_rect.width;
        bi.biHeight = -current_rect.height;
        bi.biPlanes = 1;
        bi.biBitCount = 32;
        bi.biCompression = BI_RGB;
        bi.biSizeImage = 0; //because no compression
        bi.biXPelsPerMeter = 1; //we
        bi.biYPelsPerMeter = 2; //we
        bi.biClrUsed = 3; //we ^^
        bi.biClrImportant = 4; //still we

        //transform data and store into mat.data
        GetDIBits(m_memoryDeviceContext, m_bitmap, 0, current_rect.height, m_buffer.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);

        return m_buffer;
    }

	cv::Mat getMat(HWND hWND) {


		RECT windowRect;
		GetClientRect(hWND, &windowRect);

		int height = windowRect.bottom;
		int width = windowRect.right;



		HDC deviceContext = GetDC(hWND);
		HDC memoryDeviceContext = CreateCompatibleDC(deviceContext);

		HBITMAP bitmap = CreateCompatibleBitmap(deviceContext, width, height);

		SelectObject(memoryDeviceContext, bitmap);

		//copy data into bitmap
		BitBlt(memoryDeviceContext, 0, 0, width, height, deviceContext, 0, 0, SRCCOPY);


		//specify format by using bitmapinfoheader!
		BITMAPINFOHEADER bi;
		bi.biSize = sizeof(BITMAPINFOHEADER);
		bi.biWidth = width;
		bi.biHeight = -height;
		bi.biPlanes = 1;
		bi.biBitCount = 32;
		bi.biCompression = BI_RGB;
		bi.biSizeImage = 0; //because no compression
		bi.biXPelsPerMeter = 1; //we
		bi.biYPelsPerMeter = 2; //we
		bi.biClrUsed = 3; //we ^^
		bi.biClrImportant = 4; //still we

		cv::Mat mat = cv::Mat(height, width, CV_8UC4); // 8 bit unsigned ints 4 Channels -> RGBA

		//transform data and store into mat.data
		GetDIBits(memoryDeviceContext, bitmap, 0, height, mat.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);

		//clean up!
		DeleteObject(bitmap);
		DeleteDC(memoryDeviceContext); //delete not release!
		ReleaseDC(hWND, deviceContext);

		return mat;
	}




    ~ProgramRecorder()
    {
        release();
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
    std::atomic_int m_status = 0;  //0 : not recording, 1: recording, -1: stop is required
};



int main(int argc, char* argv[])
{
    ProgramRecorder test;
    test.m_window_title = "VLC media player";

    test.findWindow();
    if (!test) 
    {
        std::cout << "Windows not found" << std::endl;
        return 0;
    }

    do
    {
        auto mat = test.record();
        if (mat.empty())
            break;
        cv::imshow("Test", mat);
    } while ((cv::waitKey(30) & 0xFF) != 'q');

    return 0;

}