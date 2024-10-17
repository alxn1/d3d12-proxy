#pragma once

namespace dxgi_proxy {

class Adapter final : public Object<IDXGIAdapter4>
{
public:
	[[nodiscard]] static HRESULT wrap(const Config &config, ComPtr<IUnknown> parent, REFIID riid, void **in_out);

	Adapter(const Config &config, ComPtr<IUnknown> parent, ComPtr<IUnknown> unknown);

	// IUnknown methods
	HRESULT DXGI_P_API QueryInterface(REFIID riid, void **out) override;  // hijacked

	// IDXGIObject methods
	HRESULT DXGI_P_API GetParent(REFIID riid, void **out) override;
	HRESULT DXGI_P_API GetPrivateData(REFGUID riid, UINT *size, void *data) override;
	HRESULT DXGI_P_API SetPrivateData(REFGUID riid, UINT size, const void *data) override;
	HRESULT DXGI_P_API SetPrivateDataInterface(REFGUID riid, const IUnknown *obj) override;

	// IDXGIAdapter methods
	HRESULT DXGI_P_API EnumOutputs(UINT output, IDXGIOutput **out) override;
	HRESULT DXGI_P_API GetDesc(DXGI_ADAPTER_DESC *out) override;  // hijacked
	HRESULT DXGI_P_API CheckInterfaceSupport(REFGUID name, LARGE_INTEGER *out) override;

	// IDXGIAdapter1 methods
	HRESULT DXGI_P_API GetDesc1(DXGI_ADAPTER_DESC1 *out) override;  // hijacked

	// IDXGIAdapter2 methods
	HRESULT DXGI_P_API GetDesc2(DXGI_ADAPTER_DESC2 *out) override;  // hijacked

	// IDXGIAdapter3 methods
	HRESULT DXGI_P_API RegisterHardwareContentProtectionTeardownStatusEvent(HANDLE event, DWORD *cookie) override;
	void DXGI_P_API UnregisterHardwareContentProtectionTeardownStatus(DWORD cookie) override;
	HRESULT DXGI_P_API QueryVideoMemoryInfo(UINT index,
	                                        DXGI_MEMORY_SEGMENT_GROUP group,
	                                        DXGI_QUERY_VIDEO_MEMORY_INFO *out) override;  // hijacked

	HRESULT DXGI_P_API SetVideoMemoryReservation(UINT index, DXGI_MEMORY_SEGMENT_GROUP group, UINT64 reservation) override;
	HRESULT DXGI_P_API RegisterVideoMemoryBudgetChangeNotificationEvent(HANDLE event, DWORD *cookie) override;
	void DXGI_P_API UnregisterVideoMemoryBudgetChangeNotification(DWORD cookie) override;

	// IDXGIAdapter4 methods
	HRESULT DXGI_P_API GetDesc3(DXGI_ADAPTER_DESC3 *out) override;  // hijacked

private:
	using AdapterChain = ComObjectChain<IUnknown, IDXGIObject, IDXGIAdapter, IDXGIAdapter1, IDXGIAdapter2, IDXGIAdapter3, IDXGIAdapter4>;
	using Timepoint = std::chrono::steady_clock::time_point;
	using Clock = std::chrono::steady_clock;

	const Config *config{nullptr};
	const ComPtr<IUnknown> parent;
	const AdapterChain chain;
	const Timepoint created_at{Clock::now()};
	bool overrides_disabled{true};

	template<typename T>
	[[nodiscard]] T &get() const noexcept;
	[[nodiscard]] bool isIntegrated() const noexcept;

	template<typename T>
	void postprocess(T &info) const;
	void postprocess(DXGI_MEMORY_SEGMENT_GROUP group, DXGI_QUERY_VIDEO_MEMORY_INFO &info) const;

	void checkTimeout();
};

}  // namespace dxgi_proxy
