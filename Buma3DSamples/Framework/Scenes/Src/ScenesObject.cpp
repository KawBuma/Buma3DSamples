#include "pch.h"
#include "ScenesObject.h"

namespace buma
{
namespace scne
{

ScenesObjectImpl::ScenesObjectImpl(Scenes* _scenes)
    : ref_count { 1 }
    , scenes    { _scenes }
    , name      {}
{

}

ScenesObjectImpl::~ScenesObjectImpl()
{

}

uint32_t ScenesObjectImpl::Release()
{
    auto result = --ref_count;
    if (result == 0)
        OnDestroy();

    return result;
}

uint32_t ScenesObjectImpl::AddRef()
{
    return ++ref_count;
}

const char* ScenesObjectImpl::GetName() const
{
    return name ? name->c_str() : nullptr;
}

void ScenesObjectImpl::SetName(const char* _name)
{
    if (!_name)
        return name.reset();

    if (!name)
        name = std::make_unique<std::string>(_name);
    else
        *name = _name;
}

IScenes* ScenesObjectImpl::GetScenes()
{
    return scenes;
}


}// namespace scne
}// namespace buma
