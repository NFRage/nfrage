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
#include "main_pch.h"

//SDL_Window* sdl_window = nullptr;

int SDL_main(int argc, char* argv[]) {
    if (!nfr::engine::Initialize(argv[0])) {
        return -1;
    }

    nfr::engine::Loop();

    nfr::engine::Destroy();
    return 0;
}

// fix sdl on console exe
#if defined(SDL_h_)
    #if defined(_MSC_VER) && defined(_WIN32)
    //  -DWIN32CONSOLE=1 to have console
    //  -DWIN32CONSOLE=0 to hide console
    //  -DWIN32WINMAIN=1 to start from WinMain()
    //  -DWIN32WINMAIN=0 to start from main()
    #if WIN32WINMAIN
    #else
    
    #endif

    #endif
#endif

#ifdef main
#undef main
#endif

int main(int argc, char* argv[]) {
   //SDL_SetMainReady();
   return SDL_main(argc, argv);
}