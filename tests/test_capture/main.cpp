
#include "BHG_GuiHazard.h"
#include <iostream>
#include "winrecorder.h"
#include "inputrecorder.h"

int my_gui(auto&& path)
{
    //std::filesystem::path path = argc >= 2 ? argv[1] : std::filesystem::path{};

    bhd::GuiModHazard gui;
    gui.Init(path);
    gui.Run();
}





int main(int argc, char* argv[])
{
    WinRecorder test;

    KeyLogger keylogger = {"keylogger.txt"};

    keylogger.start();

    auto display = [](cv::Mat& mat, const char* title, float scale = 1.0f)
    {
        try
        {
            if (!mat.empty())
            {
                cv::Mat smat;
                if (scale != 1.0f && !mat.empty())
                    cv::resize(mat, smat, {0,0}, scale, scale, cv::INTER_AREA);
                else
                    smat = mat;
                if (!smat.empty())
                    cv::imshow(title, smat);
            }
        }
        catch (...)
        {

        }
    };

    int dpi_mode = DPI_MODE::AWARENESS_CONTEXT;
    while (true)
    {

        {
            auto mat = test.screenshotWithTarget({}, dpi_mode);
            display(mat, "screenshotWithTarget", 0.5f);
        }

        auto key = cv::waitKey(30);
        if (key == 'q')
            break;
        else if (key == 'w')
        {
            dpi_mode++;
            if (dpi_mode >= DPI_MODE::DPI_MODE_COUNT)
                dpi_mode = DPI_MODE::DPI_IGNORE;
            std::cout << "DPI MODE: " << dpi_mode << std::endl;
        }
    }
    keylogger.stop();


    return 0;

}