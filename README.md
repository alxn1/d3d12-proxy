# d3d12-proxy
Graphics card spoofing library for DirectX
Maybe used for fix UE5 Lumen GI bug on integrated GPUs:
UE5 do not allow 3D textures on these GPU type
(if dedicated GPU memory is zero card treated as integrated)
and Lumen automatically disabled in this case.
This proxy allows dedicated memory size spoofing and Lumen GI
is not automatically disabled on engine start.

WARNING: tested only on AMD integrated card (680m) and Silent Hill 2 Remake

## Installation
* To use it just place `dxgi.dll` and `dxgi_proxy.ini` next to executable
* Edit `dxgi_proxy.ini` if needed
