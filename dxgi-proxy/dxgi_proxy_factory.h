#pragma once

namespace dxgi_proxy {

class Factory final : public Object<IDXGIFactory7>
{
public:
	Factory(const Config &config, ComPtr<IUnknown> original);

	// IUnknown methods
	HRESULT DXGI_P_API QueryInterface(REFIID riid, void **out) override;  // hijacked

	// IDXGIObject methods
	HRESULT DXGI_P_API GetParent(REFIID riid, void **out) override;
	HRESULT DXGI_P_API GetPrivateData(REFGUID riid, UINT *size, void *data) override;
	HRESULT DXGI_P_API SetPrivateData(REFGUID riid, UINT size, const void *data) override;
	HRESULT DXGI_P_API SetPrivateDataInterface(REFGUID riid, const IUnknown *obj) override;

	// IDXGIFactory methods
	HRESULT DXGI_P_API EnumAdapters(UINT adapter, IDXGIAdapter **out) override;  // hijacked
	HRESULT DXGI_P_API MakeWindowAssociation(HWND hwnd, UINT flags) override;
	HRESULT DXGI_P_API GetWindowAssociation(HWND *out) override;
	HRESULT DXGI_P_API CreateSwapChain(IUnknown *device, DXGI_SWAP_CHAIN_DESC *desc, IDXGISwapChain **out) override;
	HRESULT DXGI_P_API CreateSoftwareAdapter(HMODULE m, IDXGIAdapter **out) override;

	// IDXGIFactory1 methods
	HRESULT DXGI_P_API EnumAdapters1(UINT adapter, IDXGIAdapter1 **out) override;  // hijacked
	BOOL DXGI_P_API IsCurrent() override;

	// IDXGIFactory2 methods
	BOOL DXGI_P_API IsWindowedStereoEnabled() override;
	HRESULT DXGI_P_API GetSharedResourceAdapterLuid(HANDLE resource, LUID *out) override;
	HRESULT DXGI_P_API RegisterStereoStatusWindow(HWND hwnd, UINT msg, DWORD *cookie) override;
	HRESULT DXGI_P_API RegisterStereoStatusEvent(HANDLE event, DWORD *cookie) override;
	void DXGI_P_API UnregisterStereoStatus(DWORD cookie) override;
	HRESULT DXGI_P_API RegisterOcclusionStatusWindow(HWND hwnd, UINT msg, DWORD *cookie) override;
	HRESULT DXGI_P_API RegisterOcclusionStatusEvent(HANDLE event, DWORD *cookie) override;
	void DXGI_P_API UnregisterOcclusionStatus(DWORD cookie) override;

	HRESULT DXGI_P_API CreateSwapChainForHwnd(IUnknown *device,
	                                          HWND hwnd,
	                                          const DXGI_SWAP_CHAIN_DESC1 *desc,
	                                          const DXGI_SWAP_CHAIN_FULLSCREEN_DESC *fdesc,
	                                          IDXGIOutput *restrict_out,
	                                          IDXGISwapChain1 **out) override;

	HRESULT DXGI_P_API CreateSwapChainForCoreWindow(IUnknown *device,
	                                                IUnknown *window,
	                                                const DXGI_SWAP_CHAIN_DESC1 *desc,
	                                                IDXGIOutput *restrict_out,
	                                                IDXGISwapChain1 **out) override;

	HRESULT DXGI_P_API CreateSwapChainForComposition(IUnknown *device,
	                                                 const DXGI_SWAP_CHAIN_DESC1 *desc,
	                                                 IDXGIOutput *restrict_out,
	                                                 IDXGISwapChain1 **out) override;

	// IDXGIFactory3 methods
	UINT DXGI_P_API GetCreationFlags() override;

	// IDXGIFactory4 methods
	HRESULT DXGI_P_API EnumAdapterByLuid(LUID luid, REFIID riid, void **out) override;  // hijacked
	HRESULT DXGI_P_API EnumWarpAdapter(REFIID riid, void **out) override;

	// IDXGIFactory5 methods
	HRESULT DXGI_P_API CheckFeatureSupport(DXGI_FEATURE feature, void *data, UINT size) override;

	// IDXGIFactory6 methods
	HRESULT DXGI_P_API EnumAdapterByGpuPreference(UINT adapter,
	                                              DXGI_GPU_PREFERENCE preference,
	                                              REFIID riid,
	                                              void **out) override;  // hijacked

	// IDXGIFactory7 methods
	HRESULT DXGI_P_API RegisterAdaptersChangedEvent(HANDLE event, DWORD *cookie) override;
	HRESULT DXGI_P_API UnregisterAdaptersChangedEvent(DWORD cookie) override;

private:
	using FactoryChain = ComObjectChain<IUnknown,
	                                    IDXGIObject,
	                                    IDXGIFactory,
	                                    IDXGIFactory1,
	                                    IDXGIFactory2,
	                                    IDXGIFactory3,
	                                    IDXGIFactory4,
	                                    IDXGIFactory5,
	                                    IDXGIFactory6,
	                                    IDXGIFactory7>;

	const Config *config{nullptr};
	const FactoryChain chain;

	template<typename T>
	[[nodiscard]] T &get() const noexcept;
	[[nodiscard]] HRESULT wrapAdapter(REFIID riid, void **in_out);
};

}  // namespace dxgi_proxy
