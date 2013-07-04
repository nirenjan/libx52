Key mappings for X52 Pro
========================

# Key map table

One simple technique to map the individual keys is as follows:

| Key   | Mode 1   | Mode 2   | Mode 3   | Mode 1 S | Mode 2 S | Mode 3 S |
| ----- | -------- | -------- | -------- | -------- | -------- | -------- |
| Trigg | L Ctrl   | `<- M1`  | `<- M1`  | `<- M1S` | `<- M2S` | `<- M3S` |
| FIRE  | SPACE    | `<- M1`  | `<- M1`  | `<- M1S` | `<- M2S` | `<- M3S` |
| ...   |          |          |          |          |          |          |
| Pinky | -SHIFT-  | -SHIFT-  | -SHIFT-  | -SHIFT-  | -SHIFT-  | -SHIFT-  |
| D     | Tab      | `<- M1`  | `<- M1`  | `<- M1S` | `<- M2S` | `<- M3S` |
| E     | Joy E    | `<- M1`  | `<- M1`  | `<- M1S` | `<- M2S` | `<- M3S` |

The `<- M*` mappings indicate that the key map in the corresponding mode is the
same as in the linked mode. By default, the keys in modes 2 and 3 are linked
with the keymap in mode 1, and the keys in the +shift modes are linked with the
keymap in their primary mode, i.e., mode 1 + shift keymaps are linked with mode
1 keymaps, mode 2 + shift keymaps with mode 2, and mode 3 + shift keymaps with
mode 3.

Also, it is not necessary to have a specific key bound to each button, instead
it can behave as the same button being pressed, and the application can take
action accordingly.
 
# Shift Keys

The key mapper software for Windows allows the user to map any button to a
keystroke or a set of keystrokes. The keystroke sent is dependent on the mode
and if a shift key is pressed. Although the mapper does allow creation of
several shift keys, this goes to a point of diminishing returns.

The application design therefore works on the principle of allowing a single
shift key, mapped to the Pinky switch on the stick. The user can choose to
disable the shift mode, reverting to only the mode selector to select the key
maps.

One additional possibility is to have the shift button latched, i.e., the first
press and release turns on the shift mode, and the next press and release turns
it off. Compare this with the unlatched case where the user must press and hold
the shift button to trigger the shift mode. By default, the shift key is
unlatched, although a user can enable it on a per-profile basis.

Finally, when the shift mode is entered, this must be indicated by sending the
control message to turn on the SHIFT indicator in the MFD. Similarly, when the
shift mode is exited, the application must turn off the SHIFT indicator.

# Clutch button

The clutch feature is an optional enhancement to allow switching of profiles on
the fly, without having to exit the game the user is in. This button (marked
`i`) reports as a regular button, but the Windows driver allows the user to
select it as a clutch. The clutch can also be latched.

When the clutch button is enabled, entering clutch mode should be signaled by
blinking the clutch and POV hat LEDs. There is a specific message to enable
and disable the blinking. In clutch mode, the hat can be used to select
different profiles displayed on the MFD screen by going in the north and south
directions. The current profile can be cleared by moving the hat west, and the
selected profile can be enabled by moving the hat east.

When in clutch mode, the other buttons do not send their mappings to the
applicaton, instead, they are captured by the driver and the driver application
will display the current mapping on the MFD. The mode selector however, will
work as expected, by changing the display to highlight the current mode. The
application will also determine the current mapping by examining the mode.

Clutch mode can only be exited by means of the clutch button. If the clutch
button is disabled, then the clutch button behaves as any normal button and can
be mapped to a keypress.
