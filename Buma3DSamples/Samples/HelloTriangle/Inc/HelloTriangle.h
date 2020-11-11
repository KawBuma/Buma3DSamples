#pragma once

namespace buma
{

class HelloTriangle : public ApplicationBase
{
public:
    HelloTriangle();
    virtual ~HelloTriangle();

    static HelloTriangle* Create();

    bool Prepare(PlatformBase& _platform) override;

    bool Init() override;
    virtual void LoadAssets();

    void Tick() override;
    virtual void Update();
    virtual void Render();

    void Term() override;

private:
    PlatformBase*                                           platform;
    buma3d::util::Ptr<buma3d::IDevice>                      device;

    buma3d::SURFACE_FORMAT                                  sfs_format;
    std::shared_ptr<buma::SwapChain>                        swapchain;
    const std::vector<buma::SwapChain::SWAP_CHAIN_BUFFER>*  back_buffers;

    Ptr<b::IRootSignature> signature;


};


}// namespace buma
