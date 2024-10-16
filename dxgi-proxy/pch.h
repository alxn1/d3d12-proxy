// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#pragma once

#define _CRT_SECURE_NO_WARNINGS  // for INIReader.h
#define DXGI_P_API STDMETHODCALLTYPE

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <unknwn.h>
#include <dxgi1_6.h>

#include <algorithm>
#include <atomic>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <memory>
#include <mutex>
#include <optional>
#include <sstream>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>

#include <stdarg.h>

#include "inireader/INIReader.h"

#include "dxgi_proxy_conf.h"
#include "dxgi_proxy_log.h"
#include "dxgi_proxy_com_ptr.h"
#include "dxgi_proxy_com_object_chain.h"
#include "dxgi_proxy_object.h"
#include "dxgi_proxy_adapter.h"
#include "dxgi_proxy_factory.h"
#include "dxgi_proxy.h"
