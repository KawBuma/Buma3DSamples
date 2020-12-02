#pragma once
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>


namespace buma
{

class HelloConstantBuffer : public ApplicationBase
{
public:
    static constexpr uint32_t BACK_BUFFER_COUNT = 3;

public:
    HelloConstantBuffer();
    virtual ~HelloConstantBuffer();

    static HelloConstantBuffer* Create();

    bool Prepare(PlatformBase& _platform) override;
    bool PrepareSwapChain();
    void PrepareSubmitInfo();
    void CreateEvents();

    bool Init() override;
    bool LoadAssets();
    bool CreateRootSignature();
    bool CreateDescriptorPool();
    bool AllocateDescriptorSets();
    bool CreateRenderPass();
    bool CreateFramebuffer();
    bool CreateShaderModules();
    bool CreateGraphicsPipelines();
    bool CreateCommandAllocator();
    bool CreateCommandLists();
    bool CreateFences();
    bool CreateBuffers();
    bool CreateHeaps(buma3d::RESOURCE_HEAP_ALLOCATION_INFO* _heap_alloc_info, std::vector<buma3d::RESOURCE_ALLOCATION_INFO>* _alloc_infos);
    bool BindResourceHeaps(buma3d::RESOURCE_HEAP_ALLOCATION_INFO* _heap_alloc_info, std::vector<buma3d::RESOURCE_ALLOCATION_INFO>* _alloc_infos);
    bool CreateBuffersForCopy();
    bool CopyBuffers();
    bool CreateBufferViews();
    bool CreateConstantBuffer();
    bool CreateConstantBufferView();
    bool WriteDescriptorSet();

    void Tick() override;
    void Update();
    void Render();

    void MoveToNextFrame();

    void OnResize(ResizeEventArgs* _args);
    void OnResized(BufferResizedEventArgs* _args);

    void Term() override;

private:
    void PrepareFrame(uint32_t buffer_index);

private:
    struct FENCE_VALUES {
        FENCE_VALUES& operator++()    { ++wait; ++signal; return *this; } 
        FENCE_VALUES  operator++(int) { auto tmp = *this; wait++; signal++; return tmp; }
        uint64_t wait   = 0;
        uint64_t signal = 1;
    };
    enum SCF { PRESENT_COMPLETE, RENDER_COMPLETE, SWAPCHAIN_FENCE_NUM };

private:
    struct VERTEX {
        buma3d::FLOAT4 position;
        buma3d::FLOAT4 color;
    };
    std::vector<VERTEX> triangle;
    std::vector<uint16_t> index;

    struct CB_MODEL // register(b0, space0);
    {
        glm::mat4 model;
    };
    struct CB_SCENE // register(b0, space1);
    {
        glm::mat4 view_proj;
    };
    CB_MODEL cb_model;
    CB_SCENE cb_scene;

    struct FRAME_CB
    {
        void*                                                   mapped_data[2/*scene,model*/];
        buma3d::util::Ptr<buma3d::IBuffer>                      constant_buffer;
        buma3d::util::Ptr<buma3d::IConstantBufferView>          scene_cbv;
        buma3d::util::Ptr<buma3d::IConstantBufferView>          model_cbv;
    };

    class ResizeEvent;
    class BufferResizedEvent;

private:
    PlatformBase*                                               platform;
    std::shared_ptr<WindowBase>                                 spwindow;
    WindowBase*                                                 window;
    buma3d::util::Ptr<buma3d::IDevice>                          device;
    buma3d::util::Ptr<buma3d::ICommandQueue>                    command_queue;
    StepTimer                                                   timer;

    std::shared_ptr<buma::SwapChain>                            swapchain;
    const std::vector<buma::SwapChain::SWAP_CHAIN_BUFFER>*      back_buffers;
    uint32_t                                                    back_buffer_index;
    const SwapChain::PRESENT_COMPLETE_FENCES*                   swapchain_fences;

    buma3d::VIEWPORT                                            vpiewport;
    buma3d::SCISSOR_RECT                                        scissor_rect;
    std::vector<buma3d::util::Ptr<buma3d::IFramebuffer>>        framebuffers;

    std::vector<buma3d::util::Ptr<buma3d::IShaderModule>>       shader_modules;
    buma3d::util::Ptr<buma3d::IPipelineState>                   pipeline;
    std::vector<buma3d::util::Ptr<buma3d::ICommandAllocator>>   cmd_allocator;
    std::vector<buma3d::util::Ptr<buma3d::ICommandList>>        cmd_lists;

    buma3d::util::Ptr<buma3d::IFence>                           util_fence;
    FENCE_VALUES                                                fence_values[BACK_BUFFER_COUNT];
    std::vector<buma3d::util::Ptr<buma3d::IFence>>              cmd_fences;
    buma3d::util::Ptr<buma3d::IFence>                           render_complete_fence;

    buma3d::util::Ptr<buma3d::IRootSignature>                   signature;
    buma3d::util::Ptr<buma3d::IDescriptorPool>                  descriptor_pool;
    std::vector<buma3d::util::Ptr<buma3d::IDescriptorSet>>      descriptor_sets;

    buma3d::util::Ptr<buma3d::IRenderPass>                      render_pass;
    buma3d::util::Ptr<buma3d::IResourceHeap>                    resource_heap;
    buma3d::util::Ptr<buma3d::IBuffer>                          vertex_buffer;
    buma3d::util::Ptr<buma3d::IBuffer>                          index_buffer;
    buma3d::util::Ptr<buma3d::IBuffer>                          vertex_buffer_src;
    buma3d::util::Ptr<buma3d::IBuffer>                          index_buffer_src;

    buma3d::util::Ptr<buma3d::IVertexBufferView>                vertex_buffer_view;
    buma3d::util::Ptr<buma3d::IIndexBufferView>                 index_buffer_view;

    RESOURCE_HEAP_ALLOCATION                                    cb_heap;
    FRAME_CB                                                    frame_cbs[BACK_BUFFER_COUNT];

    buma::util::FenceSubmitDesc                                 signal_fence_desc;
    buma::util::FenceSubmitDesc                                 wait_fence_desc;

    buma3d::SUBMIT_INFO                                         submit_info;
    buma3d::SUBMIT_DESC                                         submit;
    buma3d::SWAP_CHAIN_PRESENT_INFO                             present_info;
    buma3d::SCISSOR_RECT                                        present_region;

    std::shared_ptr<ResizeEvent>                                on_resize;
    std::shared_ptr<BufferResizedEvent>                         on_resized;

    DeferredContext ctx{};

};

/*
* Basic camera class
*
* Copyright (C) 2016 by Sascha Willems - www.saschawillems.de
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/

class Camera
{
private:
    float fov;
    float znear, zfar;

    void updateViewMatrix()
    {
        glm::mat4 rotM = glm::mat4(1.0f);
        glm::mat4 transM;

        rotM = glm::rotate(rotM, glm::radians(rotation.x * (flipY ? -1.0f : 1.0f)), glm::vec3(1.0f, 0.0f, 0.0f));
        rotM = glm::rotate(rotM, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        rotM = glm::rotate(rotM, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

        glm::vec3 translation = position;
        if (flipY)
        {
            translation.y *= -1.0f;
        }
        transM = glm::translate(glm::mat4(1.0f), translation);

        if (type == CameraType::firstperson)
        {
            matrices.view = rotM * transM;
        }
        else
        {
            matrices.view = transM * rotM;
        }

        viewPos = glm::vec4(position, 0.0f) * glm::vec4(-1.0f, 1.0f, -1.0f, 1.0f);

        updated = true;
    }
public:
    enum CameraType { lookat, firstperson };
    CameraType type = CameraType::lookat;

    glm::vec3 rotation = glm::vec3();
    glm::vec3 position = glm::vec3();
    glm::vec4 viewPos = glm::vec4();

    float rotationSpeed = 1.0f;
    float movementSpeed = 1.0f;

    bool dirty = false;
    bool updated = false;
    bool flipY = false;

    struct
    {
        glm::mat4 perspective;
        glm::mat4 view;
    } matrices;

    struct
    {
        bool left = false;
        bool right = false;
        bool up = false;
        bool down = false;
    } keys;

    bool moving()
    {
        return keys.left || keys.right || keys.up || keys.down;
    }

    float getNearClip()
    {
        return znear;
    }

    float getFarClip()
    {
        return zfar;
    }

    void setPerspective(float fov, float aspect, float znear, float zfar)
    {
        this->fov = fov;
        this->znear = znear;
        this->zfar = zfar;
        matrices.perspective = glm::perspective(glm::radians(fov), aspect, znear, zfar);
        if (flipY)
        {
            matrices.perspective[1, 1] *= -1.0f;
        }
        dirty = true;
    };

    void updateAspectRatio(float aspect)
    {
        matrices.perspective = glm::perspective(glm::radians(fov), aspect, znear, zfar);
        if (flipY)
        {
            matrices.perspective[1, 1] *= -1.0f;
        }
        dirty = true;
    }

    void setPosition(glm::vec3 position)
    {
        this->position = position;
        dirty = true;
        updateViewMatrix();
    }

    void setRotation(glm::vec3 rotation)
    {
        this->rotation = rotation;
        dirty = true;
        updateViewMatrix();
    }

    void rotate(glm::vec3 delta)
    {
        this->rotation += delta;
        dirty = true;
        updateViewMatrix();
    }

    void setTranslation(glm::vec3 translation)
    {
        this->position = translation;
        dirty = true;
        updateViewMatrix();
    };

    void translate(glm::vec3 delta)
    {
        this->position += delta;
        dirty = true;
        updateViewMatrix();
    }

    void setRotationSpeed(float rotationSpeed)
    {
        this->rotationSpeed = rotationSpeed;
    }

    void setMovementSpeed(float movementSpeed)
    {
        this->movementSpeed = movementSpeed;
    }

    void update(float deltaTime)
    {
        updated = false;
        if (type == CameraType::firstperson)
        {
            if (moving())
            {
                glm::vec3 camFront;
                camFront.x = -cos(glm::radians(rotation.x)) * sin(glm::radians(rotation.y));
                camFront.y = sin(glm::radians(rotation.x));
                camFront.z = cos(glm::radians(rotation.x)) * cos(glm::radians(rotation.y));
                camFront = glm::normalize(camFront);

                float moveSpeed = deltaTime * movementSpeed;

                if (keys.up)
                    position += camFront * moveSpeed;
                if (keys.down)
                    position -= camFront * moveSpeed;
                if (keys.left)
                    position -= glm::normalize(glm::cross(camFront, glm::vec3(0.0f, 1.0f, 0.0f))) * moveSpeed;
                if (keys.right)
                    position += glm::normalize(glm::cross(camFront, glm::vec3(0.0f, 1.0f, 0.0f))) * moveSpeed;

                updateViewMatrix();
            }
        }
        dirty = false;
    };

    // Update camera passing separate axis data (gamepad)
    // Returns true if view or position has been changed
    bool updatePad(glm::vec2 axisLeft, glm::vec2 axisRight, float deltaTime)
    {
        bool retVal = false;

        if (type == CameraType::firstperson)
        {
            // Use the common console thumbstick layout		
            // Left = view, right = move

            const float deadZone = 0.0015f;
            const float range = 1.0f - deadZone;

            glm::vec3 camFront;
            camFront.x = -cos(glm::radians(rotation.x)) * sin(glm::radians(rotation.y));
            camFront.y = sin(glm::radians(rotation.x));
            camFront.z = cos(glm::radians(rotation.x)) * cos(glm::radians(rotation.y));
            camFront = glm::normalize(camFront);

            float moveSpeed = deltaTime * movementSpeed * 2.0f;
            float rotSpeed = deltaTime * rotationSpeed * 50.0f;

            // Move
            if (fabsf(axisLeft.y) > deadZone)
            {
                float pos = (fabsf(axisLeft.y) - deadZone) / range;
                position -= camFront * pos * ((axisLeft.y < 0.0f) ? -1.0f : 1.0f) * moveSpeed;
                retVal = true;
            }
            if (fabsf(axisLeft.x) > deadZone)
            {
                float pos = (fabsf(axisLeft.x) - deadZone) / range;
                position += glm::normalize(glm::cross(camFront, glm::vec3(0.0f, 1.0f, 0.0f))) * pos * ((axisLeft.x < 0.0f) ? -1.0f : 1.0f) * moveSpeed;
                retVal = true;
            }

            // Rotate
            if (fabsf(axisRight.x) > deadZone)
            {
                float pos = (fabsf(axisRight.x) - deadZone) / range;
                rotation.y += pos * ((axisRight.x < 0.0f) ? -1.0f : 1.0f) * rotSpeed;
                retVal = true;
            }
            if (fabsf(axisRight.y) > deadZone)
            {
                float pos = (fabsf(axisRight.y) - deadZone) / range;
                rotation.x -= pos * ((axisRight.y < 0.0f) ? -1.0f : 1.0f) * rotSpeed;
                retVal = true;
            }
        }
        else
        {
            // todo: move code from example base class for look-at
        }

        if (retVal)
        {
            updateViewMatrix();
        }

        return retVal;
    }

};


}// namespace buma
