#ifndef OS_KEYMAP_H
#define OS_KEYMAP_H

#define KEY_ENTER 0x1C
#define KEY_BS    0x0e /* Back Space */
#define KB_SHIFT_L 1
#define KB_SHIFT_R 2
#define KB_RELEASED 0x80

#define KEY_SHIFT_L 0x2a
#define KEY_SHIFT_R 0x36
#define KEY_TAB     0x0f
#define KEY_SHIFT_RL KEY_SHIFT_L | KB_RELEASED
#define KEY_SHIFT_RR KEY_SHIFT_R | KB_RELEASED

#define KEY_CAPSLOCK   0x3A
#define KEY_CAPSLOCK_R KEY_CAPSLOCK | KB_RELEASED

#define IS_BREAK_CODE(k) (k & 0x80)

#endif /* OS_KEYMAP_H */
