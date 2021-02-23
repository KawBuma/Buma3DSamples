#include "pch.h"
#include "TextureComponent.h"

namespace buma
{
namespace scne
{

TextureComponent::TextureComponent(Scenes* _scenes)
    : ScenesObjectImpl  (_scenes)
    , filename          {}
{

}

TextureComponent::~TextureComponent()
{

}

TextureComponent* TextureComponent::Create(Scenes* _scenes)
{
    return new TextureComponent(_scenes);
}

void TextureComponent::OnDestroy()
{
    delete this;
}

SCENES_OBJECT_TYPE TextureComponent::GetType() const
{
    return SCENES_OBJECT_TYPE::ITextureComponent;
}

void* TextureComponent::As(SCENES_OBJECT_TYPE _type)
{
         if (_type == SCENES_OBJECT_TYPE::IScenesObject)                    return static_cast<IScenesObject*>(this);
    else if (_type == SCENES_OBJECT_TYPE::IComponent)                       return static_cast<IComponent*>(this);
    else if (_type == SCENES_OBJECT_TYPE::ITextureComponent)                return static_cast<ITextureComponent*>(this);

    else if (_type == SCENES_OBJECT_TYPE_INTERNAL::TextureComponentImpl)    return static_cast<TextureComponent*>(this);
    else if (_type == SCENES_OBJECT_TYPE_INTERNAL::ScenesObjectImpl)        return static_cast<ScenesObjectImpl*>(this);

    return nullptr;
}

void TextureComponent::SetFilename(const char* _filename)
{
    if (!_filename)
    {
        filename.clear();
        return;
    }

    filename = _filename;
}

const char* TextureComponent::GetFilename()
{
    return filename.c_str();
}


}// namespace scne
}// namespace buma
