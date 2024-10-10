#include "pch.h"

using namespace dxgi_proxy;

namespace {

HRESULT(WINAPI *pCreateDXGIFactory)(REFIID, void **) = nullptr;
HRESULT(WINAPI *pCreateDXGIFactory1)(REFIID, void **) = nullptr;
HRESULT(WINAPI *pCreateDXGIFactory2)(UINT, REFIID, void **) = nullptr;
HRESULT(WINAPI *pDXGIDeclareAdapterRemovalSupport)() = nullptr;
HRESULT(WINAPI *pDXGIGetDebugInterface1)(UINT, REFIID, void **) = nullptr;

[[nodiscard]] std::string dxgiModulePath()
{
	char path[MAX_PATH];
	GetSystemDirectory(path, sizeof(path) - 1);
	return std::string{path} + "\\dxgi.dll";
}

void initialize()
{
	static std::once_flag gInitFlag;

	std::call_once(gInitFlag, [] {
		HMODULE m = LoadLibrary("dxgi.o.dll");
		if(!m) {
			m = LoadLibrary(dxgiModulePath().data());
			if(m) {
#define GET_FN_PTR(name) reinterpret_cast<decltype(p##name)>(GetProcAddress(m, #name))
				pCreateDXGIFactory = GET_FN_PTR(CreateDXGIFactory);
				pCreateDXGIFactory1 = GET_FN_PTR(CreateDXGIFactory1);
				pCreateDXGIFactory2 = GET_FN_PTR(CreateDXGIFactory2);
				pDXGIDeclareAdapterRemovalSupport = GET_FN_PTR(DXGIDeclareAdapterRemovalSupport);
				pDXGIGetDebugInterface1 = GET_FN_PTR(DXGIGetDebugInterface1);
#undef GET_FN_PTR
			} else {
				log("DXGI", "can't load original dxgi.dll");
			}
		}
		log("DXGI", "initialized");
	});
}

}  // namespace

extern "C"
{

HRESULT WINAPI CreateDXGIFactory(REFIID riid, void **out)
{
	initialize();
	log("DXGI", "create factory");
	const auto hr = pCreateDXGIFactory ? pCreateDXGIFactory(riid, out) : E_NOINTERFACE;
	return hr == S_OK ? Factory::wrap(riid, *out, out) : hr;
}

HRESULT WINAPI CreateDXGIFactory1(REFIID riid, void **out)
{
	initialize();
	log("DXGI", "create factory 1");
	const auto hr = pCreateDXGIFactory1 ? pCreateDXGIFactory1(riid, out) : E_NOINTERFACE;
	return hr == S_OK ? Factory::wrap(riid, *out, out) : hr;
}

HRESULT WINAPI CreateDXGIFactory2(UINT flags, REFIID riid, void **out)
{
	initialize();
	log("DXGI", "create factory 2");
	const auto hr = pCreateDXGIFactory2 ? pCreateDXGIFactory2(flags, riid, out) : E_NOINTERFACE;
	return hr == S_OK ? Factory::wrap(riid, *out, out) : hr;
}

HRESULT WINAPI DXGIDeclareAdapterRemovalSupport()
{
	initialize();
	return pDXGIDeclareAdapterRemovalSupport ? pDXGIDeclareAdapterRemovalSupport() : E_NOINTERFACE;
}

HRESULT WINAPI DXGIGetDebugInterface1(UINT flags, REFIID riid, void **out)
{
	initialize();
	return pDXGIGetDebugInterface1 ? pDXGIGetDebugInterface1(flags, riid, out) : E_NOINTERFACE;
}

}  // extern "C"