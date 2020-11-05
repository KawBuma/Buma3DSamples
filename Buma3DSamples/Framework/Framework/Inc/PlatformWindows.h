#pragma once

namespace buma
{

class PlatformWindows : public PlatformBase
{
public:
    PlatformWindows(HINSTANCE   _h_instance,
                    HINSTANCE   _h_previnstance,
                    LPWSTR      _lp_cmdline,
                    int         _n_cmdshow)
        : hins          { _h_instance }
        , prev_hins     { _h_previnstance } 
        , cmdline       { util::ConvertWideToAnsi(_lp_cmdline) }
        , num_cmdshow   { _n_cmdshow } 
    {
        Prepare();
    }

    ~PlatformWindows()
    {
    }

    int MainLoop() override
    {
        int result = 0;
        while (true)
        {

        }

        return result;
    }

    HINSTANCE GetHinstance() const { return hins; }


protected:
    bool Prepare() override
    {
        auto res = PrepareWindow();

        return res;
    }

    bool PrepareWindow() override
    {
        window = window_windows = std::make_shared<WindowWindows>(*this);
        auto res = window_windows->Init();
        return res;
    }

private:
    HINSTANCE                       hins;
    HINSTANCE                       prev_hins;
    std::string                     cmdline;
    int                             num_cmdshow;
    std::shared_ptr<WindowWindows>  window_windows;
    

};


}// namespace buma
