#pragma once

namespace buma
{

class ConsoleSession;

namespace input
{

class PCInputsWindows : public input::PCInputs
{
public:
    PCInputsWindows()
        : key{}
        , mouse{}
        , gpad{}
    {
    }

    ~PCInputsWindows()
    {

    }

    input::KeyboardInput&  GetKeyboardInput () { return key; }
    input::MouseInput&     GetMouseInput    () { return mouse; }
    input::GamePadInputs&  GetGamePadInputs () { return gpad; }
    const void Update(float _delta_time)
    {
        key.Update(_delta_time);
        mouse.Update(_delta_time);
        //gpad.Update(_delta_time);
    }

    const input::KEY_PRESS_DATA&   GetKey()                 override { return key.KeyPressData(); }
    const input::MOUSE_DATA&       GetMouse()               override { return mouse.MouseData(); }
    const input::GAMEPAD_DATA&     GetGpad(int _player = 0) override { return gpad.GamePadData(_player); }

private:
    input::KeyboardInput    key;
    input::MouseInput       mouse;
    input::GamePadInputs    gpad;

};

}// namespace input


class PlatformWindows : public PlatformBase
{
public:
    inline static constexpr const wchar_t* CLASS_NAME = L"Buma3DSamples Framework";

public:
    PlatformWindows();
    virtual ~PlatformWindows();

    int MainLoop() override;
    void ProcessMain();

    HINSTANCE GetHinstance() const { return hins; }

    bool Prepare(const PLATFORM_DESC& _desc) override;
    bool Init() override;
    bool Term() override;

    debug::ILogger* GetLogger() { return logger.get(); }
    input::PCInputs* GetInputs() { return inputs.get(); }
    input::PCInputsWindows* GetInputsImpl() { return inputs.get(); }

protected:
    static void B3D_APIENTRY B3DMessageCallback(buma3d::DEBUG_MESSAGE_SEVERITY _sev, buma3d::DEBUG_MESSAGE_CATEGORY_FLAG _category, const buma3d::Char8T* const _msg, void* _user_data);

protected:
    bool ParseCommandLines(const PLATFORM_DESC& _desc) override;
    std::vector<std::unique_ptr<std::string>>::iterator FindArgument(const char* _find_str);
    bool PrepareDeviceResources() override;
    bool PrepareWindow() override;
    bool RegisterWndClass();
    bool PrepareLog();

private:
    WNDCLASSEXW                             wnd_class;
    HINSTANCE                               hins;
    HINSTANCE                               prev_hins;
    std::string                             cmdline;
    int                                     num_cmdshow;
    std::shared_ptr<WindowWindows>          window_windows;
    std::string                             execution_path;
    std::shared_ptr<debug::LoggerWindows>   logger;
    std::unique_ptr<input::PCInputsWindows> inputs;

    std::shared_ptr<ConsoleSession>         console_session;

};


}// namespace buma
