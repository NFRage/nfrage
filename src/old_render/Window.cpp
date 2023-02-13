/*********************************************************************
* Copyright (C) Anton Kovalev (vertver), 2022-2023. All rights reserved.
* nfrage - engine code for NFRage project
**********************************************************************
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2.1 of the License, or (at your option) any later version.
* 
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
* 
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the Free 
* Software Foundation, Inc., 51 Franklin Street, Fifth Floor, 
* Boston, MA 02110-1301 USA
*****************************************************************/
#include "AppLayerPch.h"

#if 0
namespace nfr::window
{

	SDL_Window* sdl_window = nullptr;

bool Initialize()
{
	dbg::Log("Initializing \"Window\" module...");


    //
    if (SDL_Init(
        SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS | 
        SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC | SDL_INIT_GAMECONTROLLER | SDL_INIT_SENSOR 
    ) != 0) {
        nfr::dbg::Error("Unable to initialize SDL: %s", SDL_GetError());
        return false;
    }

    //
    sdl_window = SDL_CreateWindow(
        "Engine", 
        SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED, 
        SDL_DEFAULT_WINDOWSIZE_W, 
        SDL_DEFAULT_WINDOWSIZE_H, 
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );
    if (!sdl_window) {
        nfr::dbg::Error("SDL2 window creation failed: %s", SDL_GetError());
        return false;
    }

	return true;
}

void Destroy()
{
	dbg::Log("Destroying \"Window\" module...");

    //
    if (sdl_window) {
        SDL_DestroyWindow(sdl_window);
        sdl_window = nullptr;
    }

    //
    SDL_Quit();
}

void* GetHandle()
{
    return (void*)sdl_window;
}

bool ExecuteWindowEvents() 
{
    SDL_Event e;
    while (SDL_PollEvent(&e)) {

        nfr::app::imgui::ImGui_ImplSDL2_ProcessEvent(&e);

        switch (e.type) {
        case SDL_QUIT: 
            return false;

        case SDL_WINDOWEVENT:
            if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                int window_w, window_h;
                SDL_GetWindowSizeInPixels(sdl_window, &window_w, &window_h);
                if (window_w > 0 && window_h > 0) {
                    nfr::app::RenderResize(window_w, window_h);
                }
            }

        case SDL_KEYDOWN:
            /*engine::input::inputmng.event_key(
            e.key.keysym.scancode,
            e.key.state == SDL_PRESSED,
            (e.key.keysym.mod & KMOD_SHIFT) > 0,
            (e.key.keysym.mod & KMOD_CTRL) > 0,
            (e.key.keysym.mod & KMOD_ALT) > 0,
            (e.key.keysym.mod & KMOD_GUI) > 0
            );*/
            break;

        case SDL_KEYUP:
            /*engine::input::inputmng.event_key(
            e.key.keysym.scancode,
            e.key.state == SDL_PRESSED,
            (e.key.keysym.mod & KMOD_SHIFT) > 0,
            (e.key.keysym.mod & KMOD_CTRL) > 0,
            (e.key.keysym.mod & KMOD_ALT) > 0,
            (e.key.keysym.mod & KMOD_GUI) > 0
            );*/
            break;

        case  SDL_MOUSEBUTTONDOWN:
            /*if (e.button.button == SDL_BUTTON_LEFT) {
            key_down(&input->mouse.left);
            } else if (e.button.button == SDL_BUTTON_RIGHT) {
            key_down(&input->mouse.right);
            }*/
            break;

        case  SDL_MOUSEBUTTONUP:
            /*if (e.button.button == SDL_BUTTON_LEFT) {
            key_up(&input->mouse.left);
            } else if (e.button.button == SDL_BUTTON_RIGHT) {
            key_up(&input->mouse.right);
            }*/
            break;

        case SDL_MOUSEMOTION:
            /*input->mouse.wnd.x = e.motion.x;
            input->mouse.wnd.y = e.motion.y;
            input->mouse.rel.x = e.motion.xrel;
            input->mouse.rel.y = e.motion.yrel;*/
            break;

        case SDL_MOUSEWHEEL:
            /*input->mouse.scroll.x = e.wheel.x;
            input->mouse.scroll.y = e.wheel.y;*/
            break;
        
        default:
            break;
        }
    }

    return true;
}

}
#endif