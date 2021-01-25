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

enum MYIMGUI_CREATE_FLAG
{
      MYIMGUI_CREATE_FLAG_NONE                              = 0x0

    // このフラグが指定された場合、RecordGuiCommandsの呼び出しが可能です。 
    // ImGui::Image等による任意の画像用ディスクリプタプールを、必要に応じて追加作成します。
    // これにより、ディスクリプタの枯渇による描画コマンド実行の分割を回避できますが、メモリを過剰に消費する可能性があります。 
    , MYIMGUI_CREATE_FLAG_DESCRIPTOR_POOL_FEEDING           = 0x1

    // 各ビューポートのコマンドを全て RecordGuiCommands::_list 上で書き込みます。 
    // DESCRIPTOR_POOL_FEEDING も同時に含まれている必要があります。
    , MYIMGUI_CREATE_FLAG_USE_SINGLE_COMMAND_LIST           = 0x2
};
using MYIMGUI_CREATE_FLAGS = uint32_t;

struct MYIMGUI_CREATE_DESC
{
    void*                       window_handle;
    int                         config_flags; // ImGuiConfigFlags
    buma3d::RESOURCE_FORMAT     framebuffer_format;
    MYIMGUI_CREATE_FLAGS        flags;
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

    void DrawGui(buma3d::IFramebuffer* _framebuffer, buma3d::RESOURCE_STATE _current_state, buma3d::RESOURCE_STATE _state_after);
    void RecordGuiCommands(buma3d::ICommandList* _list, buma3d::IFramebuffer* _framebuffer, buma3d::RESOURCE_STATE _current_state, buma3d::RESOURCE_STATE _state_after);

    void SubmitCommands();
    void PresentViewports();

    void Destroy();

private:
    class MyImGuiImpl;
    MyImGuiImpl* impl;

};


}//namespace gui
}//namespace buma
