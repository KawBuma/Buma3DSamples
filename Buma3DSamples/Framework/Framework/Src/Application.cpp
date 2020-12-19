#include "pch.h"
#include "Application.h"

#undef GetObject
#include "../../rapidjson/include/rapidjson/document.h"
#include "../../rapidjson/include/rapidjson/rapidjson.h"

namespace buma
{

ApplicationBase::ApplicationBase()
    : dr        {}
    , settings  {}
{
}

ApplicationBase::~ApplicationBase()
{

}

bool ApplicationBase::PrepareSettings()
{
    std::ifstream ifs("b3dsample_settings.json");
    if (ifs.fail()) return false;

    auto s = std::string(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());

    rapidjson::Document d{};
    if (d.Parse(s.c_str()).HasParseError()) return false;
    if (!d.HasMember("Sample"))             return false;

    auto&& o = d["Sample"].GetObject();
    if (!o.HasMember("ASSET_PATH"))         return false;
    if (!o.HasMember("WIDTH"))              return false;
    if (!o.HasMember("HEIGHT"))             return false;
    if (!o.HasMember("DISABLE_VSYNC"))      return false;
    if (!o.HasMember("ENABLE_FULLSCREEN"))  return false;

    settings.asset_path             = o["ASSET_PATH"       ].GetString();
    settings.window_desc.width      = o["WIDTH"            ].GetUint();
    settings.window_desc.height     = o["HEIGHT"           ].GetUint();
    settings.is_disabled_vsync      = o["DISABLE_VSYNC"    ].GetBool();
    settings.is_enabled_fullscreen  = o["ENABLE_FULLSCREEN"].GetBool();

    return true;
}


}// namespace buma

