#pragma once
#ifndef XNE_DEVICE_IMPL
#define XNE_DEVICE_IMPL

#define XNE_TARGET_OPENGL

#include "core/core.h"
#include "core/math.h"

#include <memory.h>
#include <string.h>

#ifndef xne_SkipIfNotFocus
    #define xne_skip_if_not_focus() { if(!xne_is_focus()) continue; }
#endif

typedef enum xne_Keys {
    XNE_KEY_UNKNOWN      = 0,
    XNE_KEY_SPACE        = 32,
    XNE_KEY_MENU         = 34, 
    XNE_KEY_APOSTROPHE   = 39,
    XNE_KEY_COMMA        = 44, 
    XNE_KEY_MINUS        = 45,  
    XNE_KEY_PERIOD       = 46, 
    XNE_KEY_SLASH        = 47, 
    XNE_KEY_0            = 48,
    XNE_KEY_1            = 49,
    XNE_KEY_2            = 50,
    XNE_KEY_3            = 51,
    XNE_KEY_4            = 52,
    XNE_KEY_5            = 53,
    XNE_KEY_6            = 54,
    XNE_KEY_7            = 55,
    XNE_KEY_8            = 56,
    XNE_KEY_9            = 57,
    XNE_KEY_SEMICOLON    = 59,
    XNE_KEY_EQUAL        = 61,  
    XNE_KEY_A            = 65,
    XNE_KEY_B            = 66,
    XNE_KEY_C            = 67,
    XNE_KEY_D            = 68,
    XNE_KEY_E            = 69,
    XNE_KEY_F            = 70,
    XNE_KEY_G            = 71,
    XNE_KEY_H            = 72,
    XNE_KEY_I            = 73,
    XNE_KEY_J            = 74,
    XNE_KEY_K            = 75,
    XNE_KEY_L            = 76,
    XNE_KEY_M            = 77,
    XNE_KEY_N            = 78,
    XNE_KEY_O            = 79,
    XNE_KEY_P            = 80,
    XNE_KEY_Q            = 81,
    XNE_KEY_R            = 82,
    XNE_KEY_S            = 83,
    XNE_KEY_T            = 84,
    XNE_KEY_U            = 85,
    XNE_KEY_V            = 86,
    XNE_KEY_W            = 87,
    XNE_KEY_X            = 88,
    XNE_KEY_Y            = 89,
    XNE_KEY_Z            = 90,
    XNE_KEY_LEFT_BRACKET = 91, 
    XNE_KEY_BACKSLASH    = 92,  
    XNE_KEY_RIGHT_BRACKET= 93, 
    XNE_KEY_GRAVE_ACCENT = 96,
    XNE_KEY_WORLD_1      = 161, 
    XNE_KEY_WORLD_2      = 162, 
    XNE_KEY_ESCAPE       = 256,
    XNE_KEY_ENTER        = 257,
    XNE_KEY_TAB          = 258,
    XNE_KEY_BACKSPACE    = 259,
    XNE_KEY_INSERT       = 260,
    XNE_KEY_DELETE       = 261,
    XNE_KEY_RIGHT        = 262,
    XNE_KEY_LEFT         = 263,
    XNE_KEY_DOWN         = 264,
    XNE_KEY_UP           = 265,
    XNE_KEY_PAGE_UP      = 266,
    XNE_KEY_PAGE_DOWN    = 267,
    XNE_KEY_HOME         = 268,
    XNE_KEY_END          = 269,
    XNE_KEY_CAPS_LOCK    = 280,
    XNE_KEY_SCROLL_LOCK  = 281,
    XNE_KEY_NUM_LOCK     = 282,
    XNE_KEY_PRINT_SCREEN = 283,
    XNE_KEY_PAUSE        = 284,
    XNE_KEY_F1           = 290,
    XNE_KEY_F2           = 291,
    XNE_KEY_F3           = 292,
    XNE_KEY_F4           = 293,
    XNE_KEY_F5           = 294,
    XNE_KEY_F6           = 295,
    XNE_KEY_F7           = 296,
    XNE_KEY_F8           = 297,
    XNE_KEY_F9           = 298,
    XNE_KEY_F10          = 299,
    XNE_KEY_F11          = 300,
    XNE_KEY_F12          = 301,
    XNE_KEY_F13          = 302,
    XNE_KEY_F14          = 303,
    XNE_KEY_F15          = 304,
    XNE_KEY_F16          = 305,
    XNE_KEY_F17          = 306,
    XNE_KEY_F18          = 307,
    XNE_KEY_F19          = 308,
    XNE_KEY_F20          = 309,
    XNE_KEY_F21          = 310,
    XNE_KEY_F22          = 311,
    XNE_KEY_F23          = 312,
    XNE_KEY_F24          = 313,
    XNE_KEY_F25          = 314,
    XNE_KEY_KP_0         = 320,
    XNE_KEY_KP_1         = 321,
    XNE_KEY_KP_2         = 322,
    XNE_KEY_KP_3         = 323,
    XNE_KEY_KP_4         = 324,
    XNE_KEY_KP_5         = 325,
    XNE_KEY_KP_6         = 326,
    XNE_KEY_KP_7         = 327,
    XNE_KEY_KP_8         = 328,
    XNE_KEY_KP_9         = 329,
    XNE_KEY_KP_DECIMAL   = 330,
    XNE_KEY_KP_DIVIDE    = 331,
    XNE_KEY_KP_MULTIPLY  = 332,
    XNE_KEY_KP_SUBTRACT  = 333,
    XNE_KEY_KP_ADD       = 334,
    XNE_KEY_KP_ENTER     = 335,
    XNE_KEY_KP_EQUAL     = 336,
    XNE_KEY_LEFT_SHIFT   = 340,
    XNE_KEY_LEFT_CONTROL = 341,
    XNE_KEY_LEFT_ALT     = 342,
    XNE_KEY_LEFT_SUPER   = 343,
    XNE_KEY_RIGHT_SHIFT  = 344,
    XNE_KEY_RIGHT_CONTROL= 345,
    XNE_KEY_RIGHT_ALT    = 346,
    XNE_KEY_RIGHT_SUPER  = 347,
} xne_Keys_t;

typedef enum xne_MouseButtons {
    XNE_MOUSE_BUTTON_1      = 0,
    XNE_MOUSE_BUTTON_2      = 1,
    XNE_MOUSE_BUTTON_3      = 2,
    XNE_MOUSE_BUTTON_4      = 3,
    XNE_MOUSE_BUTTON_5      = 4,
    XNE_MOUSE_BUTTON_6      = 5,
    XNE_MOUSE_BUTTON_7      = 6,
    XNE_MOUSE_BUTTON_8      = 7,
    XNE_MOUSE_BUTTON_LAST   = XNE_MOUSE_BUTTON_8,
    XNE_MOUSE_BUTTON_LEFT   = XNE_MOUSE_BUTTON_1,
    XNE_MOUSE_BUTTON_RIGHT  = XNE_MOUSE_BUTTON_2,
    XNE_MOUSE_BUTTON_MIDDLE = XNE_MOUSE_BUTTON_3,
} xne_MouseButtons_t;

typedef enum xne_DeviceFlags {
    XNE_WINDOW_NONE = 0x000,
    XNE_WINDOW_FULLSCREEN = 0x001,
    XNE_WINDOW_RESIZABLE = 0x002,
    XNE_WINDOW_VISIBLE = 0x004,
    XNE_WINDOW_DECORATED = 0x008,
    XNE_WINDOW_FOCUSED = 0x010,
    XNE_WINDOW_AUTO_ICONIFY = 0x020,
    XNE_WINDOW_FLOATING = 0x040,
    XNE_WINDOW_MAXIMIZED = 0x080,
    XNE_WINDOW_CENTER_CUSOR = 0x100,
    XNE_WINDOW_FOCUS_ON_SHOW = 0x200,
    XNE_WINDOW_SCALE_TO_MONITOR = 0x400,
    XNE_WINDOW_DEFAULT = (XNE_WINDOW_RESIZABLE & XNE_WINDOW_DECORATED & XNE_WINDOW_AUTO_ICONIFY & XNE_WINDOW_CENTER_CUSOR)
} xne_DeviceFlags_t;

typedef struct xne_Device {
    void* handle;
    char* title;

    xne_vec2i client_position;
    xne_vec2i client_size;
    xne_vec2i framebuffer_size;
} xne_Device_t;

void xne_create_device(xne_Device_t* device, const int width, const int height, const char* title, uint32_t flags);
void xne_poll_events(xne_Device_t* device);
void xne_swap_buffers(xne_Device_t* device);
void xne_set_device_title(xne_Device_t* device, const char* title);
void xne_set_device_client_size(xne_Device_t* device, const int width, const int height);
void xne_set_device_client_position(xne_Device_t* device, const int x, const int y);
void xne_destroy_device(xne_Device_t* device);

/*
    Check if xne is finished.
    @return Returns 'true' if the application should close.
*/
bool xne_is_done(void);

/*
    Check if xne if focused.
    @return Returns 'true' if window is focused.
*/
bool xne_is_focus(void);

/*
    Get time since the application is running.
*/
double xne_get_time(void);

/*
    Get milliseconds time since 01/01/1970 in UTC.
*/
size_t xne_get_millis_time(void);

/*
    Ask xne to quit.
*/
void xne_done(void);

char xne_key_down(xne_Keys_t);
char xne_key_pressed(xne_Keys_t);
char xne_key_released(xne_Keys_t);

char xne_mouse_button_down(xne_MouseButtons_t);
char xne_mouse_button_pressed(xne_MouseButtons_t);
char xne_mouse_button_released(xne_MouseButtons_t);

void xne_get_mouse_position(int*, int*);
void xne_get_mouse_delta_position(float*, float*);
int xne_get_mouse_scroll();

#endif