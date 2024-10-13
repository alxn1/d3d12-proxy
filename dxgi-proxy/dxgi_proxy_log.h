#pragma once

namespace dxgi_proxy {

#define DXGI_P_LOG_SYSTEM(NAME) static const char sLogSystem[] = #NAME

#define DXGI_P_LOG(MSG) DXGI_P_LOGF("%s", MSG)
#define DXGI_P_LOGF(FORMAT, ...)                                 \
	do {                                                         \
		if(isLogEnabled()) {                                     \
			log(format("%s: " FORMAT, sLogSystem, __VA_ARGS__)); \
		}                                                        \
	} while(false)

void initLog(const Config &config);

[[nodiscard]] bool isLogEnabled() noexcept;

void log(const std::string &msg);

[[nodiscard]] std::string format(const char *format, ...);
[[nodiscard]] std::string format(const GUID &guid);

}  // namespace dxgi_proxy
