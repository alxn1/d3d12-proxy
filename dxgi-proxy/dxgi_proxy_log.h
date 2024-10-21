#pragma once

namespace dxgi_proxy::log {

#define DXGI_P_LOG_SYSTEM(NAME) static const char sLogSystem[] = #NAME

#define DXGI_P_LOG(MSG) DXGI_P_LOGF("%s", MSG)
#define DXGI_P_LOGF(FORMAT, ...)                                                                   \
	do {                                                                                           \
		if(dxgi_proxy::log::enabled()) {                                                           \
			dxgi_proxy::log::log(dxgi_proxy::log::format("%s: " FORMAT, sLogSystem, __VA_ARGS__)); \
		}                                                                                          \
	} while(false)

void init(const Config &config);

[[nodiscard]] bool enabled() noexcept;

void log(const std::string &msg);

[[nodiscard]] std::string format(const char *format, ...);
[[nodiscard]] std::string format(const GUID &guid);

}  // namespace dxgi_proxy::log
