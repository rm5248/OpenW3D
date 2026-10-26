/*
**	Command & Conquer Renegade(tm)
**	Copyright 2026 OpenW3D Developers
**
**	This program is free software: you can redistribute it and/or modify
**	it under the terms of the GNU General Public License as published by
**	the Free Software Foundation, either version 3 of the License, or
**	(at your option) any later version.
**
**	This program is distributed in the hope that it will be useful,
**	but WITHOUT ANY WARRANTY; without even the implied warranty of
**	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**	GNU General Public License for more details.
**
**	You should have received a copy of the GNU General Public License
**	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#pragma once

#if defined(OPENW3D_WIN32)
#include <dinput.h>
#elif defined(OPENW3D_SDL3)
#include <SDL3/SDL_keycode.h>

// Map SDL keys to their directinput counterparts until we only depend on SDL
#define DIK_F1 SDLK_F1
#define DIK_F2 SDLK_F2
#define DIK_F3 SDLK_F3
#define DIK_F4 SDLK_F4
#define DIK_F5 SDLK_F5
#define DIK_F6 SDLK_F6
#define DIK_F7 SDLK_F7
#define DIK_F8 SDLK_F8
#define DIK_F9 SDLK_F9
#define DIK_F10 SDLK_F10
#define DIK_F11 SDLK_F11
#define DIK_F12 SDLK_F12
#define DIK_0 SDLK_0
#define DIK_1 SDLK_1
#define DIK_2 SDLK_2
#define DIK_3 SDLK_3
#define DIK_4 SDLK_4
#define DIK_5 SDLK_5
#define DIK_6 SDLK_6
#define DIK_7 SDLK_7
#define DIK_8 SDLK_8
#define DIK_9 SDLK_9
#define DIK_A SDLK_A
#define DIK_B SDLK_B
#define DIK_C SDLK_C
#define DIK_D SDLK_D
#define DIK_E SDLK_E
#define DIK_F SDLK_F
#define DIK_G SDLK_G
#define DIK_H SDLK_H
#define DIK_I SDLK_I
#define DIK_J SDLK_J
#define DIK_K SDLK_K
#define DIK_L SDLK_L
#define DIK_M SDLK_M
#define DIK_N SDLK_N
#define DIK_O SDLK_O
#define DIK_P SDLK_P
#define DIK_Q SDLK_Q
#define DIK_R SDLK_R
#define DIK_S SDLK_S
#define DIK_T SDLK_T
#define DIK_U SDLK_U
#define DIK_V SDLK_V
#define DIK_W SDLK_W
#define DIK_X SDLK_X
#define DIK_Y SDLK_Y
#define DIK_Z SDLK_Z
#define DIK_MINUS SDLK_MINUS
#define DIK_EQUALS SDLK_EQUALS
#define DIK_BACK SDLK_BACKSPACE
#define DIK_TAB SDLK_TAB
#define DIK_LBRACKET SDLK_LEFTBRACKET
#define DIK_RBRACKET SDLK_RIGHTBRACKET
#define DIK_RETURN SDLK_RETURN
#define DIK_SEMICOLON SDLK_SEMICOLON
#define DIK_APOSTROPHE SDLK_APOSTROPHE
#define DIK_GRAVE SDLK_GRAVE
#define DIK_BACKSLASH SDLK_BACKSLASH
#define DIK_COMMA SDLK_COMMA
#define DIK_PERIOD SDLK_PERIOD
#define DIK_SLASH SDLK_SLASH
#define DIK_SPACE SDLK_SPACE
#define DIK_CAPITAL SDLK_CAPSLOCK
#define DIK_NUMLOCK SDLK_NUMLOCKCLEAR
#define DIK_SCROLL SDLK_SCROLLLOCK
#define DIK_ESCAPE SDLK_ESCAPE
#define DIK_NUMPAD0 SDLK_KP_0
#define DIK_NUMPAD1 SDLK_KP_1
#define DIK_NUMPAD2 SDLK_KP_2
#define DIK_NUMPAD3 SDLK_KP_3
#define DIK_NUMPAD4 SDLK_KP_4
#define DIK_NUMPAD5 SDLK_KP_5
#define DIK_NUMPAD6 SDLK_KP_6
#define DIK_NUMPAD7 SDLK_KP_7
#define DIK_NUMPAD8 SDLK_KP_8
#define DIK_NUMPAD9 SDLK_KP_9
#define DIK_SUBTRACT SDLK_KP_MINUS
#define DIK_MULTIPLY SDLK_KP_MULTIPLY
#define DIK_ADD SDLK_KP_PLUS
#define DIK_DECIMAL SDLK_KP_PERIOD
#define DIK_NUMPADENTER SDLK_KP_ENTER
#define DIK_DIVIDE SDLK_KP_DIVIDE
#define DIK_HOME SDLK_HOME
#define DIK_PRIOR SDLK_PAGEUP
#define DIK_END SDLK_END
#define DIK_NEXT SDLK_PAGEDOWN
#define DIK_INSERT SDLK_INSERT
#define DIK_DELETE SDLK_DELETE
#define DIK_UP SDLK_UP
#define DIK_DOWN SDLK_DOWN
#define DIK_LEFT SDLK_LEFT
#define DIK_RIGHT SDLK_RIGHT
#define DIK_SYSRQ SDLK_SYSREQ
#define DIK_LSHIFT SDLK_LSHIFT
#define DIK_RSHIFT SDLK_RSHIFT
#define DIK_LALT SDLK_LALT
#define DIK_RALT SDLK_RALT
#define DIK_LWIN SDLK_LMETA
#define DIK_RWIN SDLK_RMETA
#define DIK_LMENU SDLK_MENU
#define DIK_RMENU SDLK_MENU

// these keys are modifiers in SDL, not keys as in directinput
#define DIK_LCONTROL SDL_KMOD_LCTRL
#define DIK_RCONTROL SDL_KMOD_RCTRL
// #define DIK_CONTROL SDL_KMOD_CTRL
// #define DIK_SHIFT SDL_KMOD_SHIFT
// #define DIK_ALT SDL_KMOD_ALT
// #define DIK_WIN (SDLK_LMETA | SDLK_RMETA)
#define DIK_APPS SDLK_UNKNOWN
#endif