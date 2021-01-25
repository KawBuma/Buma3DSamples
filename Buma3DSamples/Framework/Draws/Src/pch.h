#ifndef PCH_H
#define PCH_H

#include "Draws.h"

#include <new>
#include <atomic>
#include <mutex>
#include <memory>
#include <vector>
#include <map>
#include <set>
#include <array>
#include <unordered_map>
#include <unordered_set>

// From Framework
#include "Utils.h"
#include "FormatUtils.h"
#include "B3DInit.h"
#include "DeviceResources.h"
#include "VariableSizeAllocationsManager.h"
#include "ResourceHeapProperties.h"
#include "ResourceHeapAllocator.h"
#include "Resource.h"
#include "ResourceBuffer.h"
#include "ResourceTexture.h"
#include "ResourceCreate.h"
#include "ShaderLoader.h"
#include "StagingBufferPool.h"
#include "DeferredContext.h"
#include "ImmediateContext.h"


namespace buma
{
namespace draws
{

class DrawsInstance;

class DrawsResource;
class DrawsSampler;
class DrawsTexture;
class DrawsBuffer;
class DrawsVertexBuffer;
class DrawsIndexBuffer;

class DrawsMaterialParameters;
class DrawsMaterial;
class DrawsMaterialConstant;
class DrawsMaterialInstance;
class DrawsMeshData;
class DrawsMesh;
class DrawsStaticMesh;

class DrawsScene;
class DrawsSceneComponent;
class DrawsNodeComponent;
class DrawsPrimitiveComponent;
class DrawsMeshComponent;
class DrawsStaticMeshComponent;
class DrawsLightComponent;

class DrawsCamera;
class DrawsView;
class DrawsRenderer;

}// namespace buma
}// namespace draws


#include "DrawsInstance.h"

#include "DrawsSampler.h"
#include "DrawsTexture.h"
#include "DrawsVertexBuffer.h"
#include "DrawsIndexBuffer.h"

//#include "DrawsMaterialParameters.h"
//#include "DrawsMaterial.h"
//#include "DrawsMaterialConstant.h"
//#include "DrawsMaterialInstance.h"
//#include "DrawsMeshData.h"
//#include "DrawsMesh.h"
//#include "DrawsStaticMesh.h"
//
//#include "DrawsScene.h"
//#include "DrawsNodeComponent.h"
//#include "DrawsPrimitiveComponent.h"
//#include "DrawsMeshComponent.h"
//#include "DrawsStaticMeshComponent.h"
//#include "DrawsLightComponent.h"
//
//#include "DrawsCamera.h"
//#include "DrawsView.h"
//#include "DrawsRenderer.h"


#endif //PCH_H
