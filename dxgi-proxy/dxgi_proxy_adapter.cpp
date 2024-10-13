#include "pch.h"

DXGI_P_LOG_SYSTEM(ADAPTER);

namespace dxgi_proxy {
namespace {

template<typename T>
void applyOverride(const Config &config, T *info) noexcept
{
	if(info) {
		const auto &overrides = config.overrideSection();
		if(overrides.gpu_description) {
			const std::wstring old_description = info->Description;
			std::wstring new_description = *overrides.gpu_description;
			if(new_description.size() >= sizeof(info->Description)) {
				new_description.resize(sizeof(info->Description));
			}
			std::memset(info->Description, 0, sizeof(info->Description));
			std::memcpy(info->Description, new_description.data(), new_description.size());
			DXGI_P_LOGF("override GPU description from '%ls' to '%ls'", old_description.data(), new_description.data());
		}
		if(overrides.gpu_vendor_id) {
			const auto old_vendor_id = info->VendorId;
			info->VendorId = *overrides.gpu_vendor_id;
			DXGI_P_LOGF("override GPU vendor id from 0x%04X to 0x%04X", old_vendor_id, *overrides.gpu_vendor_id);
		}
		if(overrides.gpu_device_id) {
			const auto old_device_id = info->DeviceId;
			info->DeviceId = *overrides.gpu_device_id;
			DXGI_P_LOGF("override GPU device id from 0x%04X to 0x%04X", old_device_id, *overrides.gpu_device_id);
		}
		if(overrides.gpu_dedicated_memory_size) {
			const auto old_mem_size = info->DedicatedVideoMemory;
			info->DedicatedVideoMemory = *overrides.gpu_dedicated_memory_size;
			DXGI_P_LOGF("override dedicated video memory size from %llu to %llu", old_mem_size, *overrides.gpu_dedicated_memory_size);
		}
	}
}

}  // namespace

HRESULT Adapter::wrap(const Config &config, ComPtr<IUnknown> parent, REFIID riid, void *original, void **out)
{
	if(!original) {
		DXGI_P_LOG("attempt to wrap null pointer");
		return E_NOINTERFACE;
	} else {
		ComPtr<IUnknown> unknown{static_cast<IUnknown *>(original)};
		const auto wrapper = makeObject<Adapter>(config, std::move(parent), std::move(unknown));
		DXGI_P_LOG("new wrapper created");
		return wrapper->QueryInterface(riid, out);
	}
}

Adapter::Adapter(const Config &config, ComPtr<IUnknown> parent, ComPtr<IUnknown> unknown)
    : config{&config}
    , parent{std::move(parent)}
    , unknown{std::move(unknown)}
    , object{queryInterface<IDXGIObject>(this->unknown)}
    , adapter{queryInterface<IDXGIAdapter>(this->unknown)}
    , adapter1{queryInterface<IDXGIAdapter1>(this->unknown)}
    , adapter2{queryInterface<IDXGIAdapter2>(this->unknown)}
    , adapter3{queryInterface<IDXGIAdapter3>(this->unknown)}
    , adapter4{queryInterface<IDXGIAdapter4>(this->unknown)}
{
	DXGI_P_LOG("factory is initialized");
	DXGI_P_LOGF("\tIUnknown: %p", this->unknown.get());
	DXGI_P_LOGF("\tIDXGIObject: %p", object.get());
	DXGI_P_LOGF("\tIDXGIAdapter: %p", adapter.get());
	DXGI_P_LOGF("\tIDXGIAdapter1: %p", adapter1.get());
	DXGI_P_LOGF("\tIDXGIAdapter2: %p", adapter2.get());
	DXGI_P_LOGF("\tIDXGIAdapter3: %p", adapter3.get());
	DXGI_P_LOGF("\tIDXGIAdapter4: %p", adapter4.get());
}

// IUnknown methods

HRESULT DXGI_P_API Adapter::QueryInterface(REFIID riid, void **out)
{
	DXGI_P_LOGF("QueryInterface call with '%s' riid", format(riid).data());
	if(!out) {
		DXGI_P_LOG("output pointer is nullptr");
		return E_POINTER;
	} else if(isWrapped(riid)) {
		*out = addRef(this).release();
		DXGI_P_LOG("return pointer to self");
		return S_OK;
	} else {
		DXGI_P_LOG("unknown/unsupported interface, call original object");
		const auto hr = object ? object->QueryInterface(riid, out) : E_NOINTERFACE;
		DXGI_P_LOGF("QueryInterface call result: 0x%X", hr);
		return hr;
	}
}

// IDXGIObject methods

HRESULT DXGI_P_API Adapter::GetParent(REFIID riid, void **out)
{
	return parent ? parent->QueryInterface(riid, out) : E_NOTIMPL;
}

HRESULT DXGI_P_API Adapter::GetPrivateData(REFGUID riid, UINT *size, void *data)
{
	auto *o = getObject();
	return o ? o->GetPrivateData(riid, size, data) : E_NOTIMPL;
}

HRESULT DXGI_P_API Adapter::SetPrivateData(REFGUID riid, UINT size, const void *data)
{
	auto *o = getObject();
	return o ? o->SetPrivateData(riid, size, data) : E_NOTIMPL;
}

HRESULT DXGI_P_API Adapter::SetPrivateDataInterface(REFGUID riid, const IUnknown *obj)
{
	auto *o = getObject();
	return o ? o->SetPrivateDataInterface(riid, obj) : E_NOTIMPL;
}

// IDXGIAdapter methods

HRESULT DXGI_P_API Adapter::EnumOutputs(UINT output, IDXGIOutput **out)
{
	auto *a = getAdapter();
	return a ? a->EnumOutputs(output, out) : E_NOTIMPL;
}

HRESULT DXGI_P_API Adapter::GetDesc(DXGI_ADAPTER_DESC *out)
{
	DXGI_P_LOG("GetDesc call");
	auto *a = getAdapter();
	const auto hr = a ? a->GetDesc(out) : E_NOTIMPL;
	if(out && hr == S_OK) {
		applyOverride(*config, out);
	}
	DXGI_P_LOGF("GetDesc call result: 0x%X", hr);
	return hr;
}

HRESULT DXGI_P_API Adapter::CheckInterfaceSupport(REFGUID name, LARGE_INTEGER *out)
{
	auto *a = getAdapter();
	return a ? a->CheckInterfaceSupport(name, out) : E_NOTIMPL;
}

// IDXGIAdapter1 methods

HRESULT DXGI_P_API Adapter::GetDesc1(DXGI_ADAPTER_DESC1 *out)
{
	DXGI_P_LOG("GetDesc1 call");
	auto *a = getAdapter1();
	const auto hr = a ? a->GetDesc1(out) : E_NOTIMPL;
	if(out && hr == S_OK) {
		applyOverride(*config, out);
	}
	DXGI_P_LOGF("GetDesc1 call result: 0x%X", hr);
	return hr;
}

// IDXGIAdapter2 methods

HRESULT DXGI_P_API Adapter::GetDesc2(DXGI_ADAPTER_DESC2 *out)
{
	DXGI_P_LOG("GetDesc2 call");
	auto *a = getAdapter2();
	const auto hr = a ? a->GetDesc2(out) : E_NOTIMPL;
	if(out && hr == S_OK) {
		applyOverride(*config, out);
	}
	DXGI_P_LOGF("GetDesc2 call result: 0x%X", hr);
	return hr;
}

// IDXGIAdapter3 methods

HRESULT DXGI_P_API Adapter::RegisterHardwareContentProtectionTeardownStatusEvent(HANDLE event, DWORD *cookie)
{
	auto *a = getAdapter3();
	return a ? a->RegisterHardwareContentProtectionTeardownStatusEvent(event, cookie) : E_NOTIMPL;
}

void DXGI_P_API Adapter::UnregisterHardwareContentProtectionTeardownStatus(DWORD cookie)
{
	if(auto *a = getAdapter3()) {
		a->UnregisterHardwareContentProtectionTeardownStatus(cookie);
	}
}

HRESULT DXGI_P_API Adapter::QueryVideoMemoryInfo(UINT index, DXGI_MEMORY_SEGMENT_GROUP group, DXGI_QUERY_VIDEO_MEMORY_INFO *out)
{
	DXGI_P_LOGF("QueryVideoMemoryInfo call with '%u' index and '%i' group", index, group);
	auto *a = getAdapter3();
	const auto hr = a ? a->QueryVideoMemoryInfo(index, group, out) : E_NOTIMPL;
	if(out && group == DXGI_MEMORY_SEGMENT_GROUP_NON_LOCAL && hr == S_OK) {
		if(const auto &overrides = config->overrideSection(); overrides.gpu_dedicated_memory_size) {
			const auto old_mem_size = out->Budget;
			out->Budget = *overrides.gpu_dedicated_memory_size;
			DXGI_P_LOGF(
			    "override dedicated video memory size from %llu to %llu (QVMI)", old_mem_size, *overrides.gpu_dedicated_memory_size);
		}
	}
	DXGI_P_LOGF("QueryVideoMemoryInfo call result: 0x%X", hr);
	return hr;
}

HRESULT DXGI_P_API Adapter::SetVideoMemoryReservation(UINT index, DXGI_MEMORY_SEGMENT_GROUP group, UINT64 reservation)
{
	auto *a = getAdapter3();
	return a ? a->SetVideoMemoryReservation(index, group, reservation) : E_NOTIMPL;
}

HRESULT DXGI_P_API Adapter::RegisterVideoMemoryBudgetChangeNotificationEvent(HANDLE event, DWORD *cookie)
{
	auto *a = getAdapter3();
	return a ? a->RegisterVideoMemoryBudgetChangeNotificationEvent(event, cookie) : E_NOTIMPL;
}

void DXGI_P_API Adapter::UnregisterVideoMemoryBudgetChangeNotification(DWORD cookie)
{
	if(auto *a = getAdapter3()) {
		a->UnregisterVideoMemoryBudgetChangeNotification(cookie);
	}
}

// IDXGIAdapter4 methods

HRESULT DXGI_P_API Adapter::GetDesc3(DXGI_ADAPTER_DESC3 *out)
{
	DXGI_P_LOG("GetDesc3 call");
	auto *a = getAdapter4();
	const auto hr = a ? a->GetDesc3(out) : E_NOTIMPL;
	if(out && hr == S_OK) {
		applyOverride(*config, out);
	}
	DXGI_P_LOGF("GetDesc3 call result: 0x%X", hr);
	return hr;
}

// utilities

IDXGIObject *Adapter::getObject() const noexcept
{
	if(object) {
		return object.get();
	} else if(adapter) {
		return adapter.get();
	} else if(adapter1) {
		return adapter1.get();
	} else if(adapter2) {
		return adapter2.get();
	} else if(adapter3) {
		return adapter3.get();
	} else {
		return adapter4.get();
	}
}

IDXGIAdapter *Adapter::getAdapter() const noexcept
{
	if(adapter) {
		return adapter.get();
	} else if(adapter1) {
		return adapter1.get();
	} else if(adapter2) {
		return adapter2.get();
	} else if(adapter3) {
		return adapter3.get();
	} else {
		return adapter4.get();
	}
}

IDXGIAdapter1 *Adapter::getAdapter1() const noexcept
{
	if(adapter1) {
		return adapter1.get();
	} else if(adapter2) {
		return adapter2.get();
	} else if(adapter3) {
		return adapter3.get();
	} else {
		return adapter4.get();
	}
}

IDXGIAdapter2 *Adapter::getAdapter2() const noexcept
{
	if(adapter2) {
		return adapter2.get();
	} else if(adapter3) {
		return adapter3.get();
	} else {
		return adapter4.get();
	}
}

IDXGIAdapter3 *Adapter::getAdapter3() const noexcept
{
	if(adapter3) {
		return adapter3.get();
	} else {
		return adapter4.get();
	}
}

IDXGIAdapter4 *Adapter::getAdapter4() const noexcept
{
	return adapter4.get();
}

bool Adapter::isWrapped(REFIID riid) const noexcept
{
	if(riid == __uuidof(IUnknown)) {
		return static_cast<bool>(unknown);
	} else if(riid == __uuidof(IDXGIObject)) {
		return static_cast<bool>(object);
	} else if(riid == __uuidof(IDXGIAdapter)) {
		return static_cast<bool>(adapter);
	} else if(riid == __uuidof(IDXGIAdapter1)) {
		return static_cast<bool>(adapter1);
	} else if(riid == __uuidof(IDXGIAdapter2)) {
		return static_cast<bool>(adapter2);
	} else if(riid == __uuidof(IDXGIAdapter3)) {
		return static_cast<bool>(adapter3);
	} else if(riid == __uuidof(IDXGIAdapter4)) {
		return static_cast<bool>(adapter4);
	} else {
		return false;
	}
}

}  // namespace dxgi_proxy
