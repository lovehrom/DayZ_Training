# UI Framework - Complete Reference

**Source:** TraderX UI + Vanilla DayZ UI
**Complexity:** Advanced
**Related:** [HUD System](UI_HUD_SYSTEM.md) | [Notifications](NOTIFICATION_SYSTEM.md)

---

## ⚡ 30-Second Summary

DayZ UI Framework (**UIScriptedMenu**) enables creation of custom menus, HUDs, and interactive interfaces. Understanding widgets, layouts, events, and RPC integration is critical for player-facing features.

---

## 🎯 Key Concepts

### UI Architecture

```
UIScriptedMenu (Base Class)
├── Widgets (Buttons, Text, Edit Boxes)
├── Layouts (Positioning, Sizing)
├── Events (Click, Hover, Input)
└── RPC (Client-Server Sync)
```

**Key Components:**
- **UIScriptedMenu** - Base class for all menus
- **Widgets** - UI elements (buttons, text, images)
- **Layouts** - Position and size widgets
- **Events** - User interactions
- **RPC** - Server-client communication

---

## 🔧 UIScriptedMenu Base Class

### Basic Menu Structure

```c
class MyMenu: UIScriptedMenu
{
    protected reference Widget m_RootWidget;
    protected TextWidget m_TitleText;
    protected ButtonWidget m_CloseButton;
    protected EditBoxWidget m_NameInput;

    void MyMenu()
    {
        // Must call constructor
    }

    override void Init()
    {
        // Load layout file
        m_RootWidget = layoutRoot;
    }

    override void OnShow()
    {
        super.OnShow();

        // Initialize widgets
        m_TitleText = TextWidget.Cast(m_RootWidget.FindAnyWidget("TitleText"));
        m_CloseButton = ButtonWidget.Cast(m_RootWidget.FindAnyWidget("CloseButton"));
        m_NameInput = EditBoxWidget.Cast(m_RootWidget.FindAnyWidget("NameInput"));

        // Set initial values
        m_TitleText.SetText("My Menu");
    }

    override bool OnClick(Widget w, int x, int y, int button)
    {
        // Handle button clicks
        if (w == m_CloseButton)
        {
            Close();
            return true;
        }
        return false;
    }

    override void Update()
    {
        // Called every frame
        super.Update();

        // Update dynamic content
        UpdateHealthBar();
    }

    void UpdateHealthBar()
    {
        PlayerBase player = GetGame().GetPlayer();
        float health = player.GetHealth("GlobalHealth", "Health");

        TextWidget healthText = TextWidget.Cast(m_RootWidget.FindAnyWidget("HealthText"));
        healthText.SetText("Health: " + health.ToString());
    }
}
```

---

## 📦 Widget Types

### 1. TextWidget

**Display text:**
```c
TextWidget text = TextWidget.Cast(m_RootWidget.FindAnyWidget("MyText"));
text.SetText("Hello World");
text.SetColor(ARGB(255, 255, 255, 255));  // White
text.SetSize(256, 32);  // Width, height
text.SetPos(100, 100);  // X, Y position
```

---

### 2. ButtonWidget

**Clickable button:**
```c
ButtonWidget button = ButtonWidget.Cast(m_RootWidget.FindAnyWidget("MyButton"));
button.SetText("Click Me");
button.SetSize(128, 32);
button.SetPos(100, 100);
```

**Handle click:**
```c
override bool OnClick(Widget w, int x, int y, int button)
{
    if (w == button)
    {
        Print("Button clicked!");
        return true;  // Handled
    }
    return false;  // Not handled
}
```

---

### 3. EditBoxWidget

**Text input field:**
```c
EditBoxWidget editBox = EditBoxWidget.Cast(m_RootWidget.FindAnyWidget("MyEditBox"));
editBox.SetText("");
editBox.SetLimit(64);  // Max 64 characters
editBox.SetSize(256, 32);
editBox.SetPos(100, 100);
```

**Get input:**
```c
string text = editBox.GetText();
```

---

### 4. ImageWidget

**Display image:**
```c
ImageWidget image = ImageWidget.Cast(m_RootWidget.FindAnyWidget("MyImage"));
image.SetImage("set:dayz_gui filename:images/logo.paa");
image.SetSize(256, 256);
image.SetPos(100, 100);
```

---

### 5. ProgressBarWidget

**Progress bar:**
```c
ProgressBarWidget progressBar = ProgressBarWidget.Cast(m_RootWidget.FindAnyWidget("MyProgressBar"));
progressBar.SetSize(256, 32);
progressBar.SetPos(100, 100);
progressBar.SetCurrentState(0.5);  // 50%
```

---

## 📐 Layout System

### Absolute Positioning

**Set position and size explicitly:**
```c
widget.SetPos(100, 100);  // X, Y from top-left
widget.SetSize(256, 64);  // Width, height
```

---

### Anchors

**Anchor widget to screen edge:**
```c
// Anchor to top-left (default)
widget.SetFlags(WidgetFlags.AnchorTop | WidgetFlags.AnchorLeft);

// Anchor to top-right
widget.SetFlags(WidgetFlags.AnchorTop | WidgetFlags.AnchorRight);
widget.SetPos(-100, 100);  // 100px from right edge

// Anchor to bottom-right
widget.SetFlags(WidgetFlags.AnchorBottom | WidgetFlags.AnchorRight);
widget.SetPos(-100, -50);  // 100px from right, 50px from bottom
```

---

### Grid Layout

**Organize widgets in grid:**
```c
GridWidget grid = GridWidget.Cast(m_RootWidget.FindAnyWidget("MyGrid"));
grid.SetCols(3);  // 3 columns
grid.SetRows(2);  // 2 rows

// Add child to grid cell (0, 0)
Widget child = grid.GetWidget(0, 0);
```

---

## 🎨 Styling

### Colors

**ARGB format:**
```c
// ARGB(A, R, G, B)
// A = Alpha (0-255, 255 = opaque)
// R = Red (0-255)
// G = Green (0-255)
// B = Blue (0-255)

text.SetColor(ARGB(255, 255, 0, 0));  // Red
text.SetColor(ARGB(255, 0, 255, 0));  // Green
text.SetColor(ARGB(255, 0, 0, 255));  // Blue
text.SetColor(ARGB(128, 255, 255, 255));  // Semi-transparent white
```

---

### Fonts

**Set font size and style:**
```c
TextWidget text = ...;

// Set font size (0 = default)
text.SetTextSize(24);  // 24pt

// Set font (if custom font available)
text.SetFont("myCustomFont");
```

---

## 🔔 Event Handling

### Click Events

```c
override bool OnClick(Widget w, int x, int y, int button)
{
    Print("Clicked: " + w.GetName());
    Print("Position: " + x + ", " + y);
    Print("Button: " + button);  // 0 = left, 1 = right

    // Check which widget was clicked
    if (w == m_SubmitButton)
    {
        OnSubmit();
        return true;
    }

    return false;  // Let other handlers process
}

void OnSubmit()
{
    string text = m_NameInput.GetText();
    Print("Submitted: " + text);
    Close();
}
```

---

### Hover Events

```c
override bool OnMouseEnter(Widget w, int x, int y)
{
    if (w == m_Button)
    {
        m_Button.SetColor(ARGB(255, 255, 255, 0));  // Yellow on hover
        return true;
    }
    return false;
}

override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
{
    if (w == m_Button)
    {
        m_Button.SetColor(ARGB(255, 200, 200, 200));  // Gray normally
        return true;
    }
    return false;
}
```

---

### Input Events

```c
override bool OnChange(Widget w, int x, int y, bool finished)
{
    if (w == m_NameInput)
    {
        string text = m_NameInput.GetText();
        Print("Typing: " + text);
        return true;
    }
    return false;
}
```

---

## 🌐 RPC Integration

### Server → Client UI Updates

**Server sends data, client updates UI:**

**Server:**
```c
void UpdatePlayerUI(PlayerBase player, string message)
{
    // Send RPC to client
    GetRPCManager().SendRPC("MyMod", "UpdateUI", new Param1<string>(message), true, player.GetIdentity());
}
```

**Client (in UIScriptedMenu):**
```c
// Register RPC
void InitRPC()
{
    GetRPCManager().AddRPC("MyMod", "UpdateUI", this, SingleplayerExecutionType.Client);
}

// Receive RPC
void UpdateUI(PlayerIdentity sender, Object target, ParamsReadContext ctx)
{
    string message;
    if (!ctx.Read(message)) return;

    // Update UI
    TextWidget statusText = TextWidget.Cast(m_RootWidget.FindAnyWidget("StatusText"));
    statusText.SetText(message);
}
```

---

### Client → Server Input

**Client sends input, server processes:**

**Client (in UIScriptedMenu):**
```c
override bool OnClick(Widget w, int x, int y, int button)
{
    if (w == m_SubmitButton)
    {
        string text = m_NameInput.GetText();

        // Send to server
        GetRPCManager().SendRPC("MyMod", "SubmitName", new Param1<string>(text), true);
        return true;
    }
    return false;
}
```

**Server:**
```c
void Server_SubmitName(PlayerIdentity sender, Object target, ParamsReadContext ctx)
{
    string name;
    if (!ctx.Read(name)) return;

    // Validate
    if (name.Length() > 32)
    {
        GetRPCManager().SendRPC("MyMod", "ShowError", new Param1<string>("Name too long!"), true, sender);
        return;
    }

    // Process
    SetPlayerName(sender.GetId(), name);
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

### Find Widget

```c
Widget w = m_RootWidget.FindAnyWidget("WidgetName");
TextWidget text = TextWidget.Cast(w);
```

### Set Widget Properties

```c
widget.SetPos(x, y);
widget.SetSize(width, height);
widget.Show(true);
widget.Show(false);
```

---

## ⚠️ Common Mistakes

### ❌ Mistake 1: Not Checking Null

```c
// WRONG: No null check
TextWidget text = TextWidget.Cast(m_RootWidget.FindAnyWidget("MyText"));
text.SetText("Hello");  // Might crash!

// CORRECT: Check null
TextWidget text = TextWidget.Cast(m_RootWidget.FindAnyWidget("MyText"));
if (text)
    text.SetText("Hello");
```

### ❌ Mistake 2: Updating Every Frame

```c
// WRONG: Updates every frame = LAG
override void Update()
{
    GetServerData();  // Expensive RPC every frame
}

// CORRECT: Cache and update periodically
override void Update()
{
    if (GetGame().GetTickTime() - m_LastUpdate > 1.0)
    {
        GetServerData();
        m_LastUpdate = GetGame().GetTickTime();
    }
}
```

---

## 🎓 Next Steps

**After learning UI framework:**
1. **Read:** [HUD System](UI_HUD_SYSTEM.md) - HUD elements
2. **Example:** [examples/gui_crafting/](../examples/gui_crafting/) - Working UI
3. **Example:** Create custom trader menu

---

## 💡 Key Takeaways

1. **UIScriptedMenu = Base Class**
   - All menus inherit from UIScriptedMenu
   - Override OnShow(), OnClick(), Update()
   - Use widgets for UI elements

2. **Widgets = UI Elements**
   - TextWidget, ButtonWidget, EditBoxWidget, etc.
   - FindAnyWidget() to locate
   - SetPos(), SetSize() to position

3. **Events = User Interaction**
   - OnClick() for button clicks
   - OnMouseEnter() / OnMouseLeave() for hover
   - OnChange() for input

4. **RPC = Client-Server Sync**
   - Server → Client: Update UI
   - Client → Server: Submit input
   - Always validate on server

---

**Last Updated:** 2025-01-18
**Status:** ✅ Complete
**Next:** [Notification System](NOTIFICATION_SYSTEM.md)
