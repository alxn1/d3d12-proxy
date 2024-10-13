#pragma once

namespace dxgi_proxy {

class Config final
{
public:
	inline static const std::string sIniFilePath{"dxgi_proxy.ini"};

	struct Log final
	{
		bool enable{false};
	};

	struct Override final
	{
		std::optional<std::wstring> gpu_description;
		std::optional<UINT> gpu_vendor_id;
		std::optional<UINT> gpu_device_id;
		std::optional<SIZE_T> gpu_dedicated_memory_size;
	};

	struct DXGI final
	{
		std::string dll_path;
	};

	explicit Config(const std::string &ini_file_path = sIniFilePath);

	[[nodiscard]] const Log &logSection() const noexcept { return log_section; }
	[[nodiscard]] const DXGI &dxgiSection() const noexcept { return dxgi_section; }
	[[nodiscard]] const Override &overrideSection() const noexcept { return override_section; }

	[[nodiscard]] std::string toString() const;

private:
	Log log_section;
	Override override_section;
	DXGI dxgi_section;
};

}  // namespace dxgi_proxy
