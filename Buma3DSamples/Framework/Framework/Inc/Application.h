#pragma once

#define BUMA_DLL_API extern "C" __declspec(dllexport)

namespace buma
{

class PlatformBase;

struct APPLICATION_SETTINGS
{
    std::string     asset_path;
    WINDOW_DESC     window_desc;
    bool            is_disabled_vsync;
    bool            is_enabled_fullscreen;
};

class ApplicationBase
{
public:
    ApplicationBase();
    ApplicationBase(const ApplicationBase&) = delete;
    virtual ~ApplicationBase();

    virtual bool Prepare(PlatformBase& _platform) = 0;

    std::shared_ptr<DeviceResources>    GetDeviceResources() const { return dr; }
    const APPLICATION_SETTINGS&         GetSettings()        const { return settings; }

    virtual bool Init() = 0;
    virtual void Tick() = 0;
    virtual void Term() = 0;

protected:
    virtual bool PrepareSettings();
    std::string AssetPath(const char* _str) { return settings.asset_path + _str; }

protected:
    std::shared_ptr<DeviceResources>    dr;
    APPLICATION_SETTINGS                settings;

};

using PFN_CreateApplication = ApplicationBase * (*)();


}// namespace buma
