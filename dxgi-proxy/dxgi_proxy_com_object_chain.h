#pragma once

namespace dxgi_proxy {

template<typename... T>
class ComObjectChain final
{
public:
	explicit ComObjectChain(ComPtr<IUnknown> object) noexcept
	    : storage{queryInterface<T>(object)...}
	{}

	ComObjectChain(const ComObjectChain &) = delete;
	ComObjectChain &operator=(const ComObjectChain &) = delete;

	template<typename Fn>
	void enumerate(Fn &&fn) const noexcept
	{
		(fn(std::get<ComPtr<T>>(storage).get()) && ...);
	}

	template<typename U>
	[[nodiscard]] U *anyOr(U *def) const noexcept
	{
		U *out = nullptr;
		enumerate([&out](auto *p) -> bool {
			using TP = std::decay_t<decltype(*p)>;
			if constexpr(std::is_convertible_v<TP *, U *>) {
				out = p;
			}
			return !out;
		});
		return out ? out : def;
	}

	[[nodiscard]] HRESULT proxiedQueryInterface(IUnknown *proxy, REFIID riid, void **out) const noexcept
	{
		if(!out) {
			return E_POINTER;
		} else if(has(riid)) {
			*out = addRef(proxy).release();
			return S_OK;
		} else if(auto *any = anyOr<IUnknown>(nullptr)) {
			return any->QueryInterface(riid, out);
		} else {
			return E_NOINTERFACE;
		}
	}

private:
	const std::tuple<ComPtr<T>...> storage;

	[[nodiscard]] bool has(REFIID riid) const noexcept
	{
		bool r = false;
		enumerate([this, &riid, &r](auto *p) {
			using TP = std::decay_t<decltype(*p)>;
			r = riid == __uuidof(TP) && anyOr<TP>(nullptr);
			return !r;
		});
		return r;
	}
};

}  // namespace dxgi_proxy
