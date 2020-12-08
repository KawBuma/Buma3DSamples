#include "pch.h"
#include "ResourceCreate.h"
#include "ResourceTextureImpl.h"
#include "ResourceBufferImpl.h"

namespace buma
{
namespace res
{

ResourceCreate::ResourceCreate(const std::shared_ptr<DeviceResources>& _dr)
    : dr{ _dr.get() }
{
}

ResourceCreate::~ResourceCreate()
{
}


std::unique_ptr<buma::res::IResourceBuffer> ResourceCreate::CreateBuffer(const buma3d::RESOURCE_DESC& _desc, buma3d::RESOURCE_HEAP_PROPERTY_FLAGS _heap_flags, buma3d::RESOURCE_HEAP_PROPERTY_FLAGS  _deny_heap_flags)
{
    return ResourceBufferImpl::Create(dr->shared_from_this(), RESOURCE_CREATE_TYPE_PLACED, _desc, _heap_flags, _deny_heap_flags);
}

std::unique_ptr<buma::res::IResourceTexture> ResourceCreate::CreateTexture(const buma3d::RESOURCE_DESC& _desc, buma3d::RESOURCE_HEAP_PROPERTY_FLAGS _heap_flags, buma3d::RESOURCE_HEAP_PROPERTY_FLAGS  _deny_heap_flags)
{
    return ResourceTextureImpl::Create(dr->shared_from_this(), RESOURCE_CREATE_TYPE_PLACED, _desc, _heap_flags, _deny_heap_flags);
}


}// namespace res
}// namespace buma
