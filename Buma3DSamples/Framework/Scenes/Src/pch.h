#ifndef PCH_H
#define PCH_H

#define NOMINMAX

#include <stddef.h>
#include <typeinfo>
#include <typeindex>
#include <vector>

#include <new>
#include <memory>
#include <string>
#include <map>
#include <unordered_map>

#include <fstream>
#include <sstream>
#include <filesystem>

#define GLM_ENABLE_EXPERIMENTAL
// to use a clip space between 0 to 1.
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
// For DirectX, Metal, Vulkan
#define GLM_FORCE_LEFT_HANDED 
#include <glm/glm.hpp>
#include <glm/matrix.hpp>
#include <glm/ext.hpp>

#include "Scenes.h"


namespace buma
{
namespace scne
{

enum class SCENES_OBJECT_TYPE_INTERNAL : uint32_t
{
    //  IScenesObject
    //, IComponent
    //, IScene
    //, INode
    //, IValueComponent
    //, ITransformComponent
    //, IAABBComponent
    //, ICameraComponent
    //, ILightComponent
    //, ILightPropertyComponent
    //, IMaterialComponent
    //, IMaterialPropertyComponent
    //, ITextureComponent
    //, ISamplerComponent
    //, IBufferComponent
    //, IBufferViewComponent
    //, IIndexLayoutComponent
    //, IVertexLayoutComponent
    //, IVertexElementComponent
    //, IVertexBufferComponent
    //, IIndexBufferComponent
    //, ISubmeshComponent
    //, IMeshComponent

    //, SCENES_OBJECT_TYPE_NUM_TYPES
    //, SCENES_OBJECT_TYPE_UNDEFINED

      SceneImpl = uint32_t(SCENES_OBJECT_TYPE::SCENES_OBJECT_TYPE_UNDEFINED) + 0xffff
    , NodeImpl 
    , ValueComponentImpl
    , TransformComponentImpl
    , AABBComponentImpl
    , CameraComponentImpl
    , LightComponentImpl
    , LightPropertyComponentImpl
    , MaterialComponentImpl
    , MaterialPropertyComponentImpl
    , TextureComponentImpl
    , SamplerComponentImpl
    , BufferComponentImpl
    , BufferViewComponentImpl
    , IndexLayoutComponentImpl
    , VertexLayoutComponentImpl
    , VertexElementComponentImpl
    , VertexBufferComponentImpl
    , IndexBufferComponentImpl
    , SubmeshComponentImpl
    , MeshComponentImpl

    , ScenesObjectImpl

    , SCENES_OBJECT_TYPE_INTERNAL_END
    , SCENES_OBJECT_TYPE_INTERNAL_BEGIN = SceneImpl
    , SCENES_OBJECT_TYPE_INTERNAL_NUM_TYPES = SCENES_OBJECT_TYPE_INTERNAL_END - SCENES_OBJECT_TYPE_INTERNAL_BEGIN
    , SCENES_OBJECT_TYPE_INTERNAL_UNDEFINED
};

inline bool operator== (SCENES_OBJECT_TYPE          _type , SCENES_OBJECT_TYPE_INTERNAL _typei) { return uint32_t(_type)  == uint32_t(_typei); }
inline bool operator== (SCENES_OBJECT_TYPE_INTERNAL _typei, SCENES_OBJECT_TYPE          _type)  { return uint32_t(_typei) == uint32_t(_type); }
inline bool operator!= (SCENES_OBJECT_TYPE          _type , SCENES_OBJECT_TYPE_INTERNAL _typei) { return uint32_t(_type)  != uint32_t(_typei); }
inline bool operator!= (SCENES_OBJECT_TYPE_INTERNAL _typei, SCENES_OBJECT_TYPE          _type)  { return uint32_t(_typei) != uint32_t(_type); }

inline bool operator== (SCENES_OBJECT_TYPE _typei, SCENES_OBJECT_TYPE _type)  { return uint32_t(_typei) == uint32_t(_type); }
inline bool operator!= (SCENES_OBJECT_TYPE _type , SCENES_OBJECT_TYPE _typei) { return uint32_t(_type)  != uint32_t(_typei); }

template<typename T> inline SCENES_OBJECT_TYPE_INTERNAL GetScenesObjectTypeInternal() { return SCENES_OBJECT_TYPE_INTERNAL::SCENES_OBJECT_TYPE_INTERNAL_UNDEFINED; }

template<typename T> inline T* GetAs(IScenesObject* _obj)
{
    return _obj
        ? (T*)(_obj->As((SCENES_OBJECT_TYPE)GetScenesObjectTypeInternal<T>()))
        : nullptr;
}

template<typename T>
inline typename std::vector<T>::iterator FindByName(std::vector<T>& _v, const char* _name)
{
    return std::find_if(_v.begin(), _v.end(), [_name](const T& _prop)
    { return _prop->GetName() ? strcmp(_prop->GetName(), _name) == 0 : false; });
}

template<typename T, typename U>
inline bool EraseElem(std::vector<T>& _v, U* _obj)
{
    auto it_find = std::find(_v.begin(), _v.end(), _obj);
    if (it_find == _v.end())
        return false;

    _v.erase(it_find);
    return true;
}

template <typename T>
class ScopedRef
{
public:
    ScopedRef()
        : ptr{}
    {
    }
    ScopedRef(T* _ptr)
        : ptr{ _ptr }
    {
        if (ptr)
            ptr->AddRef();
    }
    ScopedRef(const ScopedRef<T>& _ptr)
    {
        Reset(_ptr.ptr);
    }
    ScopedRef<T>& operator= (const ScopedRef<T>& _ptr)
    {
        Reset(_ptr.ptr);
        return *this;
    }
    ScopedRef<T>& operator= (T* _ptr)
    {
        Reset(_ptr);
        return *this;
    }
    ~ScopedRef() { if (ptr) ptr->Release(); }

    void Reset(T* _ptr = nullptr)
    {
        if (ptr)
            ptr->Release();
        ptr = _ptr;
        if (ptr)
            ptr->AddRef();
    }

    T* Detach()
    {
        auto result = ptr;
        ptr = nullptr;
        return result;
    }

    T* Get() const 
    {
        return ptr;
    }

    T* operator->()
    {
        return ptr;
    }
    const T* operator->() const 
    {
        return ptr;
    }

private:
    T* ptr;

};

template<typename T, typename U>
inline bool operator==(const ScopedRef<T>& _a, const ScopedRef<U>& _b)
{ return _a.Get() == _b.Get(); }

template<typename T, typename U>
inline bool operator!=(const ScopedRef<T>& _a, const ScopedRef<U>& _b)
{ return _a.Get() != _b.Get(); }

template<typename T, typename U>
inline bool operator==(const ScopedRef<T>& _a, const U* _b)
{ return _a.Get() == _b; }

template<typename T, typename U>
inline bool operator!=(const ScopedRef<T>& _a, const U* _b)
{ return _a.Get() != _b; }

class ScenesObjectImpl;
class Scene;
class Node;
class ValueComponent;
class TransformComponent;
class AABBComponent;
class CameraComponent;
class LightComponent;
class LightPropertyComponent;
class MaterialComponent;
class MaterialPropertyComponent;
class TextureComponent;
class SamplerComponent;
class BufferComponent;
class BufferViewComponent;
class IndexLayoutComponent;
class VertexLayoutComponent;
class VertexElementComponent;
class IndexBufferComponent;
class VertexBufferComponent;
class SubmeshComponent;
class MeshComponent;
template<> extern SCENES_OBJECT_TYPE_INTERNAL GetScenesObjectTypeInternal<ScenesObjectImpl>();
template<> extern SCENES_OBJECT_TYPE_INTERNAL GetScenesObjectTypeInternal<Scene>();
template<> extern SCENES_OBJECT_TYPE_INTERNAL GetScenesObjectTypeInternal<Node>();
template<> extern SCENES_OBJECT_TYPE_INTERNAL GetScenesObjectTypeInternal<ValueComponent>();
template<> extern SCENES_OBJECT_TYPE_INTERNAL GetScenesObjectTypeInternal<TransformComponent>();
template<> extern SCENES_OBJECT_TYPE_INTERNAL GetScenesObjectTypeInternal<AABBComponent>();
template<> extern SCENES_OBJECT_TYPE_INTERNAL GetScenesObjectTypeInternal<CameraComponent>();
template<> extern SCENES_OBJECT_TYPE_INTERNAL GetScenesObjectTypeInternal<LightComponent>();
template<> extern SCENES_OBJECT_TYPE_INTERNAL GetScenesObjectTypeInternal<LightPropertyComponent>();
template<> extern SCENES_OBJECT_TYPE_INTERNAL GetScenesObjectTypeInternal<MaterialComponent>();
template<> extern SCENES_OBJECT_TYPE_INTERNAL GetScenesObjectTypeInternal<MaterialPropertyComponent>();
template<> extern SCENES_OBJECT_TYPE_INTERNAL GetScenesObjectTypeInternal<TextureComponent>();
template<> extern SCENES_OBJECT_TYPE_INTERNAL GetScenesObjectTypeInternal<SamplerComponent>();
template<> extern SCENES_OBJECT_TYPE_INTERNAL GetScenesObjectTypeInternal<BufferComponent>();
template<> extern SCENES_OBJECT_TYPE_INTERNAL GetScenesObjectTypeInternal<BufferViewComponent>();
template<> extern SCENES_OBJECT_TYPE_INTERNAL GetScenesObjectTypeInternal<IndexLayoutComponent>();
template<> extern SCENES_OBJECT_TYPE_INTERNAL GetScenesObjectTypeInternal<VertexLayoutComponent>();
template<> extern SCENES_OBJECT_TYPE_INTERNAL GetScenesObjectTypeInternal<VertexElementComponent>();
template<> extern SCENES_OBJECT_TYPE_INTERNAL GetScenesObjectTypeInternal<IndexBufferComponent>();
template<> extern SCENES_OBJECT_TYPE_INTERNAL GetScenesObjectTypeInternal<VertexBufferComponent>();
template<> extern SCENES_OBJECT_TYPE_INTERNAL GetScenesObjectTypeInternal<SubmeshComponent>();
template<> extern SCENES_OBJECT_TYPE_INTERNAL GetScenesObjectTypeInternal<MeshComponent>();


}// namespace scne
}// namespace buma

#include "ScenesImpl.h"
#include "ScenesObject.h"

#include "Scene.h"
#include "Components/Node.h"
#include "Components/ValueComponent.h"
#include "Components/TransformComponent.h"
#include "Components/AABBComponent.h"
#include "Components/CameraComponent.h"
#include "Components/LightPropertyComponent.h"
#include "Components/LightComponent.h"
#include "Components/TextureComponent.h"
#include "Components/SamplerComponent.h"
#include "Components/MaterialPropertyComponent.h"
#include "Components/MaterialComponent.h"
#include "Components/BufferComponent.h"
#include "Components/BufferViewComponent.h"
#include "Components/IndexLayoutComponent.h"
#include "Components/VertexElementComponent.h"
#include "Components/VertexLayoutComponent.h"
#include "Components/IndexBufferComponent.h"
#include "Components/VertexBufferComponent.h"
#include "Components/SubmeshComponent.h"
#include "Components/MeshComponent.h"

/* Loader */
#include "ScenesLoader.h"
#include "Loader/ScenesLoaderGltf.h"


#endif //PCH_H
