#pragma once

namespace buma
{

class PlatformWindows : public PlatformBase
{
public:
    PlatformWindows()
    {
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

protected:
    bool Prepare() override
    {

        return true;
    }

    bool PrepareWindow() override
    {

        return true;
    }

private:
    HINSTANCE   hins;

};

class WindowWindows
{
public:
    WindowWindows(PlatformWindows& _platform)
        : platform  { _platform }
        , hwnd      {}
    {
    }

    ~WindowWindows()
    {

    }

private:
    PlatformWindows&    platform;
    HWND                hwnd;

};



}// namespace buma
