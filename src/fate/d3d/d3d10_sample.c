#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <SFML/Window.h>
#include <fate/d3d/defs.h>

int main(int argc, char *argv[])
{
    sfVideoMode vm = sfVideoMode_getDesktopMode();
    vm.width = 640;
    vm.height = 480;
    sfWindow* window = sfWindow_create(vm, "D3D10", sfDefaultStyle, 0);
	HWND hwnd = sfWindow_getSystemHandle(window);

	HGLRC glctx = wglGetCurrentContext();
	wglDeleteContext(glctx);

	ID3D10Device *dev;
	IDXGISwapChain *swap_chain;
	ID3D10RenderTargetView *render_target;
	ID3D10Texture2D *back_buffer;

	DXGI_SWAP_CHAIN_DESC sd;
	D3D10_VIEWPORT vp;

	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = 640;
	sd.BufferDesc.Height = 480;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hwnd;
	sd.SampleDesc.Count = 8;
	sd.SampleDesc.Quality = 16;
	sd.Windowed = TRUE;

	D3D10CreateDeviceAndSwapChain(0, D3D10_DRIVER_TYPE_HARDWARE,
			0, 0, D3D10_SDK_VERSION, &sd, &swap_chain, &dev);

	assert(dev && swap_chain);

	// Create a render target view.
	DXCALLV(swap_chain, GetBuffer, 0, &IID_ID3D10Texture2D, &back_buffer);
	DXCALLV(dev, CreateRenderTargetView, (ID3D10Resource*) back_buffer,
			0, &render_target);
	DXCALLV(dev, OMSetRenderTargets, 1, &render_target, 0);

	// Set up the viewport.
	vp.Width = 640;
	vp.Height = 480;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	DXCALLV(dev, RSSetViewports, 1, &vp);

	const float CORNFLOWER[4] = { 0.392157f, 0.584314f, 0.929412f, 1 };
	sfClock *refresh_rate_clock = sfClock_create();
	bool vsync = false;

    while(sfWindow_isOpen(window)) {

		if(!vsync)
			sfClock_restart(refresh_rate_clock);
		
		DXCALLV(dev, ClearRenderTargetView, render_target, CORNFLOWER);
		// normally we'd render stuff here...
		DXCALLV(swap_chain, Present, 0, 0);

		if (!vsync) {
			sfInt64 el_time = sfClock_getElapsedTime(refresh_rate_clock).microseconds;
			if (el_time < 1000000.0f / 60.0f) {
				sfTime zzzz = { 1000000.0f / 60.0f - el_time };
				sfSleep(zzzz);
			}
		}

        sfEvent event;
		while (sfWindow_pollEvent(window, &event)) {
			if (event.type == sfEvtClosed)
				sfWindow_close(window);
		}
    }

	if (dev) {
		DXCALL(dev, ClearState);
		DXCALL(dev, Release);
		DXCALL(render_target, Release);
		DXCALL(swap_chain, Release);
		DXCALL(back_buffer, Release);
	}

	sfClock_destroy(refresh_rate_clock);
    sfWindow_destroy(window);
    return EXIT_SUCCESS;
}

