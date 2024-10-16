#pragma once

namespace dxgi_proxy {

template<typename T>
class Object : public T
{
public:
	Object() = default;
	Object(const Object &) = delete;
	Object &operator=(const Object &) = delete;

	// IUnknown common part
	ULONG DXGI_P_API AddRef() override { return ++ref_count; }
	ULONG DXGI_P_API Release() override
	{
		const auto remain = --ref_count;
		if(!remain) {
			delete this;
		}
		return remain;
	}

private:
	std::atomic<ULONG> ref_count{};
};

}  // namespace dxgi_proxy
