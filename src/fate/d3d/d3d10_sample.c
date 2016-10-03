/* 
 * This file is part of F.A.T.E - the Fight And Travel game Engine.
 * Copyright (c) 2015-2016 Yoan Lecoq (yoanlecoq.io at gmail.com)
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from the
 * use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 
 * 1. The origin of this software must not be misrepresented;
 *    you must not claim that you wrote the original software. 
 *    If you use this software in a product, an acknowledgment in the product
 *    documentation would be appreciated but is not required.
 * 
 * 2. Altered source versions must be plainly marked as such, 
 *    and must not be misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 *
 *
 * Contributors :
 * - Please make sure your indentation is set to 4 spaces;
 * - Please avoid exceeding 80 character columns.
 *
 */

/*! \file fate/d3d/d3d10_sample.c
 *  \brief Direct3D 10 sample
 *
 * TODO
 */

/*
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <fate/dbg.h>
#include <SFML/Window.h>
#include <fate/defs.h>
#include <fate/d3d/defs.h>

#ifdef FE_WINDOWS
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
                   PSTR pCmdLine, int nCmdShow)
#else
int main(int argc, char *argv[])
#endif
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

    fe_dbg_assert(dev && swap_chain);

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
        
        //DXCALLV(dev, ClearDepthStencilView, mDepthStencilView, 
        //        D3D10_CLEAR_DEPTH | D3D10_CLEAR_STENCIL, 1.0f, 0);
        
        // Render things here !
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

*/
