#include "pch.h"

namespace dxgi_proxy {
namespace {

std::unique_ptr<std::ofstream> log_out;
std::mutex log_mutex;

}  // namespace

void log(const char *m, const char *msg)
{
	// TODO: implement normal logging later
	std::unique_lock<std::mutex> lock{log_mutex};
	if(!log_out) {
		log_out.reset(new std::ofstream{"dxgi_proxy.log", std::ofstream::out | std::ofstream::ate});
	}
	*log_out << m << " -> " << msg << std::endl;
}

}  // namespace dxgi_proxy
