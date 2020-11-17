#pragma once

#include "Buma3D.h"
#include "Util/Buma3DPtr.h"

#include "imgui.h"
#include <cereal/cereal.hpp>

template<class Archive>
void serialize(Archive& _ar, ImVec2& _imvec2)
{
    _ar(CEREAL_NVP(_imvec2.x), CEREAL_NVP(_imvec2.y));
}
template<class Archive>
void serialize(Archive& _ar, ImVec4& _imvec4)
{
    _ar(CEREAL_NVP(_imvec4.x), CEREAL_NVP(_imvec4.y), CEREAL_NVP(_imvec4.z), CEREAL_NVP(_imvec4.w));
}
template<class Archive>
void serialize(Archive& _ar, ImGuiDir& _imdir)
{
    _ar(CEREAL_NVP(_imdir));
}

template<class Archive>
void serialize(Archive& _ar, ImGuiStyle& _style)
{
    _ar
    (
        CEREAL_NVP(_style.Alpha)
        , CEREAL_NVP(_style.WindowPadding)
        , CEREAL_NVP(_style.WindowRounding)
        , CEREAL_NVP(_style.WindowBorderSize)
        , CEREAL_NVP(_style.WindowMinSize)
        , CEREAL_NVP(_style.WindowTitleAlign)
        , CEREAL_NVP(_style.WindowMenuButtonPosition)
        , CEREAL_NVP(_style.ChildRounding)
        , CEREAL_NVP(_style.ChildBorderSize)
        , CEREAL_NVP(_style.PopupRounding)
        , CEREAL_NVP(_style.PopupBorderSize)
        , CEREAL_NVP(_style.FramePadding)
        , CEREAL_NVP(_style.FrameRounding)
        , CEREAL_NVP(_style.FrameBorderSize)
        , CEREAL_NVP(_style.ItemSpacing)
        , CEREAL_NVP(_style.ItemInnerSpacing)
        , CEREAL_NVP(_style.TouchExtraPadding)
        , CEREAL_NVP(_style.IndentSpacing)
        , CEREAL_NVP(_style.ColumnsMinSpacing)
        , CEREAL_NVP(_style.ScrollbarSize)
        , CEREAL_NVP(_style.ScrollbarRounding)
        , CEREAL_NVP(_style.GrabMinSize)
        , CEREAL_NVP(_style.GrabRounding)
        , CEREAL_NVP(_style.TabRounding)
        , CEREAL_NVP(_style.TabBorderSize)
        , CEREAL_NVP(_style.ColorButtonPosition)
        , CEREAL_NVP(_style.ButtonTextAlign)
        , CEREAL_NVP(_style.SelectableTextAlign)
        , CEREAL_NVP(_style.DisplayWindowPadding)
        , CEREAL_NVP(_style.DisplaySafeAreaPadding)
        , CEREAL_NVP(_style.MouseCursorScale)
        , CEREAL_NVP(_style.AntiAliasedLines)
        , CEREAL_NVP(_style.AntiAliasedFill)
        , CEREAL_NVP(_style.CurveTessellationTol)
        , CEREAL_NVP(_style.Colors[ImGuiCol_Text])
        , CEREAL_NVP(_style.Colors[ImGuiCol_TextDisabled])
        , CEREAL_NVP(_style.Colors[ImGuiCol_WindowBg])
        , CEREAL_NVP(_style.Colors[ImGuiCol_ChildBg])
        , CEREAL_NVP(_style.Colors[ImGuiCol_PopupBg])
        , CEREAL_NVP(_style.Colors[ImGuiCol_Border])
        , CEREAL_NVP(_style.Colors[ImGuiCol_BorderShadow])
        , CEREAL_NVP(_style.Colors[ImGuiCol_FrameBg])
        , CEREAL_NVP(_style.Colors[ImGuiCol_FrameBgHovered])
        , CEREAL_NVP(_style.Colors[ImGuiCol_FrameBgActive])
        , CEREAL_NVP(_style.Colors[ImGuiCol_TitleBg])
        , CEREAL_NVP(_style.Colors[ImGuiCol_TitleBgActive])
        , CEREAL_NVP(_style.Colors[ImGuiCol_TitleBgCollapsed])
        , CEREAL_NVP(_style.Colors[ImGuiCol_MenuBarBg])
        , CEREAL_NVP(_style.Colors[ImGuiCol_ScrollbarBg])
        , CEREAL_NVP(_style.Colors[ImGuiCol_ScrollbarGrab])
        , CEREAL_NVP(_style.Colors[ImGuiCol_ScrollbarGrabHovered])
        , CEREAL_NVP(_style.Colors[ImGuiCol_ScrollbarGrabActive])
        , CEREAL_NVP(_style.Colors[ImGuiCol_CheckMark])
        , CEREAL_NVP(_style.Colors[ImGuiCol_SliderGrab])
        , CEREAL_NVP(_style.Colors[ImGuiCol_SliderGrabActive])
        , CEREAL_NVP(_style.Colors[ImGuiCol_Button])
        , CEREAL_NVP(_style.Colors[ImGuiCol_ButtonHovered])
        , CEREAL_NVP(_style.Colors[ImGuiCol_ButtonActive])
        , CEREAL_NVP(_style.Colors[ImGuiCol_Header])
        , CEREAL_NVP(_style.Colors[ImGuiCol_HeaderHovered])
        , CEREAL_NVP(_style.Colors[ImGuiCol_HeaderActive])
        , CEREAL_NVP(_style.Colors[ImGuiCol_Separator])
        , CEREAL_NVP(_style.Colors[ImGuiCol_SeparatorHovered])
        , CEREAL_NVP(_style.Colors[ImGuiCol_SeparatorActive])
        , CEREAL_NVP(_style.Colors[ImGuiCol_ResizeGrip])
        , CEREAL_NVP(_style.Colors[ImGuiCol_ResizeGripHovered])
        , CEREAL_NVP(_style.Colors[ImGuiCol_ResizeGripActive])
        , CEREAL_NVP(_style.Colors[ImGuiCol_Tab])
        , CEREAL_NVP(_style.Colors[ImGuiCol_TabHovered])
        , CEREAL_NVP(_style.Colors[ImGuiCol_TabActive])
        , CEREAL_NVP(_style.Colors[ImGuiCol_TabUnfocused])
        , CEREAL_NVP(_style.Colors[ImGuiCol_TabUnfocusedActive])
        , CEREAL_NVP(_style.Colors[ImGuiCol_PlotLines])
        , CEREAL_NVP(_style.Colors[ImGuiCol_PlotLinesHovered])
        , CEREAL_NVP(_style.Colors[ImGuiCol_PlotHistogram])
        , CEREAL_NVP(_style.Colors[ImGuiCol_PlotHistogramHovered])
        , CEREAL_NVP(_style.Colors[ImGuiCol_TextSelectedBg])
        , CEREAL_NVP(_style.Colors[ImGuiCol_DragDropTarget])
        , CEREAL_NVP(_style.Colors[ImGuiCol_NavHighlight])
        , CEREAL_NVP(_style.Colors[ImGuiCol_NavWindowingHighlight])
        , CEREAL_NVP(_style.Colors[ImGuiCol_NavWindowingDimBg])
        , CEREAL_NVP(_style.Colors[ImGuiCol_ModalWindowDimBg])
    );
}

namespace buma
{
namespace gui
{

struct MYIMGUI_CREATE_DESC
{
    buma3d::SHADER_BYTECODE compiled_vertex_shader{};
    buma3d::SHADER_BYTECODE compiled_pixel_shader{};
    const ImGuiConfigFlags  config_flags = ImGuiConfigFlags_None;
};

class MyImGui
{
public:
    enum class eImGuiStyleColor : int { Classic, Dark, Light };

public:
    MyImGui();
    ~MyImGui();

    bool Init(MYIMGUI_CREATE_DESC& _desc);

    void NewFrame();
    void DrawGui(buma3d::ICommandList* _cmd_list);

    void Destroy();

    void SetStyleColor(eImGuiStyleColor _e_style_col);

    void SetStyle(ImGuiStyle& _style);

    void LoadStyle(const char* _path);
    void SaveStyle(const char* _path);

private:
    bool CreateContext(ImGuiConfigFlags _config_flags);

private:
    ImGuiContext*                                   p_imgui_ctx;

    buma3d::util::Ptr<buma3d::ITexture>             font_texture;
    buma3d::util::Ptr<buma3d::IRootSignature>       root_signature;
    buma3d::util::Ptr<buma3d::IPipelineState>       pipeline_state;

};


}//namespace gui
}//namespace buma
