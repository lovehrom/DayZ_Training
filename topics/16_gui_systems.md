# GUI & HUD Systems

**Status:** ⚠️ Advanced (требуется практики)
**Related:** [Mission System](17_mission_system.md) | [Crafting GUI](25_crafting_gui.md)

---

## ⚡ 30-Second Summary

DayZ UI система: ScriptedWidgetEventHandler для обработки, CreateWidgets() для создания UI, layout файлы для дизайна.

---

## 🎯 5-Minute Solution: Простое окно

### Step 1: Создать layout файл

**Файл:** `MyMod/GUI/layouts/MyWindow.layout`

```cpp
class MyWindow
{
    class Root
    {
        x = 500;
        y = 300;
        w = 300;
        h = 200;
    }

    class TextWidget
    {
        x = 20;
        y = 20;
        text = "Hello World";
    }

    class ButtonWidget
    {
        x = 20;
        y = 50;
        w = 100;
        h = 30;
        text = "Click Me";
    }
}
```

### Step 2: Создать handler

```c
class MyWindowHandler extends ScriptedWidgetEventHandler
{
    protected Widget m_Root;

    void MyWindowHandler()
    {
        m_Root = GetGame().GetWorkspace().CreateWidgets("MyMod/GUI/layouts/MyWindow.layout");
        m_Root.SetHandler(this);
    }

    override void OnWidgetScriptInit(Widget w)
    {
        super.OnWidgetScriptInit(w);
        Print("Window initialized!");
    }
}
```

### Step 3: Показать окно

```c
MyWindowHandler window = new MyWindowHandler();
```

---

## 📚 Deep Dive

### ScriptedWidgetEventHandler

**Базовый класс для всех UI handlers**

```c
class MyCustomWidget extends ScriptedWidgetEventHandler
{
    protected Widget m_Root;
    protected TextWidget m_Text;
    protected ButtonWidget m_Button;

    void MyCustomWidget()
    {
        m_Root = GetGame().GetWorkspace().CreateWidgets("MyMod/GUI/layouts/MyCustom.layout");
        m_Root.SetHandler(this);

        // Получить виджеты по name
        m_Text = TextWidget.Cast(m_Root.FindAnyWidget("TextWidget"));
        m_Button = ButtonWidget.Cast(m_Root.FindAnyWidget("ButtonWidget"));
    }

    // Инициализация (один раз при создании)
    override void OnWidgetScriptInit(Widget w)
    {
        super.OnWidgetScriptInit(w);
    }

    // Обновление (каждый кадр)
    override void Update()
    {
        super.Update();

        if (m_Text)
        {
            m_Text.SetText("Hello World!");
        }
    }

    // Клик по кнопке
    override bool OnClick(Widget w, int x, int y, int button)
    {
        if (w == m_Button)
        {
            Print("Button clicked!");
            return true;
        }
        return false;
    }
}
```

---

## 💡 Examples

### Пример 1: Текст с обновлением

```c
class ScoreWidget extends ScriptedWidgetEventHandler
{
    protected Widget m_Root;
    protected TextWidget m_ScoreText;
    protected int m_Score = 0;

    void ScoreWidget()
    {
        m_Root = GetGame().GetWorkspace().CreateWidgets("MyMod/GUI/layouts/Score.layout");
        m_Root.SetHandler(this);

        m_ScoreText = TextWidget.Cast(m_Root.FindAnyWidget("ScoreText"));
    }

    override void Update()
    {
        super.Update();

        if (m_ScoreText)
        {
            m_ScoreText.SetText("Score: " + m_Score);
        }
    }

    void AddScore(int points)
    {
        m_Score += points;
    }
}
```

### Пример 2: Кнопка с действием

```c
class MenuWidget extends ScriptedWidgetEventHandler
{
    protected ButtonWidget m_StartButton;
    protected ButtonWidget m_ExitButton;

    override bool OnClick(Widget w, int x, int y, int button)
    {
        if (w == m_StartButton)
        {
            OnStartClick();
            return true;
        }

        if (w == m_ExitButton)
        {
            OnExitClick();
            return true;
        }

        return false;
    }

    void OnStartClick()
    {
        Print("Start button clicked!");
        // Начать игру
    }

    void OnExitClick()
    {
        Print("Exit button clicked!");
        // Выйти
    }
}
```

### Пример 3: Динамическое создание

```c
class DynamicWidget
{
    protected Widget m_Root;

    void DynamicWidget()
    {
        m_Root = GetGame().GetWorkspace().CreateWidgets("MyMod/GUI/layouts/Dynamic.layout");
    }

    void AddButton(string text, int y)
    {
        ButtonWidget button = ButtonWidget.Cast(m_Root.FindAnyWidget("ButtonTemplate"));
        if (button)
        {
            button.SetText(text);
            button.SetPos(0, y);
            button.Show(true);
        }
    }
}
```

---

## 🔗 Related Topics

→ [Crafting GUI](25_crafting_gui.md) - GUI для крафтинга
→ [Mission System](17_mission_system.md) - Mission init
→ [Reference: UI/HUD](../reference/UI_HUD_SYSTEM.md) - Полная документация

---

## 💡 Tips

### Best Practices

1. **Используйте layout файлы** для дизайна UI
2. **Получайте виджеты по name** через FindAnyWidget()
3. **Переопределяйте Update()** для динамического обновления
4. **Переопределяйте OnClick()** для обработки кликов
5. **Скрывайте виджеты** через Show(false) вместо удаления

### Common Widget Types

- **TextWidget** - Текст
- **ButtonWidget** - Кнопка
- **ImageWidget** - Изображение
- **GridWidget** - Сетка (для списков)
- **ScrollWidget** - Прокрутка

### Layout Positioning

```cpp
class Example
{
    class Root
    {
        // Позиция относительно экрана
        x = 100;  // Пиксели слева
        y = 100;  // Пиксели сверху
        w = 300;  // Ширина
        h = 200;  // Высота
    }
}
```

---

*Последнее обновление: 2025-01-17*
*Источник: reference/UI_HUD_SYSTEM.md*
