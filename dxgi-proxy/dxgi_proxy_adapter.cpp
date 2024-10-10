#include "pch.h"

namespace dxgi_proxy {
namespace {

template<typename T>
void fixGpuInfo(T *info) noexcept
{
	if(info) {
		// TODO: this should be in ini-file
		std::wstring gpu_name(L"AMD Radeon RX 6700");
		std::memset(info->Description, 0, sizeof(info->Description));
		std::memcpy(info->Description, gpu_name.data(), gpu_name.size());
		info->VendorId = 0x1002;
		info->DeviceId = 0x73DF;
		info->DedicatedVideoMemory = 4ull * 1024 * 1024 * 1024;  // 4Gb
		log("Adapter", "Fixed info!");
	} else {
		log("Adapter", "Can't fix info: nullptr");
	}
}

}  // namespace

HRESULT Adapter::wrap(ComPtr<IUnknown> parent, REFIID riid, void *original, void **out)
{
	if(!original) {
		return E_NOINTERFACE;
	} else {
		ComPtr<IUnknown> unknown{static_cast<IUnknown *>(original)};
		const auto wrapper = makeObject<Adapter>(std::move(parent), std::move(unknown));
		log("Adapter", "Wrapper created");
		return wrapper->QueryInterface(riid, out);
	}
}

Adapter::Adapter(ComPtr<IUnknown> parent, ComPtr<IUnknown> unknown)
    : parent{std::move(parent)}
    , unknown{std::move(unknown)}
    , object{queryInterface<IDXGIObject>(this->unknown)}
    , adapter{queryInterface<IDXGIAdapter>(this->unknown)}
    , adapter1{queryInterface<IDXGIAdapter1>(this->unknown)}
    , adapter2{queryInterface<IDXGIAdapter2>(this->unknown)}
    , adapter3{queryInterface<IDXGIAdapter3>(this->unknown)}
    , adapter4{queryInterface<IDXGIAdapter4>(this->unknown)}
{}

// IUnknown methods

HRESULT DXGI_P_API Adapter::QueryInterface(REFIID riid, void **out)
{
	if(!out) {
		return E_POINTER;
	} else if(isWrapped(riid)) {
		*out = addRef(this).release();
		log("Adapter", "Wrapper used!");
		return S_OK;
	} else {
		log("Adapter", "Unknown/unsupported interface usage");
		return object ? object->QueryInterface(riid, out) : E_NOINTERFACE;
	}
}

// IDXGIObject methods

HRESULT DXGI_P_API Adapter::GetParent(REFIID riid, void **out)
{
	return parent ? parent->QueryInterface(riid, out) : E_NOTIMPL;
}

HRESULT DXGI_P_API Adapter::GetPrivateData(REFGUID riid, UINT *size, void *data)
{
	return object ? object->GetPrivateData(riid, size, data) : E_NOTIMPL;
}

HRESULT DXGI_P_API Adapter::SetPrivateData(REFGUID riid, UINT size, const void *data)
{
	return object ? object->SetPrivateData(riid, size, data) : E_NOTIMPL;
}

HRESULT DXGI_P_API Adapter::SetPrivateDataInterface(REFGUID riid, const IUnknown *obj)
{
	return object ? object->SetPrivateDataInterface(riid, obj) : E_NOTIMPL;
}

// IDXGIAdapter methods

HRESULT DXGI_P_API Adapter::EnumOutputs(UINT output, IDXGIOutput **out)
{
	return adapter ? adapter->EnumOutputs(output, out) : E_NOTIMPL;
}

HRESULT DXGI_P_API Adapter::GetDesc(DXGI_ADAPTER_DESC *out)
{
	const auto hr = adapter ? adapter->GetDesc(out) : E_NOTIMPL;
	if(hr == S_OK) {
		fixGpuInfo(out);
	}
	return hr;
}

HRESULT DXGI_P_API Adapter::CheckInterfaceSupport(REFGUID name, LARGE_INTEGER *out)
{
	return adapter ? adapter->CheckInterfaceSupport(name, out) : E_NOTIMPL;
}

// IDXGIAdapter1 methods

HRESULT DXGI_P_API Adapter::GetDesc1(DXGI_ADAPTER_DESC1 *out)
{
	const auto hr = adapter1 ? adapter1->GetDesc1(out) : E_NOTIMPL;
	if(hr == S_OK) {
		fixGpuInfo(out);
	}
	return hr;
}

// IDXGIAdapter2 methods

HRESULT DXGI_P_API Adapter::GetDesc2(DXGI_ADAPTER_DESC2 *out)
{
	const auto hr = adapter2 ? adapter2->GetDesc2(out) : E_NOTIMPL;
	if(hr == S_OK) {
		fixGpuInfo(out);
	}
	return hr;
}

// IDXGIAdapter3 methods

HRESULT DXGI_P_API Adapter::RegisterHardwareContentProtectionTeardownStatusEvent(HANDLE event, DWORD *cookie)
{
	return adapter3 ? adapter3->RegisterHardwareContentProtectionTeardownStatusEvent(event, cookie) : E_NOTIMPL;
}

void DXGI_P_API Adapter::UnregisterHardwareContentProtectionTeardownStatus(DWORD cookie)
{
	if(adapter3) {
		adapter3->UnregisterHardwareContentProtectionTeardownStatus(cookie);
	}
}

HRESULT DXGI_P_API Adapter::QueryVideoMemoryInfo(UINT index, DXGI_MEMORY_SEGMENT_GROUP group, DXGI_QUERY_VIDEO_MEMORY_INFO *out)
{
	const auto hr = adapter3 ? adapter3->QueryVideoMemoryInfo(index, group, out) : E_NOTIMPL;
	if(hr == S_OK && group == DXGI_MEMORY_SEGMENT_GROUP_NON_LOCAL && out) {
		// TODO: this should be in ini-file
		out->Budget = 4ull * 1024 * 1024 * 1024;  // 4Gb
	}
	return hr;
}

HRESULT DXGI_P_API Adapter::SetVideoMemoryReservation(UINT index, DXGI_MEMORY_SEGMENT_GROUP group, UINT64 reservation)
{
	return adapter3 ? adapter3->SetVideoMemoryReservation(index, group, reservation) : E_NOTIMPL;
}

HRESULT DXGI_P_API Adapter::RegisterVideoMemoryBudgetChangeNotificationEvent(HANDLE event, DWORD *cookie)
{
	return adapter3 ? adapter3->RegisterVideoMemoryBudgetChangeNotificationEvent(event, cookie) : E_NOTIMPL;
}

void DXGI_P_API Adapter::UnregisterVideoMemoryBudgetChangeNotification(DWORD cookie)
{
	if(adapter3) {
		adapter3->UnregisterVideoMemoryBudgetChangeNotification(cookie);
	}
}

// IDXGIAdapter4 methods

HRESULT DXGI_P_API Adapter::GetDesc3(DXGI_ADAPTER_DESC3 *out)
{
	const auto hr = adapter4 ? adapter4->GetDesc3(out) : E_NOTIMPL;
	if(hr == S_OK) {
		fixGpuInfo(out);
	}
	return hr;
}

// utilities

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
