
#include "BHG_GuiHazard.h"
#include "winrecorder.h"
#include "inputrecorder.h"

namespace bhd
{
    class CaptureViewer : public GuiHazard
    {

    public:
        ImGui::FixedWindow m_mainWindow;	//main window
        ImGui::MatWatch m_sceenViewer;      //wath the screen record

        WinRecorderAsyncLoop m_winRecorder;
        KeyLogger m_keylogger = { "keylogger.txt" };


        bool m_show_imgui_demo = 0;

        bool m_is_recording = false;

        void Init()
        {
            GuiHazard::Init();
            m_mainWindow.m_title = ICON_FA_BUG " Modulazard";
            m_mainWindow.m_rect = ImGui::FixedWindow::RECT_FULLSCREEN;

            start_recording();
        }

        void DrawGui() override;


        ~CaptureViewer() {
            this->stop_recording();
        }

    private:

        //main panels
        void MainMenuBar();
        void LeftPanel_Settings();
        void RightPanel_TabBars();

        //subpanel
        void RightPanel_TabBar_Processing();


        void start_recording() {
            m_is_recording = true;
            m_winRecorder.start();
            //m_keylogger.start();
        }

        void stop_recording() {
            m_winRecorder.stop();
            m_keylogger.stop();
            m_is_recording = false;
        }

    };

}