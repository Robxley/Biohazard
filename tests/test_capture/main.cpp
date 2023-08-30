
#include "BHG_GuiHazard.h"
#include <iostream>
#include "winrecorder.h"

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
    //test.m_window_title = "Notepad";

    /*test.findWindow();
    if (!test) 
    {
        std::cout << "Windows not found" << std::endl;
        return 0;
    }

    std::cout << "Windows recording: " << test.m_window_title << std::endl;
    */

    auto display = [](cv::Mat& mat, const char* title)
    {
        try
        {
            if (!mat.empty())
            {
                cv::Mat smat = mat;
               // cv::resize(mat, smat, {}, 0.5, 0.5, cv::INTER_AREA);
                if (!smat.empty())
                    cv::imshow(title, smat);
            }
        }
        catch (...)
        {

        }
    };

    do
    {

        {
            auto mat = test.captureWindow();
            display(mat, "captureWindow");
        }

        {
        //    auto mat = test.screenshoot();
        //    display(mat, "screenshoot");
        }

        {
            auto mat = test.screenshootWithTarget();
            display(mat, "screenshootWithTarget");
        }


    } while (cv::waitKey(30));



    return 0;

}