#pragma once

namespace buma
{
namespace res
{

class ResourceTextureImpl : public IResourceTexture, private ResourceImpl
{
public:
    ResourceTextureImpl(std::shared_ptr<DeviceResources>& _dr, RESOURCE_CREATE_TYPE _create_type);
    ResourceTextureImpl(const IResource&) = delete;
    virtual ~ResourceTextureImpl();

    static std::unique_ptr<ResourceTextureImpl> Create(std::shared_ptr<DeviceResources>& _dr, RESOURCE_CREATE_TYPE _create_type
                                                       , const buma3d::RESOURCE_DESC& _desc, buma3d::RESOURCE_HEAP_PROPERTY_FLAGS _heap_flags, buma3d::RESOURCE_HEAP_PROPERTY_FLAGS  _deny_heap_flags);
    
    RESOURCE_CREATE_TYPE                        IResourceTexture::GetResourceCreateType() const override { return ResourceImpl::GetResourceCreateTypeImpl(); }
    const RESOURCE_HEAP_ALLOCATION&             IResourceTexture::GetAllocation()         const override { return ResourceImpl::GetAllocationImpl(); }
    const buma3d::util::Ptr<buma3d::IResource>& IResourceTexture::GetB3DResource()        const override { return ResourceImpl::GetB3DResourceImpl(); }

private:
    bool Init(const buma3d::RESOURCE_DESC& _desc, buma3d::RESOURCE_HEAP_PROPERTY_FLAGS _heap_flags, buma3d::RESOURCE_HEAP_PROPERTY_FLAGS  _deny_heap_flags);

};


}// namespace res
}// namespace buma
