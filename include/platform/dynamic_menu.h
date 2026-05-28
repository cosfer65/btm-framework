#ifndef __dynamic_menu_h__
#define __dynamic_menu_h__

#include <Windows.h>
#include <functional>
#include <unordered_map>
#include <string>
#include <vector>

namespace btm
{
    using MenuCallback = std::function<void()>;

    class Menu
    {
    public:
        Menu();
        ~Menu();

        // Attach this menu to a window
        void attach_to_window(HWND window);

        // Create a submenu and return its handle
        HMENU create_submenu(const std::string& label);

        // Add an item to a menu or submenu
        int add_item(HMENU parent, const std::string& label, MenuCallback cb);

        // Add a separator
        void add_separator(HMENU parent);

        // Remove an item dynamically
        void remove_item(int commandId);

        // Rebuild the entire menu bar (optional)
        void refresh();

    private:
        HMENU m_MenuBar;
        std::unordered_map<int, MenuCallback> m_Callbacks;
        int m_NextId;

        void register_callback(int id, MenuCallback cb);
        static LRESULT CALLBACK MenuWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
        static WNDPROC s_OldWndProc;
        static Menu* s_Instance; // singleton per window
    };
}

#endif // __dynamic_menu_h__
