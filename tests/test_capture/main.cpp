
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

    auto display = [](cv::Mat& mat, const char* title, float scale = 1.0f)
    {
        try
        {
            if (!mat.empty())
            {
                cv::Mat smat;
                if (scale != 1.0f)
                    cv::resize(mat, smat, {}, 0.5, 0.5, cv::INTER_AREA);
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

    do
    {

        {
            auto mat = test.captureWindow();
            display(mat, "captureWindow", 0.5f);
        }

        {
        //    auto mat = test.screenshoot();
        //    display(mat, "screenshoot");
        }

        {
            auto mat = test.screenshootWithTarget();
            display(mat, "screenshootWithTarget", 0.5f);
        }


    } while (cv::waitKey(30));



    return 0;

}