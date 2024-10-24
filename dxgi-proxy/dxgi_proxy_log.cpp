#include "pch.h"

DXGI_P_LOG_SYSTEM(LOG);

namespace dxgi_proxy::log {
namespace {

std::mutex sMutex;
std::unique_ptr<std::ofstream> sOut;

}  // namespace

void init(const Config &config)
{
	if(config.logSection().enable) {
		std::unique_lock lock{sMutex};
		sOut.reset(new std::ofstream{"dxgi_proxy.log"});
		lock.unlock();
		log("logging is enabled");
	}
}

bool enabled() noexcept
{
	return static_cast<bool>(sOut);
}

void log(const std::string &msg)
{
	std::unique_lock lock{sMutex};
	*sOut << msg << std::endl;
}

std::string format(const char *format, ...)
{
	static const auto clamp = [](int size) { return size > 0 ? size : 0; };

	std::string out;
	va_list args;

	va_start(args, format);
	out.resize(clamp(vsnprintf(nullptr, 0, format, args)));
	va_end(args);

	va_start(args, format);
	out.resize(clamp(vsnprintf(out.data(), out.size() + 1, format, args)));
	va_end(args);

	return out;
}

std::string format(const GUID &guid)
{
	return format("%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
	              guid.Data1,
	              guid.Data2,
	              guid.Data3,
	              guid.Data4[0],
	              guid.Data4[1],
	              guid.Data4[2],
	              guid.Data4[3],
	              guid.Data4[4],
	              guid.Data4[5],
	              guid.Data4[6],
	              guid.Data4[7]);
}

}  // namespace dxgi_proxy::log
