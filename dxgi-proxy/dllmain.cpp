#include "pch.h"

namespace {

std::once_flag gInitFlag;

HMODULE pDXGIModule = nullptr;
HRESULT (WINAPI *pCreateDXGIFactory)(REFIID, _COM_Outptr_ void**) = nullptr;
HRESULT (WINAPI *pCreateDXGIFactory1)(REFIID, _COM_Outptr_ void**) = nullptr;
HRESULT (WINAPI *pCreateDXGIFactory2)(UINT, REFIID, _COM_Outptr_ void**) = nullptr;
HRESULT (WINAPI *pDXGIDeclareAdapterRemovalSupport)() = nullptr;
HRESULT (WINAPI *pDXGIGetDebugInterface1)(UINT, REFIID, _COM_Outptr_ void**) = nullptr;

[[nodiscard]] std::string dxgiModulePath()
{
	char path[MAX_PATH];
	GetSystemDirectory(path, sizeof(path) - 1);
	return std::string{path} + "\\dxgi.dll";
}

void log(std::string v)
{
	std::ofstream{"dxgi_proxy.log", std::ios_base::out | std::ios_base::ate} << "DXGI-Proxy: " << v << "\n";
}

void initialize()
{
	std::call_once(gInitFlag, [] {
		if (!pDXGIModule) {
			pDXGIModule = LoadLibrary(dxgiModulePath().data());
			if(pDXGIModule) {
#define GET_FN_PTR(name) reinterpret_cast<decltype(p##name)>(GetProcAddress(pDXGIModule, #name))
				pCreateDXGIFactory = GET_FN_PTR(CreateDXGIFactory);
				pCreateDXGIFactory1 = GET_FN_PTR(CreateDXGIFactory1);
				pCreateDXGIFactory2 = GET_FN_PTR(CreateDXGIFactory2);
				pDXGIDeclareAdapterRemovalSupport = GET_FN_PTR(DXGIDeclareAdapterRemovalSupport);
				pDXGIGetDebugInterface1 = GET_FN_PTR(DXGIGetDebugInterface1);
#undef GET_FN_PTR
				log("initialized");
			} else {
				log("initialization failed :(");
			}
		}
	});
}

} // namespace

HRESULT WINAPI CreateDXGIFactory(REFIID riid, _COM_Outptr_ void** ppFactory)
{
	initialize();
	log("create factory call");
	return pCreateDXGIFactory ? pCreateDXGIFactory(riid, ppFactory) : E_NOINTERFACE;
}

HRESULT WINAPI CreateDXGIFactory1(REFIID riid, _COM_Outptr_ void** ppFactory)
{
	initialize();
	log("create factory1 call");
	return pCreateDXGIFactory1 ? pCreateDXGIFactory1(riid, ppFactory) : E_NOINTERFACE;
}

HRESULT WINAPI CreateDXGIFactory2(UINT flags, REFIID riid, _COM_Outptr_ void** ppFactory)
{
	initialize();
	log("create factory2 call");
	return pCreateDXGIFactory2 ? pCreateDXGIFactory2(flags, riid, ppFactory) : E_NOINTERFACE;
}

HRESULT WINAPI DXGIDeclareAdapterRemovalSupport()
{
	initialize();
	log("declare adapter removal support call");
	return pDXGIDeclareAdapterRemovalSupport ? pDXGIDeclareAdapterRemovalSupport() : E_NOINTERFACE;
}

HRESULT WINAPI DXGIGetDebugInterface1(UINT flags, REFIID riid, _COM_Outptr_ void** ppDebug)
{
	initialize();
	log("get debug interface1 call");
	return pDXGIGetDebugInterface1 ? pDXGIGetDebugInterface1(flags, riid, ppDebug) : E_NOINTERFACE;
}