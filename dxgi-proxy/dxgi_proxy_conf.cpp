#include "pch.h"

namespace dxgi_proxy {
namespace {

[[nodiscard]] int clamp(int size) noexcept
{
	return size > 0 ? size : 0;
}

[[nodiscard]] std::wstring cvt(const std::string &str)
{
	std::wstring out;
	if(!str.empty()) {
		out.resize(clamp(MultiByteToWideChar(CP_THREAD_ACP, 0, str.data(), static_cast<int>(str.size()), nullptr, 0)));
		out.resize(clamp(
		    MultiByteToWideChar(CP_THREAD_ACP, 0, str.data(), static_cast<int>(str.size()), out.data(), static_cast<int>(out.size()))));
	}
	return out;
}

[[nodiscard]] std::string cvt(const std::wstring &str)
{
	std::string out;
	if(!str.empty()) {
		out.resize(clamp(WideCharToMultiByte(CP_THREAD_ACP, 0, str.data(), static_cast<int>(str.size()), nullptr, 0, nullptr, nullptr)));
		out.resize(clamp(WideCharToMultiByte(
		    CP_THREAD_ACP, 0, str.data(), static_cast<int>(str.size()), out.data(), static_cast<int>(out.size()), nullptr, nullptr)));
	}
	return out;
}

[[nodiscard]] std::optional<std::string> getOptionalString(const INIReader &ini, const std::string &section, const std::string &name)
{
	if(auto v = ini.Get(section, name, {}); !v.empty()) {
		return {std::move(v)};
	}
	return std::nullopt;
}

[[nodiscard]] std::optional<std::wstring> getOptionalWString(const INIReader &ini, const std::string &section, const std::string &name)
{
	if(const auto v = getOptionalString(ini, section, name)) {
		return {cvt(*v)};
	}
	return std::nullopt;
}

[[nodiscard]] std::optional<UINT> getOptionalUInt(const INIReader &ini, const std::string &section, const std::string &name)
{
	if(const auto v = ini.GetInteger(section, name, -1); v >= 0) {
		return {static_cast<UINT>(v)};
	}
	return std::nullopt;
}

[[nodiscard]] std::optional<SIZE_T> getOptionalSizeT(const INIReader &ini, const std::string &section, const std::string &name)
{
	if(const auto v = ini.GetInteger(section, name, -1); v >= 0) {
		return {static_cast<SIZE_T>(v)};
	}
	return std::nullopt;
}

[[nodiscard]] std::string systemDirPath()
{
	char path[MAX_PATH]{};
	GetSystemDirectory(path, sizeof(path) - 1);
	return {path};
}

}  // namespace

Config::Config(const std::string &ini_file_path)
{
	INIReader ini{ini_file_path};
	if(!ini.ParseError()) {
		log_section.enable = ini.GetBoolean("log", "enable", false);
		override_section.only_igpu = ini.GetBoolean("override", "only_igpu", false);
		override_section.gpu_description = getOptionalWString(ini, "override", "gpu_description");
		override_section.gpu_vendor_id = getOptionalUInt(ini, "override", "gpu_vendor_id");
		override_section.gpu_device_id = getOptionalUInt(ini, "override", "gpu_device_id");
		override_section.gpu_dedicated_memory_size = getOptionalSizeT(ini, "override", "gpu_dedicated_memory_size");
		dxgi_section.enable_factory_proxy = ini.GetBoolean("dxgi", "enable_factory_proxy", true);
		dxgi_section.enable_adapter_proxy = ini.GetBoolean("dxgi", "enable_adapter_proxy", true);
		if(auto path = getOptionalString(ini, "dxgi", "dll_path")) {
			dxgi_section.dll_path = std::move(*path);
		}
	}
	if(dxgi_section.dll_path.empty()) {
		dxgi_section.dll_path = systemDirPath() + "\\dxgi.dll";
	}
}

std::string Config::toString() const
{
	std::ostringstream s;
	s << "[log]\n";
	s << "enable=" << (log_section.enable ? "true" : "false") << "\n";
	s << "[override]\n";
	s << "only_igpu=" << (override_section.only_igpu ? "true" : "false") << "\n";
	if(override_section.gpu_description) {
		s << "gpu_description=" << cvt(*override_section.gpu_description) << "\n";
	}
	if(override_section.gpu_vendor_id) {
		s << "gpu_vendor_id=0x" << std::hex << std::setw(4) << *override_section.gpu_vendor_id << "\n";
	}
	if(override_section.gpu_device_id) {
		s << "gpu_device_id=0x" << std::hex << std::setw(4) << *override_section.gpu_device_id << "\n";
	}
	if(override_section.gpu_dedicated_memory_size) {
		s << "gpu_dedicated_memory_size=" << std::dec << std::setw(0) << *override_section.gpu_dedicated_memory_size << "\n";
	}
	s << "[dxgi]\n";
	s << "dll_path=" << dxgi_section.dll_path << "\n";
	s << "enable_factory_proxy=" << (dxgi_section.enable_factory_proxy ? "true" : "false") << "\n";
	s << "enable_adapter_proxy=" << (dxgi_section.enable_adapter_proxy ? "true" : "false") << "\n";
	return std::move(s).str();
}

}  // namespace dxgi_proxy
