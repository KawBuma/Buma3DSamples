#include "pch.h"
#include "ResourceTexture.h"
#include "ResourceImpl.h"

namespace buma
{
namespace res
{

ResourceTextureImpl::ResourceTextureImpl(std::shared_ptr<DeviceResources>& _dr, RESOURCE_CREATE_TYPE _create_type)
    : ResourceImpl(_dr, _create_type)
{

}

ResourceTextureImpl::~ResourceTextureImpl()
{

}

std::unique_ptr<ResourceTextureImpl> ResourceTextureImpl::Create(std::shared_ptr<DeviceResources>& _dr, RESOURCE_CREATE_TYPE _create_type
                                                                 , const buma3d::RESOURCE_DESC& _desc, buma3d::RESOURCE_HEAP_PROPERTY_FLAGS _heap_flags, buma3d::RESOURCE_HEAP_PROPERTY_FLAGS _deny_heap_flags)
{
    auto result = std::make_unique<ResourceTextureImpl>(_dr, _create_type);
    if (!result->Init(_desc, _heap_flags, _deny_heap_flags))
        return nullptr;

    return result;
}

bool ResourceTextureImpl::Init(const buma3d::RESOURCE_DESC& _desc, buma3d::RESOURCE_HEAP_PROPERTY_FLAGS _heap_flags, buma3d::RESOURCE_HEAP_PROPERTY_FLAGS _deny_heap_flags)
{
    auto&& d = dr->GetDevice();
    auto bmr = d->CreatePlacedResource(_desc, &resource);
    if (util::IsFailed(bmr))
        return false;

    if (!AllocateHeap(_heap_flags, _deny_heap_flags))
        return false;

    if (!Bind())
        return false;

    return true;
}


}// namespace res
}// namespace buma
