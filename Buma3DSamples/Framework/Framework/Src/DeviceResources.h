#pragma once
#include <MyImgui.h>

// Provides an interface for an application that owns DeviceResources to be notified of the device being lost or created.
interface IDeviceNotify
{
	virtual void OnDeviceLost() = 0;
	virtual void OnDeviceRestored() = 0;
};


// Controls all the DirectX device resources.
class DeviceResources
{
	friend class Framework;
public:
	static const unsigned int allow_tearing = 0x1;
	static const unsigned int enable_hdr	= 0x2;

	static const size_t MAX_BACK_BUFFER_COUNT = 3;

	DeviceResources
	(
		 const DXGI_FORMAT&			_back_buffer_format_on_display	= DXGI_FORMAT_R10G10B10A2_UNORM
		,const DXGI_FORMAT&			_back_buffer_format_on_render	= DXGI_FORMAT_R11G11B10_FLOAT	
		,const DXGI_FORMAT&			_depth_buffer_format	= DXGI_FORMAT_D32_FLOAT
		,const UINT&				_back_buffer_count		= 2
		,const D3D_FEATURE_LEVEL&	_min_feature_level		= D3D_FEATURE_LEVEL_11_0
		,const unsigned int&		_flags					= 0
	) noexcept(false);
	~DeviceResources();

	void Flush()
	{
		direct_command_queue->Flush();
		compute_command_queue->Flush();
		copy_command_queue->Flush();
	}

	// Configures the Direct3D device, and stores handles to it and the device context.
	void Init();
	/* Init procedure */
	void CheckAdapter();
	void CreateDevice();
	void CreateMyImGui();

	// These resources need to be recreated every time the window size is changed.
	void CreateWindowSizeDependentResources();
	void ResetSwapChainAndCreateWindowSizeDependentResources(bool _is_allow_tearing);

	// This method is called when the Win32 window is created (or re-created).
	void SetWindow(HWND _hwnd, const int& _width, const int& _height);

	// This method is called when the Win32 window changes size.
	bool WindowSizeChanged(const int& _width, const int& _height);

	// Recreate all device resources and set them back to the current state.
	void HandleDeviceLost();
	void TraverseDREDAutoBredCrumbNodes(const D3D12_AUTO_BREADCRUMB_NODE* _node, const UINT _node_depth);
	void TraverseDREDAllocationNode(const D3D12_DRED_ALLOCATION_NODE* _node, const UINT _node_depth);
	void TraceBackDeviceRemovedExtendedData();

	// Set divece notify
	void RegisterDeviceNotify(IDeviceNotify* _device_notify) { device_notify = _device_notify; }

	// Present the contents of the swap chain to the screen.
	void Present(const buma::d3d::res::Texture& _tex = buma::d3d::res::Texture());

	// Wait for pending GPU work to complete.
	void WaitForGpu();

	// Device Accessors.

	const RECT& GetOutputSize() const { return rect_output_size; }

	// Direct3D Accessors.
	ID3D12Device*								GetDevice()							const { return device.Get();								}
	IDXGISwapChain3*							GetSwapChain()						const { return swap_chain.Get();							}
	IDXGIFactory4*								GetDXGIFactory()					const { return dxgi_factory.Get();							}
	const D3D_FEATURE_LEVEL&					GetDeviceFeatureLevel()				const { return feature_level;								}
	const D3D12_FEATURE_DATA_ROOT_SIGNATURE&	GetDeviceFeatureDataRootSignature() const { return feature_data_root_signature;					}
	const DXGI_FORMAT&							GetBackBufferFormatOnDisplay()		const { return back_buffer_format_on_display;				}
	const DXGI_FORMAT&							GetBackBufferFormatOnRender()		const { return back_buffer_format_on_render;				}
	const DXGI_FORMAT&							GetDepthBufferFormat()				const { return depth_buffer_format;							}
	const D3D12_VIEWPORT&						GetScreenViewport()					const { return screen_viewport;								}
	const D3D12_RECT&							GetScissorRect()					const { return scissor_rect;								}
	std::tuple<D3D12_VIEWPORT, D3D12_RECT>		GetViewportAndScissorRect()			const { return { screen_viewport,scissor_rect };			}
	const UINT&									GetCurrentFrameIndex()				const { return back_buffer_index;							}
	const UINT&									GetBackBufferCount()				const { return back_buffer_count;							}
	const DXGI_COLOR_SPACE_TYPE&				GetColorSpace()						const { return dxgi_color_space_type;						}
	const unsigned int&							GetDeviceOptions()					const { return option_flags;								}

	std::shared_ptr<buma::d3d::CommandQueue>	GetCommandQueue(const D3D12_COMMAND_LIST_TYPE _type) const;
	std::shared_ptr<buma::d3d::CommandQueue>	GetDirecteCommandQueue()	const	{ return direct_command_queue; }
	std::shared_ptr<buma::d3d::CommandQueue>	GetComputeCommandQueue()	const	{ return compute_command_queue; }
	std::shared_ptr<buma::d3d::CommandQueue>	GetCopyCommandQueue()		const	{ return copy_command_queue; }
	std::shared_ptr<buma::d3d::GpuTimerPool>	GetGpuTimerPool(const D3D12_COMMAND_LIST_TYPE _type) const;
	std::shared_ptr<buma::d3d::GpuTimerPool>	GetDirectGpuTimerPool()		const	{ return direct_gpu_timer_pool; }
	std::shared_ptr<buma::d3d::GpuTimerPool>	GetComputeGpuTimerPool()	const	{ return compute_gpu_timer_pool; }

	std::shared_ptr<buma::MyImGui>				GetMyImGui()				const	{ return my_imugi; }
	void										NewFrameMyImGui()					{ my_imugi->NewFrame(); }
	void										BeginReadbackGpuTimerPool();
	void										EndReadbackGpuTimerPool();

	D3D12MA::Allocator*							GetMemAllocator()					{ return mem_allocator; }
	void										PushStaleMemAllocation(std::shared_ptr<D3D12MA::Allocation*> _alloc);
	void										ReleaseStaleMemAllocationAll();
	void										ReleaseStaleMemAllocationTemporal();

	std::shared_ptr<buma::d3d::pipeline::PipelineStateObjectManager> GetPipelineStateObjectManager() const { return mgr_pipeline; }

	CD3DX12_CPU_DESCRIPTOR_HANDLE					GetRenderTargetView() const;
	const buma::d3d::res::RenderTargets&			GetRenderTarget();
	const buma::d3d::res::Texture&					GetCurrentBackBuffer() { return back_buffers[back_buffer_index]; }
	std::shared_ptr<buma::d3d::DescriptorAllocator> GetDescriptorAllocator(const D3D12_DESCRIPTOR_HEAP_TYPE& _heap_type);

	bool										IsAllowTearing() const						{ return option_flags & allow_tearing; 												  }
	void										SetIsAllowTearing(bool _is_allow_tearing)	{ _is_allow_tearing ? option_flags |= allow_tearing : option_flags &= !allow_tearing; }
	void										SwitchIsAllowTearing()						{ option_flags ^= allow_tearing; 													  }

	void 										SetCurrentFrameValue(uint64_t _value)		{ frame_values[back_buffer_index] = _value; }
	void 										ResetFrameValues()							{ for (auto& i : frame_values) i = 0; 		}
	uint64_t									GetCurrentFrameValue()	const				{ return frame_values[back_buffer_index]; 	}
	UINT64 										GetCurrentFenceValue()	const				{ return fence_values[back_buffer_index]; 	}
	const uint64_t*								GetFrameValues()		const				{ return frame_values; 						}
	const UINT64*								GetFenceValues()		const				{ return fence_values; 						}
	float										GetAspectRatio()		const				{ return aspect_ratio; 						}
	float										GetAspectRatioYX()		const				{ return screen_viewport.Height / screen_viewport.Width; }

	HWND GetHWND() const { return hwnd; }

private:
	// Prepare to render the next frame.
	void MoveToNextFrame();
	void WaitForCurrentBackBufferFenceValue();
	// This method acquires the first available hardware adapter that supports Direct3D 12.
	// If no such adapter can be found, try WARP. Otherwise throw an exception.
	void GetAdapter(IDXGIAdapter1** _ppadapter);
	// Sets the color space for the swap chain in order to handle HDR output.
	void UpdateColorSpaceType();


	UINT												back_buffer_index;

	// Direct3D objects.
	Microsoft::WRL::ComPtr<ID3D12Device>					device;

	// Swap chain objects.
	Microsoft::WRL::ComPtr<IDXGIFactory4>				dxgi_factory;
	Microsoft::WRL::ComPtr<IDXGIAdapter4>				dxgi_adapter;
	Microsoft::WRL::ComPtr<IDXGISwapChain3>				swap_chain;

	// Presentation fence objects.
	UINT64												fence_values[MAX_BACK_BUFFER_COUNT];
	uint64_t											frame_values[MAX_BACK_BUFFER_COUNT];

	// Direct3D rendering objects.
	UINT												rtv_descriptor_size;
	D3D12_VIEWPORT										screen_viewport;
	D3D12_RECT											scissor_rect;
	float												aspect_ratio;

	// Direct3D properties.
	DXGI_FORMAT											back_buffer_format_on_display;
	DXGI_FORMAT											back_buffer_format_on_render;
	DXGI_FORMAT											depth_buffer_format;
	UINT												back_buffer_count;
	D3D_FEATURE_LEVEL									d3d_min_feature_level;
	D3D12_FEATURE_DATA_ROOT_SIGNATURE					feature_data_root_signature;

	// Cached device properties.
	HWND													hwnd;
	D3D_FEATURE_LEVEL									feature_level;
	DWORD												dxgi_factory_flags;
	RECT												rect_output_size;

	// HDR Support
	DXGI_COLOR_SPACE_TYPE								dxgi_color_space_type;

	// DeviceResources options (see flags above)
	unsigned int										option_flags;

	// The IDeviceNotify can be held directly as it owns the DeviceResources.
	IDeviceNotify*										device_notify;

	std::shared_ptr<buma::d3d::GpuTimerPool>			direct_gpu_timer_pool;
	std::shared_ptr<buma::d3d::GpuTimerPool>			compute_gpu_timer_pool;

	std::shared_ptr<buma::d3d::CommandQueue>			direct_command_queue;
	std::shared_ptr<buma::d3d::CommandQueue>			compute_command_queue;
	std::shared_ptr<buma::d3d::CommandQueue>			copy_command_queue;
	buma::d3d::res::Texture								back_buffers[MAX_BACK_BUFFER_COUNT];
	mutable buma::d3d::res::RenderTargets				render_target[MAX_BACK_BUFFER_COUNT];

	std::shared_ptr<buma::d3d::DescriptorAllocator>		descriptor_allocators[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];


	std::shared_ptr<buma::d3d::pipeline::PipelineStateObjectManager> mgr_pipeline;

	std::shared_ptr<buma::MyImGui>						my_imugi;

	D3D12MA::Allocator*									mem_allocator;
	D3D12MA::ALLOCATOR_DESC 							mem_alloc_desc;
	std::vector<std::shared_ptr<D3D12MA::Allocation*>>	stale_allocations;
	std::mutex											stale_allocations_mutex;
};
