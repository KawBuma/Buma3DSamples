#pragma once

namespace buma
{

class PlatformWindows : public PlatformBase
{
public:
    PlatformWindows(HINSTANCE   _h_instance,
                    HINSTANCE   _h_previnstance,
                    LPWSTR      _lp_cmdline,
                    int         _n_cmdshow);

    ~PlatformWindows();

    int MainLoop() override;

    HINSTANCE GetHinstance() const { return hins; }


protected:
    bool Prepare() override;
    bool PrepareWindow() override;

private:
    HINSTANCE                       hins;
    HINSTANCE                       prev_hins;
    std::string                     cmdline;
    int                             num_cmdshow;
    std::shared_ptr<WindowWindows>  window_windows;

};


}// namespace buma
