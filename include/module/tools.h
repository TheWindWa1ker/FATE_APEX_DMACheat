#include "public.h"




extern bool LoadTextureFromFile(const char* filename, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height);
extern void CreateRenderTarget();
extern void CleanupRenderTarget();
extern void CleanupDeviceD3D();
extern bool CreateDeviceD3D(HWND hWnd);