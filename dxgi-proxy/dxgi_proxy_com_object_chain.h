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

	template<typename U>
	[[nodiscard]] U *anyOf(U *def = nullptr) const noexcept
	{
		U *p = anyOf<U, T...>();
		return p ? p : def;
	}

	template<typename Fn>
	void enumerate(Fn &&fn) const noexcept
	{
		enumerate<Fn, T...>(std::forward<Fn>(fn));
	}

	[[nodiscard]] HRESULT proxiedQueryInterface(IUnknown *proxy, REFIID riid, void **out) const noexcept
	{
		if(!out) {
			return E_POINTER;
		} else if(has<T...>(riid)) {
			*out = addRef(proxy).release();
			return S_OK;
		} else if(auto *any = anyOf<IUnknown>()) {
			return any->QueryInterface(riid, out);
		} else {
			return E_NOINTERFACE;
		}
	}

private:
	const std::tuple<ComPtr<T>...> storage;

	template<typename U>
	[[nodiscard]] U *get() const noexcept
	{
		return std::get<ComPtr<U>>(storage).get();
	}

	template<typename T1, typename... TN>
	[[nodiscard]] bool has(REFIID riid) const noexcept
	{
		if(riid == __uuidof(T1)) {
			return !!anyOf<T1>();
		}
		if constexpr(sizeof...(TN)) {
			return has<TN...>(riid);
		}
		return false;
	}

	template<typename U, typename T1, typename... TN>
	[[nodiscard]] U *anyOf() const noexcept
	{
		if constexpr(std::is_convertible_v<T1 *, U *>) {
			if(auto p = get<U>()) {
				return p;
			}
		}
		if constexpr(sizeof...(TN)) {
			return anyOf<U, TN...>();
		}
		return nullptr;
	}

	template<typename Fn, typename T1, typename... TN>
	void enumerate(Fn &&fn) const noexcept
	{
		fn(get<T1>());
		if constexpr(sizeof...(TN)) {
			return enumerate<Fn, TN...>(std::forward<Fn>(fn));
		}
	}
};

}  // namespace dxgi_proxy
