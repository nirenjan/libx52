/*
 * Virtual keyboard/mouse interface
 *
 * Copyright (C) 2026 Nirenjan Krishnan (nirenjan@nirenjan.org)
 *
 * SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
 */

/**
 * @file vkm.h
 * @brief Functions, structures and enumerations for the virtual
 * keyboard/mouse interface library (VKM).
 *
 * This file contains the type, enum and function prototypes for VKM.
 * These functions allow an application to inject keyboard/mouse events
 * into the host OS, as long as it has the necessary permissions.
 *
 * @author Nirenjan Krishnan (nirenjan@nirenjan.org)
 */
#ifndef VKM_H
#define VKM_H

#include <stdint.h>
#include <stdbool.h>

#ifndef VKM_API
# if defined(__GNUC__) && (__GNUC__ * 100 + __GNUC_MINOR__) >= 303
#  define VKM_API __attribute__((visibility("default")))
# elif defined(_WIN32)
#  define VKM_API __declspec(dllexport)
# else
#  define VKM_API
# endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Opaque structure used by the VKM framework
 */
struct vkm_context;

/**
 * @brief Virtual device context structure used by the VKM framework
 *
 * All VKM API functions require the application to pass in a pointer to
 * a valid context structure. A pointer can be obtained by calling
 * \ref vkm_init
 */
typedef struct vkm_context vkm_context;

/**
 * @brief Return type used by VKM API functions
 */
typedef int32_t vkm_result;

/**
 * @brief Feature identifiers for \ref vkm_feature_supported
 *
 * Bit flags describing optional VKM capabilities on the current platform.
 * Pass one enumerator at a time to \ref vkm_feature_supported.
 */
typedef enum {
    VKM_FEAT_MOUSE = (1 << 0), /**< Relative mouse move, buttons, and wheel */

    /** Full \ref vkm_keyboard_send key map (platform virtual keyboard) */
    VKM_FEAT_KEYBOARD = (1 << 1),

    /** Separate left/right logical modifiers in \ref vkm_keyboard_send */
    VKM_FEAT_KEYBOARD_MODIFIERS = (1 << 2),

    /* Additional flags may be added in the future */
} vkm_feature;

/**
 * @brief Error code list
 */
typedef enum {
    /** No error, indicates success */
    VKM_SUCCESS = 0,

    /** Unknown error, used as a catch-all error state */
    VKM_ERROR_UNKNOWN,

    /** Not started, must call vkm_start first */
    VKM_ERROR_NOT_READY,

    /** Out of memory */
    VKM_ERROR_OUT_OF_MEMORY,

    /** Invalid parameter(s) */
    VKM_ERROR_INVALID_PARAM,

    /** Not supported */
    VKM_ERROR_NOT_SUPPORTED,

    /** Unable to create virtual devices */
    VKM_ERROR_DEV_FAILURE,

    /** Unable to write event */
    VKM_ERROR_EVENT,

    /** No state change in the event, please retry */
    VKM_ERROR_NO_CHANGE,

    /* Maximum error states, do not use in external code*/
    VKM_ERROR_MAX,
} vkm_error_code;

/**
 * @brief Return a short description for a \ref vkm_result / \ref vkm_error_code value
 *
 * The returned pointer refers to static storage and must not be freed. For
 * unrecognized codes, the same static buffer may be overwritten by a later call.
 *
 * When native language support (NLS) is enabled at build time, these messages
 * are translated like \ref libx52_strerror. Bind the \c libx52 text domain and
 * set \c LC_MESSAGES as for other libx52 components.
 *
 * @param[in]   code    Value returned from a VKM API function
 *
 * @returns Pointer to a NUL-terminated description string
 */
VKM_API const char *vkm_strerror(vkm_result code);

/**
 * @brief Option list
 */
typedef enum {
    /**
     * @brief Set the high resolution scrolling behavior of the mouse
     *
     * This option must be passed a boolean which lets VKM know whether to
     * enable or disable high resolution scrolling.
     *
     * Defaults to false. When enabled together with \ref vkm_start,
     * \ref vkm_mouse_scroll emits high-resolution REL_*_HI_RES events (120 units
     * per step) in addition to discrete REL_WHEEL / REL_HWHEEL ticks.
     */
    VKM_OPT_HI_RES_SCROLL,

    /**
     * @brief Enable or disable horizontal scrolling of the mouse
     *
     * This option must be passed in a boolean which lets VKM know whether to
     * enable or disable horizontal scrolling. If horizontal scrolling is
     * disabled, then any requests to \ref vkm_mouse_scroll with
     * \ref VKM_MOUSE_SCROLL_LEFT or \ref VKM_MOUSE_SCROLL_RIGHT will return
     * \ref VKM_ERROR_INVALID_PARAM.
     *
     * Defaults to false.
     */
    VKM_OPT_HORIZONTAL_SCROLL,

    /**
     * @brief Set the virtual device name in the system.
     *
     * This option sets the name of the virtual input device in the system.
     * If not set, the virtual device will have a name determined by the
     * timestamp at which it was initialized.
     *
     * Only applicable on Linux.
     */
    VKM_OPT_DEVICE_NAME,

    /* Max number of options, do not use in external code */
    VKM_OPT_MAX
} vkm_option;

/**
 * @brief Button state
 */
typedef enum {
    /** Button is released */
    VKM_BUTTON_RELEASED,

    /** Button is pressed */
    VKM_BUTTON_PRESSED,

    /* Max number of button states, do not use in external code */
    VKM_BUTTON_STATE_MAX
} vkm_button_state;

/**
 * @brief Mouse button identifiers
 */
typedef enum {
    /** Mouse left button */
    VKM_MOUSE_BTN_LEFT,

    /** Mouse right button */
    VKM_MOUSE_BTN_RIGHT,

    /** Mouse middle button */
    VKM_MOUSE_BTN_MIDDLE,

    /* Max number of mouse buttons, do not use in external code */
    VKM_MOUSE_BTN_MAX
} vkm_mouse_button;

/**
 * @brief Scroll directions
 */
typedef enum {
    /** Scroll up */
    VKM_MOUSE_SCROLL_UP,

    /** Scroll down */
    VKM_MOUSE_SCROLL_DOWN,

    /** Scroll left (horizontal scrolling) */
    VKM_MOUSE_SCROLL_LEFT,

    /** Scroll right (horizontal scrolling) */
    VKM_MOUSE_SCROLL_RIGHT,

    /* Maximum number of scroll states, do not use in external code */
    VKM_MOUSE_SCROLL_MAX
} vkm_mouse_scroll_direction;

/**
 * @brief Physical key identifiers (USB HID keyboard/keypad page, usage page 0x07)
 *
 * Logical key codes for \ref vkm_keyboard_send. Each enumerator's numeric value is
 * the USB HID usage ID for that key (same encoding as the keyboard bitmap in HID
 * report descriptors, except for \c VKM_KEY_NONE). Letters use the HID ordering
 * (\c A=0x04 … \c Z=0x1D), not QWERTY row order. \c VKM_KEY_MAX is \c 0xE8, one
 * past the highest usage used here, so valid keys satisfy \c key < VKM_KEY_MAX.
 */
typedef enum {
    VKM_KEY_NONE = 0x00, /**< Sentinel: no key (e.g. modifier-only update) */

    VKM_KEY_A = 0x04, /**< \c A / a (HID Latin alphabet block) */
    VKM_KEY_B,        /**< \c B */
    VKM_KEY_C,        /**< \c C */
    VKM_KEY_D,        /**< \c D */
    VKM_KEY_E,        /**< \c E */
    VKM_KEY_F,        /**< \c F */
    VKM_KEY_G,        /**< \c G */
    VKM_KEY_H,        /**< \c H */
    VKM_KEY_I,        /**< \c I */
    VKM_KEY_J,        /**< \c J */
    VKM_KEY_K,        /**< \c K */
    VKM_KEY_L,        /**< \c L */
    VKM_KEY_M,        /**< \c M */
    VKM_KEY_N,        /**< \c N */
    VKM_KEY_O,        /**< \c O */
    VKM_KEY_P,        /**< \c P */
    VKM_KEY_Q,        /**< \c Q */
    VKM_KEY_R,        /**< \c R */
    VKM_KEY_S,        /**< \c S */
    VKM_KEY_T,        /**< \c T */
    VKM_KEY_U,        /**< \c U */
    VKM_KEY_V,        /**< \c V */
    VKM_KEY_W,        /**< \c W */
    VKM_KEY_X,        /**< \c X */
    VKM_KEY_Y,        /**< \c Y */
    VKM_KEY_Z,        /**< \c Z */

    VKM_KEY_1 = 0x1E, /**< \c 1 / ! */
    VKM_KEY_2,        /**< \c 2 / @ */
    VKM_KEY_3,        /**< \c 3 / # */
    VKM_KEY_4,        /**< \c 4 / $ */
    VKM_KEY_5,        /**< \c 5 / % */
    VKM_KEY_6,        /**< \c 6 / ^ */
    VKM_KEY_7,        /**< \c 7 / & */
    VKM_KEY_8,        /**< \c 8 / * */
    VKM_KEY_9,        /**< \c 9 / ( */
    VKM_KEY_0,        /**< \c 0 / ) */

    VKM_KEY_ENTER = 0x28,       /**< Return / Enter */
    VKM_KEY_ESCAPE,             /**< Escape */
    VKM_KEY_BACKSPACE,          /**< Delete / Backspace */
    VKM_KEY_TAB,                /**< Tab */
    VKM_KEY_SPACE,              /**< Space bar */
    VKM_KEY_MINUS,              /**< Minus / underscore */
    VKM_KEY_EQUAL,              /**< Equals / plus */
    VKM_KEY_LEFT_BRACKET,       /**< Left bracket / brace */
    VKM_KEY_RIGHT_BRACKET,      /**< Right bracket / brace */
    VKM_KEY_BACKSLASH,         /**< Backslash / pipe (US placement; JIS Yen) */
    VKM_KEY_NONUS_HASH,        /**< Non-US # / ~ (HID usage 0x32) */
    VKM_KEY_SEMICOLON,         /**< Semicolon / colon */
    VKM_KEY_APOSTROPHE,        /**< Apostrophe / quote */
    VKM_KEY_GRAVE_ACCENT,      /**< Grave accent / tilde */
    VKM_KEY_COMMA,             /**< Comma / less-than */
    VKM_KEY_PERIOD,            /**< Period / greater-than */
    VKM_KEY_SLASH,             /**< Slash / question */

    VKM_KEY_CAPS_LOCK = 0x39,   /**< Caps Lock */

    VKM_KEY_F1 = 0x3A, /**< F1 */
    VKM_KEY_F2,        /**< F2 */
    VKM_KEY_F3,        /**< F3 */
    VKM_KEY_F4,        /**< F4 */
    VKM_KEY_F5,        /**< F5 */
    VKM_KEY_F6,        /**< F6 */
    VKM_KEY_F7,        /**< F7 */
    VKM_KEY_F8,        /**< F8 */
    VKM_KEY_F9,        /**< F9 */
    VKM_KEY_F10,       /**< F10 */
    VKM_KEY_F11,       /**< F11 */
    VKM_KEY_F12,       /**< F12 */

    VKM_KEY_PRINT_SCREEN = 0x46, /**< Print Screen */
    VKM_KEY_SCROLL_LOCK,         /**< Scroll Lock */
    VKM_KEY_PAUSE,               /**< Pause / Break */

    VKM_KEY_INSERT,         /**< Insert */
    VKM_KEY_HOME,           /**< Home */
    VKM_KEY_PAGE_UP,        /**< Page Up */
    VKM_KEY_DELETE_FORWARD, /**< Delete (forward) */
    VKM_KEY_END,            /**< End */
    VKM_KEY_PAGE_DOWN,      /**< Page Down */

    VKM_KEY_RIGHT_ARROW, /**< Arrow right */
    VKM_KEY_LEFT_ARROW,  /**< Arrow left */
    VKM_KEY_DOWN_ARROW,  /**< Arrow down */
    VKM_KEY_UP_ARROW,    /**< Arrow up */

    VKM_KEY_KEYPAD_NUM_LOCK = 0x53, /**< Keypad Num Lock */
    VKM_KEY_KEYPAD_DIVIDE,          /**< Keypad \c / */
    VKM_KEY_KEYPAD_MULTIPLY,        /**< Keypad \c * */
    VKM_KEY_KEYPAD_MINUS,           /**< Keypad \c - */
    VKM_KEY_KEYPAD_PLUS,            /**< Keypad \c + */
    VKM_KEY_KEYPAD_ENTER,           /**< Keypad Enter */
    VKM_KEY_KEYPAD_1,               /**< Keypad \c 1 / End */
    VKM_KEY_KEYPAD_2,               /**< Keypad \c 2 / Down */
    VKM_KEY_KEYPAD_3,               /**< Keypad \c 3 / Page Down */
    VKM_KEY_KEYPAD_4,               /**< Keypad \c 4 / Left */
    VKM_KEY_KEYPAD_5,               /**< Keypad \c 5 */
    VKM_KEY_KEYPAD_6,               /**< Keypad \c 6 / Right */
    VKM_KEY_KEYPAD_7,               /**< Keypad \c 7 / Home */
    VKM_KEY_KEYPAD_8,               /**< Keypad \c 8 / Up */
    VKM_KEY_KEYPAD_9,               /**< Keypad \c 9 / Page Up */
    VKM_KEY_KEYPAD_0,               /**< Keypad \c 0 / Insert */
    VKM_KEY_KEYPAD_DECIMAL,        /**< Keypad decimal / Delete */
    VKM_KEY_INTL_BACKSLASH, /**< Non-US \c \\ / \c | (ISO key; HID 0x64) */
    VKM_KEY_APPLICATION,    /**< Application / Menu */

    VKM_KEY_KEYPAD_COMMA = 0x85, /**< Keypad comma (Braz. keypad, etc.) */

    VKM_KEY_LEFT_CTRL = 0xE0,  /**< Left Control */
    VKM_KEY_LEFT_SHIFT,        /**< Left Shift */
    VKM_KEY_LEFT_ALT,          /**< Left Alt */
    VKM_KEY_LEFT_GUI,          /**< Left GUI / Meta / Windows key */
    VKM_KEY_RIGHT_CTRL,        /**< Right Control */
    VKM_KEY_RIGHT_SHIFT,       /**< Right Shift */
    VKM_KEY_RIGHT_ALT,         /**< Right Alt / AltGr */
    VKM_KEY_RIGHT_GUI,         /**< Right GUI / Meta */

    VKM_KEY_MAX /**< One past highest usage; bounds checks and tables */
} vkm_key;

/**
 * @brief Modifier bitmask for \ref vkm_keyboard_send
 *
 * Left and right modifier keys use the same bit layout as the USB HID keyboard
 * modifier byte. Combine values with bitwise OR. These are separate from
 * physical modifier key events in \ref vkm_key.
 *
 * Convenience macros \c VKM_KEY_MOD_CTRL, \c VKM_KEY_MOD_SHIFT, \c VKM_KEY_MOD_ALT,
 * and \c VKM_KEY_MOD_GUI alias the left-hand modifiers only; use
 * \c VKM_KEY_MOD_L* / \c VKM_KEY_MOD_R* when a specific side is required.
 */
typedef enum {
    VKM_KEY_MOD_NONE = 0, /**< No modifiers */

    VKM_KEY_MOD_LCTRL = (1 << 0),  /**< Left Control (HID modifier byte bit 0) */
    VKM_KEY_MOD_LSHIFT = (1 << 1), /**< Left Shift (bit 1) */
    VKM_KEY_MOD_LALT = (1 << 2),   /**< Left Alt (bit 2) */
    VKM_KEY_MOD_LGUI = (1 << 3),   /**< Left GUI / Meta (bit 3) */
    VKM_KEY_MOD_RCTRL = (1 << 4),  /**< Right Control (bit 4) */
    VKM_KEY_MOD_RSHIFT = (1 << 5), /**< Right Shift (bit 5) */
    VKM_KEY_MOD_RALT = (1 << 6),   /**< Right Alt / AltGr (bit 6) */
    VKM_KEY_MOD_RGUI = (1 << 7),   /**< Right GUI / Meta (bit 7) */
} vkm_key_modifiers;

/** Convenience alias for \ref VKM_KEY_MOD_LCTRL */
#define VKM_KEY_MOD_CTRL VKM_KEY_MOD_LCTRL
/** Convenience alias for \ref VKM_KEY_MOD_LSHIFT */
#define VKM_KEY_MOD_SHIFT VKM_KEY_MOD_LSHIFT
/** Convenience alias for \ref VKM_KEY_MOD_LALT */
#define VKM_KEY_MOD_ALT VKM_KEY_MOD_LALT
/** Convenience alias for \ref VKM_KEY_MOD_LGUI */
#define VKM_KEY_MOD_GUI VKM_KEY_MOD_LGUI

/**
 * @brief Physical key action (press or release) for \ref vkm_keyboard_send
 *
 * Determines the \c value sent with the non-modifier \ref vkm_key (and, on
 * release, how the modifier mask is reconciled after the key event).
 */
typedef enum {
    VKM_KEY_STATE_RELEASED, /**< Key or button is up */
    VKM_KEY_STATE_PRESSED,  /**< Key or button is down */
    VKM_KEY_STATE_MAX       /**< Sentinel; do not use in application code */
} vkm_key_state;

/**
 * @brief Initialize the VKM library
 *
 * This function initializes the VKM library, sets up any internal data
 * structures to send input events, and returns a \ref vkm_context pointer
 * in the output parameter. All calls to VKM use the returned pointer to
 * inject keyboard/mouse events.
 *
 * @par Example
 * @code
 * vkm_result rc;
 * vkm_context *ctx;
 * rc = vkm_init(&ctx);
 * if (rc != LIBX52_SUCCESS) {
 *     // Error handling omitted for brevity
 * }
 * // Save ctx for use later
 * @endcode
 *
 * @param[out]  ctx     Pointer to a \ref vkm_context *. This function will
 * allocate a context and return the pointer to the context in this variable.
 *
 * @returns \ref vkm_error_code indicating status
 */
VKM_API vkm_result vkm_init(vkm_context **ctx);

/**
 * @brief Exit the VKM library and free up any resources used
 *
 * This function calls \ref vkm_reset, releases any resources allocated by
 * \ref vkm_init, and terminates the library. Using the freed context after
 * this returns is invalid and can cause errors.
 *
 * @param[in]   ctx     Context pointer
 */
VKM_API void vkm_exit(vkm_context *ctx);

/**
 * @brief Release all virtual keys and mouse buttons that are still down
 *
 * Synthesizes release events for any keys pressed through \ref vkm_keyboard_send,
 * clears the logical modifier mask from that API, and releases mouse buttons that
 * are still pressed. Internal bookkeeping is cleared even if the virtual device
 * is not ready (no events are written until \ref vkm_start succeeds).
 *
 * \ref vkm_exit calls this automatically before freeing the context.
 *
 * @param[in]   ctx     Context pointer
 *
 * @returns
 * - \ref VKM_SUCCESS on success or if there was nothing to release
 * - \ref VKM_ERROR_INVALID_PARAM on bad pointer
 * - \ref VKM_ERROR_EVENT if writing a release event failed
 */
VKM_API vkm_result vkm_reset(vkm_context *ctx);

/**
 * @brief Start any virtual keyboard/mouse devices on the platform
 *
 * This must be done before injecting any events, and after setting all
 * options through \ref vkm_set_option.
 *
 * @param[in]   ctx     Context pointer
 *
 * @returns
 * - \ref VKM_SUCCESS on successful start
 * - \ref VKM_ERROR_INVALID_PARAM on bad pointer
 * - \ref VKM_ERROR_UNKNOWN on other errors
 */
VKM_API vkm_result vkm_start(vkm_context *ctx);

/**
 * @brief check if VKM is started and ready
 *
 * @param[in]   ctx     Context pointer
 *
 * @returns boolean indicating if ready or not.
 */
VKM_API bool vkm_is_ready(vkm_context *ctx);

/**
 * @brief Check if VKM is supported on this platform
 *
 * On some platforms, there is no support yet for the virtual keyboard/mouse.
 * This function will return a boolean indicating if it is supported or not.
 *
 * @returns boolean indicating support.
 */
VKM_API bool vkm_platform_supported(void);

/**
 * @brief Check if a particular feature is enabled on this platform
 *
 * Features may be limited on a per-platform basis.
 *
 * @param[in]   feat    Feature identifier
 *
 * @returns boolean indicating if feature is supported or not.
 */
VKM_API bool vkm_feature_supported(vkm_feature feat);

/**
 * @brief Set an option flag for VKM.
 *
 * Option flags control the behavior of VKM. All options must be set before
 * calling \ref vkm_start.
 *
 * @param[in]   ctx     Context pointer
 * @param[in]   option  Which option to set
 * @param[in]   ...     Any required arguments for the specified option
 *
 * @returns
 * - \ref VKM_SUCCESS on success
 * - \ref VKM_ERROR_INVALID_PARAM if the option or arguments are invalid
 * - \ref VKM_ERROR_NOT_SUPPORTED if the option is valid but not supported on this platform
 */
VKM_API vkm_result vkm_set_option(vkm_context *ctx, vkm_option option, ...);

/**
 * @brief Move the mouse by the specified amount
 *
 * The move mouse takes in a delta of x and y coordinates that tell the system
 * to move the mouse by those relative numbers.
 *
 * @param[in]   ctx     Context pointer
 * @param[in]   dx      Delta by which to move the mouse in the horizontal axis
 * @param[in]   dy      Delta by which to move the mouse in the vertical axis
 *
 * @returns
 * - \ref VKM_SUCCESS on successful move
 * - \ref VKM_ERROR_UNKNOWN on a generic error
 * - \ref VKM_ERROR_NOT_SUPPORTED if the mouse move is not supported on this platform
 * - \ref VKM_ERROR_NOT_READY if VKM is not started
 */
VKM_API vkm_result vkm_mouse_move(vkm_context *ctx, int dx, int dy);

/**
 * @brief Click the mouse button
 *
 * Send a mouse button event, this may be either a button down or button up event.
 *
 * @param[in]   ctx     Context pointer
 * @param[in]   button  Button identifier
 * @param[in]   state   Button state (press or release)
 *
 * @returns
 * - \ref VKM_SUCCESS on successful move
 * - \ref VKM_ERROR_UNKNOWN on a generic error
 * - \ref VKM_ERROR_NOT_SUPPORTED if the mouse button click is not supported on this platform
 * - \ref VKM_ERROR_NOT_READY if VKM is not started
 */
VKM_API vkm_result vkm_mouse_click(vkm_context *ctx, vkm_mouse_button button, vkm_button_state state);

/**
 * @brief Scroll the mouse wheel
 *
 * Send a single scroll event to the mouse wheel (one detent in the chosen
 * direction). If \ref VKM_OPT_HI_RES_SCROLL was enabled before \ref vkm_start,
 * also emits REL_WHEEL_HI_RES / REL_HWHEEL_HI_RES using the standard 120
 * units per detent scale before the corresponding discrete REL_WHEEL /
 * REL_HWHEEL event.
 *
 * @param[in]   ctx     Context pointer
 * @param[in]   dir     Scroll direction
 *
 * @returns
 * - \ref VKM_SUCCESS on successful move
 * - \ref VKM_ERROR_UNKNOWN on a generic error
 * - \ref VKM_ERROR_INVALID_PARAM if horizontal scrolling is not enabled
 *   and \p dir is \ref VKM_MOUSE_SCROLL_LEFT or \ref VKM_MOUSE_SCROLL_RIGHT
 * - \ref VKM_ERROR_NOT_SUPPORTED if the mouse scrolling is not supported on this platform
 * - \ref VKM_ERROR_NOT_READY if VKM is not started
 */
VKM_API vkm_result vkm_mouse_scroll(vkm_context *ctx, vkm_mouse_scroll_direction dir);

/**
 * @brief Send a single keyboard event
 *
 * Send a single keyboard event to the OS. This will send a single key event,
 * with modifiers enabled (Ctrl, Shift, Alt, GUI).
 *
 * @param[in]   ctx         Context pointer
 * @param[in]   key         Key identifier
 * @param[in]   modifiers   Modifier keys to enable (Ctrl, Shift, Alt, GUI)
 * @param[in]   state       Key state (press or release)
 *
 * @returns
 * - \ref VKM_SUCCESS on successful send
 * - \ref VKM_ERROR_UNKNOWN on a generic error
 * - \ref VKM_ERROR_INVALID_PARAM if parameters are invalid
 * - \ref VKM_ERROR_NOT_SUPPORTED if the keyboard event is not enabled or
 *  supported on this platform
 * - \ref VKM_ERROR_NOT_READY if VKM is not started
 */
VKM_API vkm_result vkm_keyboard_send(vkm_context *ctx, vkm_key key, vkm_key_modifiers modifiers, vkm_key_state state);

/**
 * @brief Send a sync packet to the OS
 *
 * On some platforms, a sync packet is necessary for the previously injected
 * events to actually get reflected in the system. For platforms where this
 * is not needed, this is a noop.
 *
 * @param[in]   ctx     Context pointer
 *
 * @returns
 * - \ref VKM_SUCCESS on successful move
 * - \ref VKM_ERROR_UNKNOWN on a generic error
 * - \ref VKM_ERROR_INVALID_PARAM if parameters are invalid
 * - \ref VKM_ERROR_NOT_READY if VKM is not started
 */
VKM_API vkm_result vkm_sync(vkm_context *ctx);

#ifdef __cplusplus
}
#endif

#endif // !defined VKM_H
