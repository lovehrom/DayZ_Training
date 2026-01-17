# Crafting GUI System

**Status:** ⚠️ Mod-derived
**Related:** [Crafting](06_crafting.md) | [GUI](16_gui_systems.md)

---

## ⚡ 30-Second Summary

GUI крафтинг: CraftingManager modded, GUI интерфейс, server-client RPC, XML configuration рецептов.

---

## 🎯 5-Minute Solution

### Modded CraftingManager

```c
modded class CraftingManager
{
    protected ref CraftingMenu m_Menu;

    override void Init()
    {
        super.Init();

        // Show GUI
        m_Menu = new CraftingMenu();
    }

    override void CraftItem(string recipe)
    {
        super.CraftItem(recipe);

        // Server-side validation
        if (CanCraft(recipe))
        {
            ExecuteCraft(recipe);
        }
    }
}
```

---

## 💡 Examples

### Пример 1: Custom recipe

```c
class MyRecipe
{
    string Name = "Custom Item";
    ref array<string> Ingredients = {"Item1", "Item2"};
    string Result = "CustomItem";
}
```

### Пример 2: GUI callback

```c
class CraftingMenu
{
    void OnCraftButton(string recipe)
    {
        GetGame().RPCSingleParam(null, RPC_CRAFT, new Param1<string>(recipe), true);
    }
}
```

---

## 🔗 Related Topics

→ [Crafting (Vanilla)](06_crafting.md) - Base crafting
→ [GUI](16_gui_systems.md) - Widgets
→ [Reference: Crafting GUI](../reference/CRAFTING_GUI_SYSTEMS.md) - Полная документация

---

*Последнее обновление: 2025-01-17*
*Источник: CraftSystem (3626433444)*
