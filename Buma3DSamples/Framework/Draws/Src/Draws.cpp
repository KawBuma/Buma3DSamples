#include "pch.h"
#include "Draws.h"

namespace buma
{
namespace draws
{

IDrawsInstance* CreateDrawsInstance()
{
    IDrawsInstance* p{};
    DrawsInstance::Create(&p);
    return p;
}

void DestroyDrawsInstance(IDrawsInstance* _instance)
{
    DrawsInstance::Destroy(_instance);
}

}
}
