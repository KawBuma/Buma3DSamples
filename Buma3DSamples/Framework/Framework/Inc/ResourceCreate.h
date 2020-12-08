#pragma once

namespace buma
{
namespace res
{

class ResourceCreate
{
public:
    ResourceCreate(const std::shared_ptr<DeviceResources>& _dr);
    ~ResourceCreate();

    std::unique_ptr<buma::res::IResourceBuffer> CreateBuffer(
          const buma3d::RESOURCE_DESC&          _desc
        , buma3d::RESOURCE_HEAP_PROPERTY_FLAGS  _heap_flags         = buma3d::RESOURCE_HEAP_PROPERTY_FLAG_DEVICE_LOCAL
        , buma3d::RESOURCE_HEAP_PROPERTY_FLAGS  _deny_heap_flags    = buma3d::RESOURCE_HEAP_PROPERTY_FLAG_ACCESS_GENERIC_MEMORY_READ_FIXED | buma3d::RESOURCE_HEAP_PROPERTY_FLAG_ACCESS_COPY_DST_FIXED);

    std::unique_ptr<buma::res::IResourceTexture> CreateTexture(
          const buma3d::RESOURCE_DESC&          _desc
        , buma3d::RESOURCE_HEAP_PROPERTY_FLAGS  _heap_flags         = buma3d::RESOURCE_HEAP_PROPERTY_FLAG_DEVICE_LOCAL
        , buma3d::RESOURCE_HEAP_PROPERTY_FLAGS  _deny_heap_flags    = buma3d::RESOURCE_HEAP_PROPERTY_FLAG_ACCESS_GENERIC_MEMORY_READ_FIXED | buma3d::RESOURCE_HEAP_PROPERTY_FLAG_ACCESS_COPY_DST_FIXED);

private:
    DeviceResources* dr;

};


}// namespace res
}// namespace buma
