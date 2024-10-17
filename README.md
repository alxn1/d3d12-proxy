# d3d12-proxy
Graphics card spoofing library for DirectX;
May be used for fix UE5 Lumen GI bug on integrated GPUs:
UE5 do not allow 3D textures on these GPUs
(if dedicated GPU memory is zero card treated as integrated)
and Lumen GI automatically disabled in this case.
This proxy allows dedicated memory size spoofing force UE5 treat GPU as dedicated.

## Installation
* To use it just place `dxgi.dll` and `dxgi_proxy.ini` next to executable
* Edit `dxgi_proxy.ini` if needed
