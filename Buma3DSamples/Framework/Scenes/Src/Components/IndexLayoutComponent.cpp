#include "pch.h"
#include "IndexLayoutComponent.h"

namespace buma
{
namespace scne
{

IndexLayoutComponent::IndexLayoutComponent(Scenes* _scenes)
    : ScenesObjectImpl  (_scenes)
    , data_format       {}
{

}

IndexLayoutComponent::~IndexLayoutComponent()
{

}

IndexLayoutComponent* IndexLayoutComponent::Create(Scenes* _scenes)
{
    return new IndexLayoutComponent(_scenes);
}

void IndexLayoutComponent::OnDestroy()
{
    delete this;
}

SCENES_OBJECT_TYPE IndexLayoutComponent::GetType() const
{
    return SCENES_OBJECT_TYPE::IIndexLayoutComponent;
}

void* IndexLayoutComponent::As(SCENES_OBJECT_TYPE _type)
{
         if (_type == SCENES_OBJECT_TYPE::IScenesObject)                        return static_cast<IScenesObject*>(this);
    else if (_type == SCENES_OBJECT_TYPE::IComponent)                           return static_cast<IComponent*>(this);
    else if (_type == SCENES_OBJECT_TYPE::IIndexLayoutComponent)                return static_cast<IIndexLayoutComponent*>(this);

    else if (_type == SCENES_OBJECT_TYPE_INTERNAL::IndexLayoutComponentImpl)    return static_cast<IndexLayoutComponent*>(this);
    else if (_type == SCENES_OBJECT_TYPE_INTERNAL::ScenesObjectImpl)            return static_cast<ScenesObjectImpl*>(this);

    return nullptr;
}

void IndexLayoutComponent::SetDataFormat(DATA_FORMAT _format)
{
    data_format = _format;
}

DATA_FORMAT IndexLayoutComponent::GetDataFormat()
{
    return data_format;
}


}// namespace scne
}// namespace buma
