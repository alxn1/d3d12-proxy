#include "pch.h"

DXGI_P_LOG_SYSTEM(ADAPTER);

namespace dxgi_proxy {
namespace {

class AdapterStub final : public Object<IDXGIAdapter4>
{
public:
	[[nodiscard]] static AdapterStub &instance()
	{
		static AdapterStub sInstance;
		return sInstance;
	}

	HRESULT DXGI_P_API QueryInterface(REFIID riid, void **out) override { return E_NOINTERFACE; }
	HRESULT DXGI_P_API GetParent(REFIID riid, void **out) override { return E_NOINTERFACE; }
	HRESULT DXGI_P_API GetPrivateData(REFGUID riid, UINT *size, void *data) override { return E_NOTIMPL; }
	HRESULT DXGI_P_API SetPrivateData(REFGUID riid, UINT size, const void *data) override { return E_NOTIMPL; }
	HRESULT DXGI_P_API SetPrivateDataInterface(REFGUID riid, const IUnknown *obj) override { return E_NOTIMPL; }
	HRESULT DXGI_P_API EnumOutputs(UINT output, IDXGIOutput **out) override { return E_NOTIMPL; }
	HRESULT DXGI_P_API GetDesc(DXGI_ADAPTER_DESC *out) override { return E_NOTIMPL; }
	HRESULT DXGI_P_API CheckInterfaceSupport(REFGUID name, LARGE_INTEGER *out) override { return E_NOTIMPL; }
	HRESULT DXGI_P_API GetDesc1(DXGI_ADAPTER_DESC1 *out) override { return E_NOTIMPL; }
	HRESULT DXGI_P_API GetDesc2(DXGI_ADAPTER_DESC2 *out) override { return E_NOTIMPL; }
	HRESULT DXGI_P_API RegisterHardwareContentProtectionTeardownStatusEvent(HANDLE event, DWORD *cookie) { return E_NOTIMPL; }
	void DXGI_P_API UnregisterHardwareContentProtectionTeardownStatus(DWORD cookie) {}
	HRESULT DXGI_P_API QueryVideoMemoryInfo(UINT index, DXGI_MEMORY_SEGMENT_GROUP group, DXGI_QUERY_VIDEO_MEMORY_INFO *out) override { return E_NOTIMPL; }
	HRESULT DXGI_P_API SetVideoMemoryReservation(UINT index, DXGI_MEMORY_SEGMENT_GROUP group, UINT64 reservation) { return E_NOTIMPL; }
	HRESULT DXGI_P_API RegisterVideoMemoryBudgetChangeNotificationEvent(HANDLE event, DWORD *cookie) { return E_NOTIMPL; }
	void DXGI_P_API UnregisterVideoMemoryBudgetChangeNotification(DWORD cookie) {}
	HRESULT DXGI_P_API GetDesc3(DXGI_ADAPTER_DESC3 *out) override { return E_NOTIMPL; }
};

}  // namespace

Adapter::Adapter(const Config &config, ComPtr<IUnknown> parent, ComPtr<IUnknown> original)
    : config{&config}
    , parent{std::move(parent)}
    , chain{std::move(original)}
    , overrides_disabled{config.overrideSection().only_igpu && !isIntegrated()}
{
	DXGI_P_LOG("factory is initialized:");
	chain.enumerate([](const auto *p) {
		DXGI_P_LOGF("\t%s: 0x%p", typeid(std::decay_t<decltype(*p)>).name(), p);
		return true;
	});
	if(overrides_disabled) {
		DXGI_P_LOG("\toverrides is disabled for dGPU adapter");
	}
}

// IUnknown methods

HRESULT DXGI_P_API Adapter::QueryInterface(REFIID riid, void **out)
{
	DXGI_P_LOGF("QueryInterface call with '%s' riid", log::format(riid).data());
	const auto hr = chain.proxiedQueryInterface(this, riid, out);
	if(hr == S_OK && *out == this) {
		DXGI_P_LOG("return pointer to self");
	}
	DXGI_P_LOGF("QueryInterface call result: 0x%X", hr);
	return hr;
}

// IDXGIObject methods

HRESULT DXGI_P_API Adapter::GetParent(REFIID riid, void **out)
{
	return parent ? parent->QueryInterface(riid, out) : E_NOTIMPL;
}

HRESULT DXGI_P_API Adapter::GetPrivateData(REFGUID riid, UINT *size, void *data)
{
	return get<IDXGIObject>().GetPrivateData(riid, size, data);
}

HRESULT DXGI_P_API Adapter::SetPrivateData(REFGUID riid, UINT size, const void *data)
{
	return get<IDXGIObject>().SetPrivateData(riid, size, data);
}

HRESULT DXGI_P_API Adapter::SetPrivateDataInterface(REFGUID riid, const IUnknown *obj)
{
	return get<IDXGIObject>().SetPrivateDataInterface(riid, obj);
}

// IDXGIAdapter methods

HRESULT DXGI_P_API Adapter::EnumOutputs(UINT output, IDXGIOutput **out)
{
	return get<IDXGIAdapter>().EnumOutputs(output, out);
}

HRESULT DXGI_P_API Adapter::GetDesc(DXGI_ADAPTER_DESC *out)
{
	DXGI_P_LOG("GetDesc call");
	const auto hr = get<IDXGIAdapter>().GetDesc(out);
	if(out && hr == S_OK) {
		checkTimeout();
		postprocess(*out);
	}
	DXGI_P_LOGF("GetDesc call result: 0x%X", hr);
	return hr;
}

HRESULT DXGI_P_API Adapter::CheckInterfaceSupport(REFGUID name, LARGE_INTEGER *out)
{
	return get<IDXGIAdapter>().CheckInterfaceSupport(name, out);
}

// IDXGIAdapter1 methods

HRESULT DXGI_P_API Adapter::GetDesc1(DXGI_ADAPTER_DESC1 *out)
{
	DXGI_P_LOG("GetDesc1 call");
	const auto hr = get<IDXGIAdapter1>().GetDesc1(out);
	if(out && hr == S_OK) {
		checkTimeout();
		postprocess(*out);
	}
	DXGI_P_LOGF("GetDesc1 call result: 0x%X", hr);
	return hr;
}

// IDXGIAdapter2 methods

HRESULT DXGI_P_API Adapter::GetDesc2(DXGI_ADAPTER_DESC2 *out)
{
	DXGI_P_LOG("GetDesc2 call");
	const auto hr = get<IDXGIAdapter2>().GetDesc2(out);
	if(out && hr == S_OK) {
		checkTimeout();
		postprocess(*out);
	}
	DXGI_P_LOGF("GetDesc2 call result: 0x%X", hr);
	return hr;
}

// IDXGIAdapter3 methods

HRESULT DXGI_P_API Adapter::RegisterHardwareContentProtectionTeardownStatusEvent(HANDLE event, DWORD *cookie)
{
	return get<IDXGIAdapter3>().RegisterHardwareContentProtectionTeardownStatusEvent(event, cookie);
}

void DXGI_P_API Adapter::UnregisterHardwareContentProtectionTeardownStatus(DWORD cookie)
{
	get<IDXGIAdapter3>().UnregisterHardwareContentProtectionTeardownStatus(cookie);
}

HRESULT DXGI_P_API Adapter::QueryVideoMemoryInfo(UINT index, DXGI_MEMORY_SEGMENT_GROUP group, DXGI_QUERY_VIDEO_MEMORY_INFO *out)
{
	DXGI_P_LOGF("QueryVideoMemoryInfo call with '%u' index and '%i' group", index, group);
	const auto hr = get<IDXGIAdapter3>().QueryVideoMemoryInfo(index, group, out);
	if(out && hr == S_OK && index == 0) {
		checkTimeout();
		postprocess(group, *out);
	}
	DXGI_P_LOGF("QueryVideoMemoryInfo call result: 0x%X", hr);
	return hr;
}

HRESULT DXGI_P_API Adapter::SetVideoMemoryReservation(UINT index, DXGI_MEMORY_SEGMENT_GROUP group, UINT64 reservation)
{
	return get<IDXGIAdapter3>().SetVideoMemoryReservation(index, group, reservation);
}

HRESULT DXGI_P_API Adapter::RegisterVideoMemoryBudgetChangeNotificationEvent(HANDLE event, DWORD *cookie)
{
	return get<IDXGIAdapter3>().RegisterVideoMemoryBudgetChangeNotificationEvent(event, cookie);
}

void DXGI_P_API Adapter::UnregisterVideoMemoryBudgetChangeNotification(DWORD cookie)
{
	get<IDXGIAdapter3>().UnregisterVideoMemoryBudgetChangeNotification(cookie);
}

// IDXGIAdapter4 methods

HRESULT DXGI_P_API Adapter::GetDesc3(DXGI_ADAPTER_DESC3 *out)
{
	DXGI_P_LOG("GetDesc3 call");
	const auto hr = get<IDXGIAdapter4>().GetDesc3(out);
	if(out && hr == S_OK) {
		checkTimeout();
		postprocess(*out);
	}
	DXGI_P_LOGF("GetDesc3 call result: 0x%X", hr);
	return hr;
}

// utilities

template<typename T>
T &Adapter::get() const noexcept
{
	return *chain.anyOr<T>(&AdapterStub::instance());
}

bool Adapter::isIntegrated() const noexcept
{
	// Unreal Engine detect integrated GPU with this algo
	DXGI_QUERY_VIDEO_MEMORY_INFO info{};
	const auto hr = get<IDXGIAdapter3>().QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_NON_LOCAL, &info);
	if(hr != S_OK) {
		DXGI_P_LOGF("query video memory info failed: 0x%X", hr);
	}
	return hr == S_OK && info.Budget == 0;
}

template<typename T>
void Adapter::postprocess(T &info) const
{
	if(!overrides_disabled) {
		const auto &overrides = config->overrideSection();
		if(overrides.gpu_description) {
			std::wstring new_description = *overrides.gpu_description;
			if(new_description.size() >= sizeof(info.Description)) {
				new_description.resize(sizeof(info.Description) - 1);
			}
			DXGI_P_LOGF("override GPU description from '%ls' to '%ls'", info.Description, new_description.data());
			std::memset(info.Description, 0, sizeof(info.Description));
			std::memcpy(info.Description, new_description.data(), new_description.size());
		}
		if(overrides.gpu_vendor_id) {
			DXGI_P_LOGF("override GPU vendor id from 0x%04X to 0x%04X", info.VendorId, *overrides.gpu_vendor_id);
			info.VendorId = *overrides.gpu_vendor_id;
		}
		if(overrides.gpu_device_id) {
			DXGI_P_LOGF("override GPU device id from 0x%04X to 0x%04X", info.DeviceId, *overrides.gpu_device_id);
			info.DeviceId = *overrides.gpu_device_id;
		}
		if(overrides.gpu_dedicated_memory_size) {
			DXGI_P_LOGF(
			    "override dedicated video memory size from %llu to %llu", info.DedicatedVideoMemory, *overrides.gpu_dedicated_memory_size);
			info.DedicatedVideoMemory = *overrides.gpu_dedicated_memory_size;
		}
	}
}

void Adapter::postprocess(DXGI_MEMORY_SEGMENT_GROUP group, DXGI_QUERY_VIDEO_MEMORY_INFO &info) const
{
	if(!overrides_disabled) {
		const auto &overrides = config->overrideSection();
		if(overrides.gpu_dedicated_memory_size && group == DXGI_MEMORY_SEGMENT_GROUP_LOCAL) {
			DXGI_P_LOGF("override dedicated video memory size from %llu to %llu (QVMI)", info.Budget, *overrides.gpu_dedicated_memory_size);
			info.Budget = *overrides.gpu_dedicated_memory_size;
		}
		if(overrides.gpu_non_local_memory_size && group == DXGI_MEMORY_SEGMENT_GROUP_NON_LOCAL) {
			DXGI_P_LOGF("override non local video memory size from %llu to %llu (QVMI)", info.Budget, *overrides.gpu_non_local_memory_size);
			info.Budget = *overrides.gpu_non_local_memory_size;
		}
	}
}

void Adapter::checkTimeout()
{
	if(!overrides_disabled) {
		if(const auto &overrides = config->overrideSection(); overrides.disable_after) {
			const auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(Clock::now() - created_at);
			if(elapsed >= *overrides.disable_after) {
				DXGI_P_LOGF("disable overrides after %llus", elapsed.count());
				overrides_disabled = true;
			}
		}
	}
}

}  // namespace dxgi_proxy
