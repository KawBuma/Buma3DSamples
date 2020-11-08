#pragma once

namespace buma
{

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

protected:
    bool PrepareDeviceResources() override;
    bool PrepareWindow() override;
    bool RegisterWndClass();

private:
    WNDCLASSEXW                     wnd_class;
    HINSTANCE                       hins;
    HINSTANCE                       prev_hins;
    std::string                     cmdline;
    int                             num_cmdshow;
    std::shared_ptr<WindowWindows>  window_windows;

};


}// namespace buma
