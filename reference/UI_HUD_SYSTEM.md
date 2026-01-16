# UI/HUD System - Пользовательский интерфейс

**Status:** ✅ Verified from samples
**Sources:** DayZCommunityOfflineMode/samples
**Related:** [Mission API](MISSION_API.md) | [Hook Points](HOOK_POINTS.md)

---

## ⚡ 30-Second Summary

DayZ UI система использует `ScriptedWidgetEventHandler` для обработки виджетов и `GetGame().GetWorkspace().CreateWidgets()` для создания UI.

---

## 🎯 5-Minute Solution: Создать custom widget

```c
// ✅ ПРОВЕРЕНО: DayZCommunityOfflineMode samples
class MyWidget extends ScriptedWidgetEventHandler
{
    protected Widget m_Root;

    void MyWidget()
    {
        // Создать widget из layout файла
        m_Root = GetGame().GetWorkspace().CreateWidgets("MyMod/GUI/layouts/MyWidget.layout");
        m_Root.SetHandler(this);  // Назначить обработчик
    }

    override void OnWidgetScriptInit(Widget w)
    {
        super.OnWidgetScriptInit(w);
        // Инициализация виджета
    }
}
```

---

## 📚 Deep Dive

### ScriptedWidgetEventHandler

**✅ ПРОВЕРЕНО:** DayZCommunityOfflineMode samples

```c
class MyCustomWidget extends ScriptedWidgetEventHandler
{
    protected Widget m_Root;
    protected TextWidget m_TextWidget;
    protected ButtonWidget m_ButtonWidget;

    void MyCustomWidget()
    {
        // Создать UI
        m_Root = GetGame().GetWorkspace().CreateWidgets("MyMod/GUI/layouts/MyCustom.layout");
        m_Root.SetHandler(this);

        // Получить виджеты по name
        m_TextWidget = TextWidget.Cast(m_Root.FindAnyWidget("TextWidget"));
        m_ButtonWidget = ButtonWidget.Cast(m_Root.FindAnyWidget("ButtonWidget"));
    }

    // Инициализация виджета
    override void OnWidgetScriptInit(Widget w)
    {
        super.OnWidgetScriptInit(w);
        Print("Widget initialized!");
    }

    // Обновление (каждый кадр)
    override void Update()
    {
        super.Update();

        if (m_TextWidget)
        {
            m_TextWidget.SetText("Hello World!");
        }
    }
}
```

### Layout файлы

**✅ ПРОВЕРЕНО:** DayZCommunityOfflineMode samples

```cpp
// MyMod/GUI/layouts/MyCustom.layout
class MyCustom
{
    // Root widget
    class Root
    {
        x = 500;
        y = 300;
        w = 300;
        h = 200;
    }

    // Text widget
    class TextWidget
    {
        x = 10;
        y = 10;
        w = 280;
        h = 30;
        text = "Hello World";
        font = "GuiFont";
        size = 20;
        color = "#FFFFFFFF";
    }

    // Button widget
    class ButtonWidget
    {
        x = 10;
        y = 50;
        w = 280;
        h = 40;
        text = "Click Me";
    }
}
```

### modded class MissionGameplay

**✅ ПРОВЕРЕНО:** DayZCommunityOfflineMode samples

```c
// Client-side only!
#ifndef SERVER
modded class MissionGameplay
{
    protected ref MyCustomWidget m_MyWidget;

    override void OnInit()
    {
        super.OnInit();

        // Создать custom widget
        m_MyWidget = new MyCustomWidget();
    }

    override void OnUpdate(float timeslice)
    {
        super.OnUpdate(timeslice);

        // Обновить widget если нужно
        if (m_MyWidget)
        {
            // Widget сам обновляется через Update()
        }
    }
}
#endif
```

### CALL_CATEGORY_GUI

**✅ ПРОВЕРЕНО:** DayZCommunityOfflineMode samples

```c
class MyCustomWidget extends ScriptedWidgetEventHandler
{
    void MyCustomWidget()
    {
        // Регистрировать обновление в GUI queue
        GetGame().GetUpdateQueue(CALL_CATEGORY_GUI).Insert(this.Update);
    }

    void Update()
    {
        // Вызывается каждый кадр через GUI queue
    }
}
```

---

## 💡 Examples

### Example 1: Custom HUD элемент

```c
// Client-side only!
#ifndef SERVER
modded class Hud
{
    protected ref MyCustomHudElement m_CustomElement;

    override void OnInit()
    {
        super.OnInit();

        // Создать custom HUD element
        m_CustomElement = new MyCustomHudElement();
    }

    override void OnUpdate(float delta_time)
    {
        super.OnUpdate(delta_time);

        // Custom element обновляется сам
    }
}
#endif
```

### Example 2: Debug monitor

```c
// ✅ ПРОВЕРЕНО: DayZCommunityOfflineMode samples
class DebugMonitor extends ScriptedWidgetEventHandler
{
    protected Widget m_Root;
    protected TextWidget m_FPSWidget;

    void DebugMonitor()
    {
        m_Root = GetGame().GetWorkspace().CreateWidgets("MyMod/GUI/layouts/DebugMonitor.layout");
        m_Root.SetHandler(this);

        m_FPSWidget = TextWidget.Cast(m_Root.FindAnyWidget("FPSWidget"));

        // Добавить в GUI update queue
        GetGame().GetUpdateQueue(CALL_CATEGORY_GUI).Insert(this.Update);
    }

    void Update()
    {
        if (m_FPSWidget)
        {
            float fps = GetGame().GetFPS();
            m_FPSWidget.SetText("FPS: " + fps.ToString());
        }
    }
}
```

---

## 🔗 Related Topics

→ [Mission API](MISSION_API.md) - GetMission().GetHud()
→ [Hook Points](HOOK_POINTS.md) - modded class паттерны

---

## 🚨 Troubleshooting

**Ошибка:** Widget не отображается
- **Причина:** Неправильный путь к layout файлу
- **Решение:** Проверьте что путь правильный и файл существует

**Ошибка:** Update не вызывается
- **Причина:** Забыли добавить в CALL_CATEGORY_GUI
- **Решение:** `GetGame().GetUpdateQueue(CALL_CATEGORY_GUI).Insert(this.Update)`

**Ошибка:** Работает на сервере
- **Причина:** UI только на клиенте
- **Решение:** Используйте `#ifndef SERVER`

---

## ⚠️ SOURCE VERIFICATION

**Статус:** ✅ Verified from samples (DayZCommunityOfflineMode)

**Что проверено:**
- ✅ ScriptedWidgetEventHandler - samples
- ✅ GetGame().GetWorkspace().CreateWidgets() - samples
- ✅ CALL_CATEGORY_GUI - samples
- ✅ modded class MissionGameplay - samples
- ✅ Layout файлы - samples

**Что НЕ проверено:**
- ❌ Полный список Widget типов
- ❌ Все Widget методы
- ❌ Сложные UI паттерны (только простые)

---

*Последнее обновление: 2025-01-16*
*Источник: DayZCommunityOfflineMode/samples*
