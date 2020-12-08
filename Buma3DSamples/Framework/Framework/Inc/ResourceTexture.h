#pragma once

namespace buma
{
namespace res
{

struct IResourceTexture : public IResource
{
    virtual ~IResourceTexture() {}

    buma3d::util::Ptr<buma3d::ITexture> GetB3DTexture() const { return GetB3DResource().As<buma3d::ITexture>(); }

};


}// namespace res
}// namespace buma

