#pragma once

namespace buma
{
namespace res
{

enum RESOURCE_CREATE_TYPE
{
      RESOURCE_CREATE_TYPE_COMMITTED
    , RESOURCE_CREATE_TYPE_PLACED
    , RESOURCE_CREATE_TYPE_RESERVED
};

struct IResource
{
    //IResource(const IResource&) = delete;
    virtual ~IResource() {}

    virtual RESOURCE_CREATE_TYPE                        GetResourceCreateType() const = 0;
    virtual const RESOURCE_HEAP_ALLOCATION&             GetAllocation()  const = 0;
    virtual const buma3d::util::Ptr<buma3d::IResource>& GetB3DResource() const = 0;

    buma3d::BMRESULT             SetName(const char* _name)       { return GetB3DResource()->SetName(_name); }
    const char*                  GetName()                        { return GetB3DResource()->GetName(); }
    const buma3d::RESOURCE_DESC& GetB3DDesc()               const { return GetB3DResource()->GetDesc(); }

};


}// namespace res
}// namespace buma

