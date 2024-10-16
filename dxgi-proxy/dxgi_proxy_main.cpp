#include "pch.h"

using namespace dxgi_proxy;

extern "C"
{

HRESULT WINAPI CreateDXGIFactory(REFIID riid, void **out)
{
	return Proxy::instance().create_factory(riid, out);
}

HRESULT WINAPI CreateDXGIFactory1(REFIID riid, void **out)
{
	return Proxy::instance().create_factory1(riid, out);
}

HRESULT WINAPI CreateDXGIFactory2(UINT flags, REFIID riid, void **out)
{
	return Proxy::instance().create_factory2(flags, riid, out);
}

HRESULT WINAPI DXGIDeclareAdapterRemovalSupport()
{
	return Proxy::instance().declare_adapter_removal_support();
}

HRESULT WINAPI DXGIGetDebugInterface1(UINT flags, REFIID riid, void **out)
{
	return Proxy::instance().get_debug_interface1(flags, riid, out);
}

}  // extern "C"