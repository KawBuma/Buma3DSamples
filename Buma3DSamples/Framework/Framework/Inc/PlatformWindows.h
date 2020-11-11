#pragma once

namespace buma
{

class ConsoleSession;

class PlatformWindows : public PlatformBase
{
public:
    inline static constexpr const wchar_t* CLASS_NAME = L"Buma3DSamples Framework";

public:
    PlatformWindows();
    virtual ~PlatformWindows();

    int MainLoop() override;

    HINSTANCE GetHinstance() const { return hins; }

    bool Init(const PLATFORM_DESC& _desc) override;
    bool Term() override;

    debug::ILogger* GetLogger() { return logger.get(); }

protected:
    static void B3D_APIENTRY B3DMessageCallback(buma3d::DEBUG_MESSAGE_SEVERITY _sev, buma3d::DEBUG_MESSAGE_CATEGORY_FLAG _category, const buma3d::Char8T* const _msg, void* _user_data);

protected:
    bool ParseCommandLines(const PLATFORM_DESC& _desc) override;
    bool PrepareDeviceResources() override;
    bool PrepareWindow(const WINDOW_DESC& _desc) override;
    bool RegisterWndClass();

private:
    WNDCLASSEXW                             wnd_class;
    HINSTANCE                               hins;
    HINSTANCE                               prev_hins;
    std::string                             cmdline;
    int                                     num_cmdshow;
    std::shared_ptr<WindowWindows>          window_windows;
    std::string                             execution_path;
    std::shared_ptr<debug::LoggerWindows>   logger;

    std::shared_ptr<ConsoleSession>         console_session;

};


}// namespace buma
