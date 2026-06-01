#ifndef __cmd_target_h__
#define __cmd_target_h__

#include <map>

typedef int (*mouse_callback)(int x, int y, unsigned __int64 extra);
typedef int (*kbd_callback)(int msg_id, int keycode);
typedef int (*cmd_callback)(int cmd);
typedef std::pair<int, int> callback_id_t; // (message_id, command_id)

#define ON_COMMAND(obj, cmd_id, func) \
    btm::command_target::register_command_func(obj, cmd_id, static_cast<int (btm::command_target::*)(int)>(&func))

#define REGISTER_CMD_CALLBACK(cmd_id, func) \
    btm::callback_registry::register_callback(callback_id_t(WM_COMMAND,cmd_id), { .type = btm::callback_reg::callback_type::menu_cmd, .callback_func = { .cmd_cb = func } })
#define REGISTER_MOUSE_CALLBACK(msg_id, func) \
    btm::callback_registry::register_callback(callback_id_t(msg_id,0), { .type = btm::callback_reg::callback_type::mouse_msg, .callback_func = { .mouse_cb = func } })
#define REGISTER_KBD_CALLBACK(msg_id, func) \
    btm::callback_registry::register_callback(callback_id_t(msg_id,0), { .type = btm::callback_reg::callback_type::kbd_msg, .callback_func = { .key_cb = func } })

namespace btm {
    // automatically handle menu commands and messages by registering member functions of command_target-derived classes, 
    // which allows for better organization of command handling logic within the relevant classes, 
    // while still providing the flexibility to use standalone callbacks when needed
    class command_target;
    struct command_reg {
        int (command_target::*command_func)(int);
        command_target* obj;
    };
    typedef std::map<int, command_reg> command_map_t;
    class command_target {
        static command_map_t command_map;
    public:
        command_target() {}
        virtual ~command_target() {}

        static int register_command_func(command_target* obj, int cmd_id, int (command_target::* func)(int)) {
            command_map[cmd_id] = { func, obj };
            return 0;
        }

        static int onCommand(int cmd) {
            auto it = command_map.find(cmd);
            if (it != command_map.end()) {
                const command_reg& reg = it->second;
                return (reg.obj->*(reg.command_func))(cmd);
            }
            return 0; // command not found
        }
    };

    enum keyboard_msg_type {
        key_down = WM_KEYDOWN,
        key_up = WM_KEYUP
    };
    struct callback_reg {
        enum callback_type {
            menu_cmd,
            mouse_msg,
            kbd_msg
        } type;
        union {
            cmd_callback cmd_cb;
            mouse_callback mouse_cb;
            kbd_callback key_cb;
        } callback_func;
    };
    typedef std::map<callback_id_t, callback_reg> callback_map_t;

    class callback_registry {
        static callback_map_t callback_map;
    public:
        static void register_callback(callback_id_t id, callback_reg reg) {
            callback_map[id] = reg;
        }
        static callback_reg* get_callback(callback_id_t id) {
            auto it = callback_map.find(id);
            if (it != callback_map.end()) {
                return &it->second;
            }
            return nullptr; // not found
        }
        static int invoke_cmd_callback(int id, int cmd) {
            auto reg = get_callback(callback_id_t(WM_COMMAND, id));
            if (reg) {
                if (reg->callback_func.cmd_cb) {
                    reg->callback_func.cmd_cb(cmd);
                    return 1; // handled
                }
            }
            return 0; // not handled
        }
        static int invoke_mouse_callback(int id, int x, int y, unsigned __int64 extra) {
            auto reg = get_callback(callback_id_t(id, 0));
            if (reg) {
                if (reg->callback_func.mouse_cb) {
                    reg->callback_func.mouse_cb(x, y, extra);
                    return 1; // handled    
                }
            }
            return 0; // not handled
        }
        // id = WM_KEYDOWN or WM_KEYUP, keycode = wParam
        static int invoke_kbd_callback(int id, int keycode) {
            auto reg = get_callback(callback_id_t(id, 0));
            if (reg) {
                if (reg->callback_func.key_cb) {
                    reg->callback_func.key_cb(id, keycode);
                    return 1; // handled
                }
            }
            return 0; // not handled
        }
    };
}

#endif // __cmd_target_h__
