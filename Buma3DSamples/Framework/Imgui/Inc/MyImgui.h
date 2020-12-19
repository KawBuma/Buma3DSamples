#pragma once
#include <memory>

#include "Buma3D.h"
#include "Util/Buma3DPtr.h"

namespace buma
{
class DeviceResources;
class ProcessMessageEventArgs;
}

namespace buma
{
namespace gui
{

struct MYIMGUI_CREATE_DESC
{
    void*                       window_handle;
    int                         config_flags; // ImGuiConfigFlags
    buma3d::RESOURCE_FORMAT     framebuffer_format;
};

class MyImGui
{
public:
    MyImGui(const std::shared_ptr<DeviceResources>& _dr);
    ~MyImGui();

    bool Init(const MYIMGUI_CREATE_DESC& _desc);

    void OnProcessMessage(ProcessMessageEventArgs* _args);

    buma3d::BMRESULT CreateFramebuffer(buma3d::IView* _rtv, buma3d::IFramebuffer** _dst);

    void NewFrame();

    // 現在RESOURCE_STATE_COLOR_ATTACHMENT_READ_WRITEが設定されている必要があります。
    void DrawGui(buma3d::IFramebuffer* _framebuffer);

    void Destroy();

private:
    class MyImGuiImpl;
    MyImGuiImpl* impl;

};


}//namespace gui
}//namespace buma
