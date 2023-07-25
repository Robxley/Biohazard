#pragma once

#include <array>
#include <cstring>
#include <filesystem>
#include <functional>
#include <memory>
#include <string>
#include "imgui.h"

#ifndef IMGUI_VERSION
#   error "include imgui.h before this header"
#endif

#ifndef IMGUI_IMPL_ICONS_FONT_AWESOME
#if defined(__has_include)
#if __has_include(<IconsFontAwesome5.h>)
#include <IconsFontAwesome5.h>
#define IMGUI_IMPL_ICONS_FONT_AWESOME
#endif
#endif
#endif



using ImGuiFileBrowserFlags = int;

enum ImGuiFileBrowserFlags_
{
    ImGuiFileBrowserFlags_SelectDirectory    = 1 << 0, // select directory instead of regular file
    ImGuiFileBrowserFlags_EnterNewFilename   = 1 << 1, // allow user to enter new filename when selecting regular file
    ImGuiFileBrowserFlags_NoModal            = 1 << 2, // file browsing window is modal by default. specify this to use a popup window
    ImGuiFileBrowserFlags_NoTitleBar         = 1 << 3, // hide window title bar
    ImGuiFileBrowserFlags_NoStatusBar        = 1 << 4, // hide status bar at the bottom of browsing window
    ImGuiFileBrowserFlags_CloseOnEsc         = 1 << 5, // close file browser when pressing 'ESC'
    ImGuiFileBrowserFlags_CreateNewDir       = 1 << 6, // allow user to create new directory
};

namespace ImGui
{
    class FileBrowser
    {
        using CallbackFunc = std::function<void(const std::filesystem::path&)>;

    public:

        // pwd is set to current working directory by default
        explicit FileBrowser(ImGuiFileBrowserFlags flags = 0);

        FileBrowser(const FileBrowser &copyFrom);

        FileBrowser &operator=(const FileBrowser &copyFrom);

        // set the window size (in pixels)
        // default is (700, 450)
        void SetWindowSize(int width, int height) noexcept;

        // set the window title text
        void SetTitle(std::string title);

        // open the browsing window
        void Open();

        // open the browsing window with callback function on closing. HasSelected is directly "consumed" by the callback. (mean HasSelected == false)
        void Open(CallbackFunc callbackFunc);

        // open the browsing window using a path name
        void Open(const std::filesystem::path& path_name);

        // open the browsing window using a path name with a callback function on closing. HasSelected is directly "consumed" by the callback. (mean HasSelected == false)
        void Open(const std::filesystem::path& path_name, CallbackFunc callback);

        // close the browsing window
        void Close();

        // the browsing window is opened or not
        bool IsOpened() const noexcept;

        // display the browsing window if opened
        void Display();

        // returns true when there is a selected filename and the "ok" button was clicked
        bool HasSelected() const noexcept;

        // set current browsing directory
        bool SetPwd(const std::filesystem::path &pwd = std::filesystem::current_path());

        // returns selected filename. make sense only when HasSelected returns true
        std::filesystem::path GetSelected() const;

        // set selected filename to empty
        void ClearSelected();

        // set file type filters. eg. { ".h", ".cpp", ".hpp", ".cc", ".inl" }
        void SetTypeFilters(const std::vector<const char*> &typeFilters);

    private:

        class ScopeGuard
        {
            std::function<void()> func_;

        public:

            template<typename T>
            explicit ScopeGuard(T func) : func_(std::move(func)) { }
            ~ScopeGuard() { func_(); }
        };

        void SetPwdUncatched(const std::filesystem::path &pwd);

        void InvokeCallBack();

#ifdef _WIN32
        static std::uint32_t GetDrivesBitMask();
#endif

        template<class ...Args>
        void HelperTooltip(Args... args);

        int width_;
        int height_;
        ImGuiFileBrowserFlags flags_;

        std::string title_;
        std::string openLabel_;

        bool openFlag_;
        bool closeFlag_;
        bool isOpened_;
        bool ok_;

        std::string statusStr_;

        std::vector<const char*> typeFilters_;
        int typeFilterIndex_;

        std::filesystem::path pwd_;
        std::string selectedFilename_;
        CallbackFunc callbackFunc_;
        std::exception_ptr callBackException_;

        struct FileRecord
        {
            bool isDir = false;
            std::string name;
            std::string showName;
            std::string extension;
        };
        std::vector<FileRecord> fileRecords_;

        // IMPROVE: truncate when selectedFilename_.length() > inputNameBuf_.size() - 1
        static constexpr size_t INPUT_NAME_BUF_SIZE = 512;
        std::unique_ptr<std::array<char, INPUT_NAME_BUF_SIZE>> inputNameBuf_;

        std::string openNewDirLabel_;
        std::unique_ptr<std::array<char, INPUT_NAME_BUF_SIZE>> newDirNameBuf_;

#ifdef _WIN32
        uint32_t drives_;
#endif

    };
} // namespace ImGui

inline ImGui::FileBrowser::FileBrowser(ImGuiFileBrowserFlags flags)
    : width_(700), height_(450), flags_(flags),
      openFlag_(false), closeFlag_(false), isOpened_(false), ok_(false),
      inputNameBuf_(std::make_unique<std::array<char, INPUT_NAME_BUF_SIZE>>())
{
    if(flags_ & ImGuiFileBrowserFlags_CreateNewDir)
        newDirNameBuf_ = std::make_unique<std::array<char, INPUT_NAME_BUF_SIZE>>();

    inputNameBuf_->at(0) = '\0';
    SetTitle("file browser");
    SetPwd(std::filesystem::current_path());

    typeFilters_.clear();
    typeFilterIndex_ = 0;

#ifdef _WIN32
    drives_ = GetDrivesBitMask();
#endif
}

inline ImGui::FileBrowser::FileBrowser(const FileBrowser &copyFrom)
    : FileBrowser()
{
    *this = copyFrom;
}

inline ImGui::FileBrowser &ImGui::FileBrowser::operator=(const FileBrowser &copyFrom)
{
    flags_ = copyFrom.flags_;
    SetTitle(copyFrom.title_);

    openFlag_  = copyFrom.openFlag_;
    closeFlag_ = copyFrom.closeFlag_;
    isOpened_  = copyFrom.isOpened_;
    ok_        = copyFrom.ok_;
    
    statusStr_ = "";
    pwd_ = copyFrom.pwd_;
    selectedFilename_ = copyFrom.selectedFilename_;

    fileRecords_ = copyFrom.fileRecords_;

    *inputNameBuf_ = *copyFrom.inputNameBuf_;

    if(flags_ & ImGuiFileBrowserFlags_CreateNewDir)
    {
        newDirNameBuf_ = std::make_unique<std::array<char, INPUT_NAME_BUF_SIZE>>();
        *newDirNameBuf_ = *copyFrom.newDirNameBuf_;
    }

    return *this;
}

inline void ImGui::FileBrowser::SetWindowSize(int width, int height) noexcept
{
    assert(width > 0 && height > 0);
    width_  = width;
    height_ = height;
}

inline void ImGui::FileBrowser::SetTitle(std::string title)
{
    title_ = std::move(title);
#ifndef IMGUI_IMPL_ICONS_FONT_AWESOME
    openLabel_ = title_ + "##filebrowser_" + std::to_string(reinterpret_cast<size_t>(this));
    openNewDirLabel_ = "new dir##new_dir_" + std::to_string(reinterpret_cast<size_t>(this));
#else
    openLabel_ = ICON_FA_FOLDER " " + title_ + "##filebrowser_" + std::to_string(reinterpret_cast<size_t>(this));
    openNewDirLabel_ = ICON_FA_FOLDER_PLUS " new dir##new_dir_" + std::to_string(reinterpret_cast<size_t>(this));
#endif
}

inline void ImGui::FileBrowser::Open()
{
    ClearSelected();
    statusStr_ = std::string();
    openFlag_ = true;
    closeFlag_ = false;
}
inline void ImGui::FileBrowser::Open(CallbackFunc callbackFunc) {
    this->callbackFunc_ = std::move(callbackFunc);
    Open();
}


inline void ImGui::FileBrowser::Open(const std::filesystem::path& path_name)
{
    if (!path_name.empty())
        SetPwd(path_name.parent_path());

    this->Open();

    if (path_name.has_filename())
    {
        auto filename = path_name.filename().generic_string();
        auto count = std::min(filename.size(), inputNameBuf_->size()-1);
        std::copy_n(filename.begin(), count, inputNameBuf_->begin());
        (*inputNameBuf_)[count] = '\0';
        selectedFilename_ = inputNameBuf_->data();
    }
}
inline void ImGui::FileBrowser::Open(const std::filesystem::path& path_name, CallbackFunc callbackFunc)
{
    this->callbackFunc_ = std::move(callbackFunc);
    Open(path_name);
}


inline void ImGui::FileBrowser::Close()
{
    ClearSelected();
    statusStr_ = std::string();
    closeFlag_ = true;
    openFlag_ = false;
}

inline bool ImGui::FileBrowser::IsOpened() const noexcept
{
    return isOpened_;
}

template<class ...Args>
void ImGui::FileBrowser::HelperTooltip(Args... args)
{
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(std::forward<Args>(args)...);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

inline void ImGui::FileBrowser::InvokeCallBack()
{
    if (callbackFunc_)
    {
        try { callbackFunc_(GetSelected()); }
        catch (...) {
            callBackException_ = std::current_exception();
        };
        callbackFunc_ = {};
        ok_ = false;
    }
    else
    {
        ok_ = true;
    }
};


inline void ImGui::FileBrowser::Display()
{
    PushID(this);
    ScopeGuard exitThis([this] { openFlag_ = false; closeFlag_ = false; PopID(); });

    if(openFlag_)
        OpenPopup(openLabel_.c_str());
    isOpened_ = false;

    // open the popup window

    if(openFlag_ && (flags_ & ImGuiFileBrowserFlags_NoModal))
        SetNextWindowSize(ImVec2(static_cast<float>(width_), static_cast<float>(height_)));
    else
        SetNextWindowSize(ImVec2(static_cast<float>(width_), static_cast<float>(height_)), ImGuiCond_FirstUseEver);
    if(flags_ & ImGuiFileBrowserFlags_NoModal)
    {
        if(!BeginPopup(openLabel_.c_str()))
            return;
    }
    else if(!BeginPopupModal(openLabel_.c_str(), nullptr,
        flags_ & ImGuiFileBrowserFlags_NoTitleBar ? ImGuiWindowFlags_NoTitleBar : 0))
    {
        return;
    }
    isOpened_ = true;
    ScopeGuard endPopup([] { EndPopup(); });

    // display elements in pwd

#ifdef _WIN32
    char currentDrive = static_cast<char>(pwd_.c_str()[0]);
    char driveStr[] = { currentDrive, ':', '\0' };

    PushItemWidth(4 * GetFontSize());
    if(BeginCombo("##select_drive", driveStr))
    {
        ScopeGuard guard([&] { ImGui::EndCombo(); });
        for(int i = 0; i < 26; ++i)
        {
            if(!(drives_ & (1 << i)))
                continue;
            char driveCh = static_cast<char>('A' + i);
            char selectableStr[] = { driveCh, ':', '\0' };
            bool selected = currentDrive == driveCh;
            if(Selectable(selectableStr, selected) && !selected)
            {
                char newPwd[] = { driveCh, ':', '\\', '\0' };
                SetPwd(newPwd);
            }
        }
    }
    PopItemWidth();

    SameLine();
#endif

    int secIdx = 0, newPwdLastSecIdx = -1;
    for(auto &sec : pwd_)
    {
#ifdef _WIN32
        if(secIdx == 1)
        {
            ++secIdx;
            continue;
        }
#endif
        PushID(secIdx);
        if(secIdx > 0)
            SameLine();
        if(SmallButton(sec.u8string().c_str()))
            newPwdLastSecIdx = secIdx;
        PopID();
        ++secIdx;
    }

    if(newPwdLastSecIdx >= 0)
    {
        int i = 0;
        std::filesystem::path newPwd;
        for(auto &sec : pwd_)
        {
            if(i++ > newPwdLastSecIdx)
                break;
            newPwd /= sec;
        }
#ifdef _WIN32
        if(newPwdLastSecIdx == 0)
            newPwd /= "\\";
#endif
        SetPwd(newPwd);
    }

    SameLine();

    if(SmallButton("*"))
        SetPwd(pwd_);
    HelperTooltip("Current folder");

    if(newDirNameBuf_)
    {
        SameLine();
#ifndef IMGUI_IMPL_ICONS_FONT_AWESOME
        if(SmallButton("+"))
#else
        if (Button(ICON_FA_FOLDER_PLUS))
#endif
        {
            OpenPopup(openNewDirLabel_.c_str());
            (*newDirNameBuf_)[0] = '\0';
        }
        HelperTooltip("Create a new folder");

        if(BeginPopup(openNewDirLabel_.c_str()))
        {
            ScopeGuard endNewDirPopup([] { EndPopup(); });

            InputText("name", newDirNameBuf_->data(), newDirNameBuf_->size()); SameLine();
            if(Button("ok") && (*newDirNameBuf_)[0] != '\0')
            {
                ScopeGuard closeNewDirPopup([] { CloseCurrentPopup(); });
                if(create_directory(pwd_ / newDirNameBuf_->data()))
                    SetPwd(pwd_);
                else
                    statusStr_ = "failed to create " + std::string(newDirNameBuf_->data());
            }
        }
    }

    // browse files in a child window

    float reserveHeight = GetFrameHeightWithSpacing();
    std::filesystem::path newPwd; bool setNewPwd = false;
    if(!(flags_ & ImGuiFileBrowserFlags_SelectDirectory) && (flags_ & ImGuiFileBrowserFlags_EnterNewFilename))
        reserveHeight += GetFrameHeightWithSpacing();
    {
        BeginChild("ch", ImVec2(0, -reserveHeight), true,
            (flags_ & ImGuiFileBrowserFlags_NoModal) ? ImGuiWindowFlags_AlwaysHorizontalScrollbar : 0);
        ScopeGuard endChild([] { EndChild(); });

        for(auto &rsc : fileRecords_)
        {
            if (!rsc.isDir && typeFilters_.size() > 0 &&
                static_cast<size_t>(typeFilterIndex_) < typeFilters_.size() &&
                !(rsc.extension == typeFilters_[typeFilterIndex_]))
                continue;

            if(!rsc.name.empty() && rsc.name[0] == '$')
                continue;

            const bool selected = selectedFilename_ == rsc.name;
            if(Selectable(rsc.showName.c_str(), selected, ImGuiSelectableFlags_DontClosePopups))
            {
                if(selected)
                {
                    selectedFilename_ = std::string();
                    (*inputNameBuf_)[0] = '\0';
                }
                else if(rsc.name != "..")
                {
                    if((rsc.isDir && (flags_ & ImGuiFileBrowserFlags_SelectDirectory)) ||
                       (!rsc.isDir && !(flags_ & ImGuiFileBrowserFlags_SelectDirectory)))
                    {
                        selectedFilename_ = rsc.name;
                        if(!(flags_ & ImGuiFileBrowserFlags_SelectDirectory))
                        {
#ifdef _MSC_VER
                            strcpy_s(inputNameBuf_->data(), inputNameBuf_->size(), selectedFilename_.c_str());
#else
                            std::strncpy(inputNameBuf_->data(), selectedFilename_.c_str(), inputNameBuf_->size());
#endif
                        }
                    }
                }
            }

            if(IsItemClicked(0) && IsMouseDoubleClicked(0) && rsc.isDir)
            {
                setNewPwd = true;
                newPwd = (rsc.name != "..") ? (pwd_ / rsc.name) : pwd_.parent_path();
            }
        }
    }

    if(setNewPwd)
        SetPwd(newPwd);

    if(!(flags_ & ImGuiFileBrowserFlags_SelectDirectory) && (flags_ & ImGuiFileBrowserFlags_EnterNewFilename))
    {
        PushID(this);
        ScopeGuard popTextID([] { PopID(); });

        PushItemWidth(-1);
        if(InputText("", inputNameBuf_->data(), inputNameBuf_->size()))
            selectedFilename_ = inputNameBuf_->data();
        PopItemWidth();
    }

    if(!(flags_ & ImGuiFileBrowserFlags_SelectDirectory))
    {
        if(Button(" ok ") && !selectedFilename_.empty())
        {
            InvokeCallBack();
            CloseCurrentPopup();
        }
    }
    else
    {
        if(selectedFilename_.empty())
        {
            if(Button(" ok "))
            {
                InvokeCallBack();
                CloseCurrentPopup();
            }
        }
        else if(Button("open"))
            SetPwd(pwd_ / selectedFilename_);
    }

    SameLine();

    int escIdx = GetIO().KeyMap[ImGuiKey_Escape];
    if (Button("cancel") || closeFlag_ ||
        ((flags_ & ImGuiFileBrowserFlags_CloseOnEsc) && IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) && escIdx >= 0 && IsKeyPressed((ImGuiKey)escIdx)))
    {
        this->callbackFunc_ = {};
        CloseCurrentPopup();
    }

    if(!statusStr_.empty() && !(flags_ & ImGuiFileBrowserFlags_NoStatusBar))
    {
        SameLine();
        Text("%s", statusStr_.c_str());
    }

    if (!typeFilters_.empty())
    {
        SameLine();
        PushItemWidth(8 * GetFontSize());
        Combo("##type_filters", &typeFilterIndex_, typeFilters_.data(), int(typeFilters_.size()));
        PopItemWidth();
    }
}

inline bool ImGui::FileBrowser::HasSelected() const noexcept
{
    return ok_;
}

inline bool ImGui::FileBrowser::SetPwd(const std::filesystem::path &pwd)
{
    try
    {
        SetPwdUncatched(pwd);
        return true;
    }
    catch(const std::exception &err)
    {
        statusStr_ = std::string("last error: ") + err.what();
    }
    catch(...)
    {
        statusStr_ = "last error: unknown";
    }

    SetPwdUncatched(std::filesystem::current_path());
    return false;
}

inline std::filesystem::path ImGui::FileBrowser::GetSelected() const
{
    return pwd_ / selectedFilename_;
}

inline void ImGui::FileBrowser::ClearSelected()
{
    selectedFilename_ = std::string();
    (*inputNameBuf_)[0] = '\0';
    ok_ = false;
}

inline void ImGui::FileBrowser::SetTypeFilters(const std::vector<const char*> &typeFilters)
{
    typeFilters_ = typeFilters;
    typeFilterIndex_ = 0;
}

inline void ImGui::FileBrowser::SetPwdUncatched(const std::filesystem::path &pwd)
{
#ifndef IMGUI_IMPL_ICONS_FONT_AWESOME
    fileRecords_ = { FileRecord{ true, "..", "[D] ..", "" } };
#else
    fileRecords_ = { FileRecord{ true, "..", ICON_FA_FOLDER " ..", "" } };
#endif

    for(auto &p : std::filesystem::directory_iterator(pwd))
    {
        FileRecord rcd;

        if(p.is_regular_file())
            rcd.isDir = false;
        else if(p.is_directory())
            rcd.isDir = true;
        else
            continue;

        rcd.name = p.path().filename().string();
        if(rcd.name.empty())
            continue;

        rcd.extension = p.path().filename().extension().string();

#ifndef IMGUI_IMPL_ICONS_FONT_AWESOME
        rcd.showName = (rcd.isDir ? "[D] " : "[F] ") + p.path().filename().u8string();
#else
        rcd.showName = (rcd.isDir ? ICON_FA_FOLDER " " : ICON_FA_FILE " ") + p.path().filename().u8string();
#endif

        fileRecords_.push_back(rcd);
    }

    std::sort(fileRecords_.begin(), fileRecords_.end(),
        [](const FileRecord &L, const FileRecord &R)
    {
        return (L.isDir ^ R.isDir) ? L.isDir : (L.name < R.name);
    });

    pwd_ = absolute(pwd);
    selectedFilename_ = std::string();
    (*inputNameBuf_)[0] = '\0';
}

