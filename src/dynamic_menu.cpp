#include "dynamic_menu.h"

namespace btm_framework
{
    WNDPROC Menu::s_OldWndProc = nullptr;
    Menu* Menu::s_Instance = nullptr;

    Menu::Menu()
        : m_MenuBar(CreateMenu()), m_NextId(1000)
    {
        s_Instance = this;
    }

    Menu::~Menu()
    {
        DestroyMenu(m_MenuBar);
    }

    void Menu::attach_to_window(HWND hwnd)
    {
        // Install menu bar
        SetMenu(hwnd, m_MenuBar);
        DrawMenuBar(hwnd);

        // Subclass WndProc once
        if (!s_OldWndProc)
        {
            s_OldWndProc = reinterpret_cast<WNDPROC>(
                SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(MenuWndProc))
            );
        }
    }

    HMENU Menu::create_submenu(const std::string& label)
    {
        HMENU submenu = CreatePopupMenu();

        AppendMenuA(m_MenuBar, MF_POPUP, reinterpret_cast<UINT_PTR>(submenu), label.c_str());
        return submenu;
    }

    int Menu::add_item(HMENU parent, const std::string& label, MenuCallback cb)
    {
        int id = m_NextId++;

        AppendMenuA(parent, MF_STRING, id, label.c_str());
        register_callback(id, std::move(cb));

        return id;
    }

    void Menu::add_separator(HMENU parent)
    {
        AppendMenuA(parent, MF_SEPARATOR, 0, nullptr);
    }

    void Menu::remove_item(int commandId)
    {
        // Remove callback
        m_Callbacks.erase(commandId);

        // Remove from menu (search all submenus)
        MENUITEMINFOA info = {};
        info.cbSize = sizeof(info);

        int count = GetMenuItemCount(m_MenuBar);
        for (int i = 0; i < count; ++i)
        {
            HMENU submenu = GetSubMenu(m_MenuBar, i);
            if (!submenu) continue;

            int subCount = GetMenuItemCount(submenu);
            for (int j = 0; j < subCount; ++j)
            {
                UINT id = GetMenuItemID(submenu, j);
                if (id == (UINT)commandId)
                {
                    RemoveMenu(submenu, j, MF_BYPOSITION);
                    return;
                }
            }
        }
    }

    void Menu::refresh()
    {
        HWND hwnd = GetForegroundWindow();
        DrawMenuBar(hwnd);
    }

    void Menu::register_callback(int id, MenuCallback cb)
    {
        m_Callbacks[id] = std::move(cb);
    }

    LRESULT CALLBACK Menu::MenuWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        if (msg == WM_COMMAND && s_Instance)
        {
            int id = LOWORD(wParam);
            auto it = s_Instance->m_Callbacks.find(id);

            if (it != s_Instance->m_Callbacks.end())
            {
                it->second();
                return 0;
            }
        }

        return CallWindowProc(s_OldWndProc, hWnd, msg, wParam, lParam);
    }
}
