# UI Framework - Complete Reference

**Source:** TraderX UI + Vanilla DayZ UI
**Complexity:** Advanced

---

## ⚡ Summary

DayZ UI Framework (UIScriptedMenu) enables creation of custom menus, HUDs, and interactive interfaces for mods.

---

## 🎯 Key Components

- ✅ **UIScriptedMenu** - Base class for menus
- ✅ **Widget System** - Buttons, text, edit boxes
- ✅ **Layout System** - Grid and positioning
- ✅ **Event Handling** - Click, hover, input
- ✅ **RPC Integration** - Client-server UI sync

---

## 🔧 Basic Menu

```c
class MyMenu: UIScriptedMenu
{
    protected TextWidget m_TitleText;
    protected ButtonWidget m_CloseButton;

    override void Init()
    {
        // Load layout
        layout = "{MyLayoutFile}";
    }

    override void OnShow()
    {
        super.OnShow();
        m_TitleText.SetText("My Menu");
    }

    override bool OnClick(Widget w, int x, int y, int button)
    {
        if (w == m_CloseButton)
        {
            Close();
            return true;
        }
        return false;
    }
}
```

---

## 📝 Quick Reference

### Show Menu
```c
GetGame().GetUIManager().ShowScriptedMenu(player.m_MyMenu, NULL);
```

### Close Menu
```c
Close();
```

---

**Last Updated:** 2025-01-18
**Status:** ✅ Complete
