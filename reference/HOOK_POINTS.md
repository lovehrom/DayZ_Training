# Hook Points - Точки внедрения в vanilla код

**Status:** ⚠️ Mod-derived (requires verification)
**Source:** [Zen Skills](https://steamcommunity.com/sharedfiles/filedetails/?id=3601119520), [CF Framework](https://steamcommunity.com/sharedfiles/filedetails/?id=1559212036)
**Related:** [PlayerBase API](PLAYERBASE_API.md) | [Items](../topics/01_items.md)

---

## ⚡ 30-Second Summary

Hook points - это места в vanilla классах, где можно внедрить свой код через `modded class` для перехвата событий (крафт, использование предметов, HUD обновления).

---

## 🎯 5-Minute Solution: Перехватить событие крафта

### Понять modded class

**⚠️ ВАЖНО:** modded class НЕ заменяет оригинал, он его расширяет!

```c
// ✅ ПРАВИЛЬНО: Расширение
modded class ActionCraftCB {
    override void OnFinishProgressServer(ActionData action)
    {
        super.OnFinishProgressServer(action); // Вызов оригинала

        // ВАШ КОД ЗДЕСЬ
        PlayerBase player = action.m_Player;
        // Обработка крафта...
    }
}

// ❌ ОШИБКА: Забыл вызвать super()
modded class ActionCraftCB {
    override void OnFinishProgressServer(ActionData action)
    {
        // super.OnFinishProgressServer(action); <- ЭТО НУЖНО!
        // Если убрать super - vanilla код не выполнится
    }
}
```

### Hook точка: OnFinishProgressServer

**⚠️ НЕ ПРОВЕРЕНО:** Mod-derived pattern

**Когда вызывается:** При завершении действия на сервере

**Классы для модификации:**
- `ActionCraftCB` - Крафтинг
- `ActionConsume` - Употребление еды/медикаментов
- `ActionBuildBase` - Строительство
- `ActionMineTree` - Рубка деревьев
- `ActionMineBush` - Сбор кустов
- `ActionDigging` - Копание

**Пример: EXP за крафт**

```c
modded class ActionCraftCB
{
    override void OnFinishProgressServer(ActionData action)
    {
        super.OnFinishProgressServer(action); // ВАЖНО!

        PlayerBase player = action.m_Player;
        ItemBase item1 = action.m_MainItem;

        // Начислить EXP
        AddCraftingEXP(player, item1.GetType());
    }

    void AddCraftingEXP(PlayerBase player, string itemType)
    {
        // Логика начисления EXP...
        Print("Player " + player.GetIdentity().GetId() + " crafted " + itemType);
    }
}
```

---

## 📚 Deep Dive

### Основные hook точки

#### 1. Action Hooks (Действия)

**⚠️ НЕ ПРОВЕРЕНО:** Mod-derived pattern

**OnFinishProgressServer()**
```c
// ✅ ПРОВЕРЕНО: raw_data/3_Game/Actions/ActionBase.c (base class)
modded class ActionBase
{
    override void OnFinishProgressServer(ActionData action)
    {
        super.OnFinishProgressServer(action);
        // Вызывается когда действие завершено на сервере
    }
}
```

**Use cases:**
- EXP за крафт
- Логирование действий
- Триггер других событий
- Анти-гринд механики

**Setup()**
```c
// ✅ ПРОВЕРЕНО: raw_data/3_Game/Actions/ActionBase.c (base class)
modded class ActionContinuousBase
{
    override void Setup(ActionData action)
    {
        super.Setup(action);
        // Вызывается при старте действия
    }
}
```

**Use cases:**
- Ускорение действий (perks)
- Проверка условий
- Модификация параметров

**OnExecute()**
```c
// ✅ ПРОВЕРЕНО: raw_data/3_Game/Actions/ActionBase.c
modded class ActionSingle
{
    override void OnExecute(ActionData action)
    {
        super.OnExecute(action);
        // Вызывается при выполнении однократного действия
    }
}
```

#### 2. Item Hooks (Предметы)

**⚠️ НЕ ПРОВЕРЕНО:** Mod-derived pattern

**OnConsume()**
```c
modded class Edible_Base
{
    override void OnConsume(float amount, PlayerBase player)
    {
        super.OnConsume(amount, player);

        // Вызывается когда предмет употреблён
        // amount - количество
        // player - кто употребил

        // Начислить EXP за еду
        // Применить эффекты
        // Записать в статистику
    }
}
```

**OnAttach() / OnDetach()**
```c
modded class ItemBase
{
    override void OnAttach(Widget w, EntityAI parent)
    {
        super.OnAttach(w, parent);
        // Предмет присоединён к другому предмету
    }

    override void OnDetach(EntityAI parent)
    {
        super.OnDetach(parent);
        // Предмет отсоединён
    }
}
```

#### 3. Player Hooks (Игрок)

**⚠️ ЧАСТИЧНО ПРОВЕРЕНО:** raw_data/PlayerBase.c

**OnInit()**
```c
// ✅ ПРОВЕРЕНО: raw_data/PlayerBase.c
modded class PlayerBase
{
    override void OnInit()
    {
        super.OnInit();
        // Инициализация игрока
        // Загрузка данных из JSON
    }
}
```

**OnUpdate()**
```c
// ✅ ПРОВЕРЕНО: raw_data/PlayerBase.c
modded class PlayerBase
{
    override void OnUpdate(float delta_time)
    {
        super.OnUpdate(delta_time);
        // Каждый кадр
        // Обновление UI
        // Проверка условий
    }
}
```

**SetActions()**
```c
// ✅ ПРОВЕРЕНО: raw_data/PlayerBase.c
modded class PlayerBase
{
    override void SetActions(out TInputActionMap InputActionMap)
    {
        super.SetActions(InputActionMap);
        // Добавить свои actions
        InputActionMap.AddAction(UActionWaveHello);
    }
}
```

#### 4. HUD Hooks (Интерфейс)

**⚠️ НЕ ПРОВЕРЕНО:** Mod-derived pattern

```c
modded class Hud
{
    protected ref Widget_MyCustomElement m_MyCustomElement;

    override void OnUpdate(float delta_time)
    {
        super.OnUpdate(delta_time);

        // Обновить свой HUD элемент
        if (m_MyCustomElement)
        {
            m_MyCustomElement.Update();
        }
    }

    override void OnInit()
    {
        super.OnInit();

        // Создать свой HUD элемент
        m_MyCustomElement = new Widget_MyCustomElement(layout_root);
    }
}
```

### File-based JSON Storage Pattern

**⚠️ НЕ ПРОВЕРЕНО:** Mod-derived pattern

**Zen Skills подход:**

```c
modded class PlayerBase
{
    protected ref ZenSkillsPlayerData m_SkillsData;

    override void OnInit()
    {
        super.OnInit();

        // Загрузка JSON при инициализации
        LoadSkillsData();
    }

    void LoadSkillsData()
    {
        string uid = GetIdentity().GetId();
        string path = "$profile:Zenarchist\\Skills\\PlayerDB\\" + uid + ".json";

        m_SkillsData = new ZenSkillsPlayerData();

        if (FileExist(path))
        {
            // ✅ JsonFileLoader из official samples
            JsonFileLoader<ZenSkillsPlayerData>.JsonLoadFile(path, m_SkillsData);
        }
        else
        {
            // Создать новый файл
            JsonFileLoader<ZenSkillsPlayerData>.JsonSaveFile(path, m_SkillsData);
        }
    }

    void SaveSkillsData()
    {
        string uid = GetIdentity().GetId();
        string path = "$profile:Zenarchist\\Skills\\PlayerDB\\" + uid + ".json";

        // ✅ JsonFileLoader из official samples
        JsonFileLoader<ZenSkillsPlayerData>.JsonSaveFile(path, m_SkillsData);
    }

    override void OnStoreSave(BytesContext ctx)
    {
        super.OnStoreSave(ctx);

        // Сохранить перед выгрузкой
        SaveSkillsData();
    }
}
```

---

## 💡 Examples

### Example 1: EXP за крафт (Anti-grind)

```c
modded class ActionCraftCB
{
    override void OnFinishProgressServer(ActionData action)
    {
        super.OnFinishProgressServer(action);

        PlayerBase player = action.m_Player;
        string itemType = action.m_MainItem.GetType();

        // Anti-grind: Nerf bubble
        if (IsRepeatCraft(player, itemType))
        {
            ReduceEXP(player);
        }

        // Начислить EXP
        AddCraftingEXP(player, itemType, 10);
    }
}
```

### Example 2: Ускорение действий (Perks)

```c
modded class ActionContinuousBase
{
    override void Setup(ActionData action)
    {
        super.Setup(action);

        // Проверить perk
        if (HasSpeedPerk(action.m_Player, this.Type()))
        {
            // Ускорить на 20%
            action.m_CompletedActionTime *= 0.8;
        }
    }
}
```

### Example 3: Логирование использования предметов

```c
modded class Edible_Base
{
    override void OnConsume(float amount, PlayerBase player)
    {
        super.OnConsume(amount, player);

        string uid = player.GetIdentity().GetId();
        string itemType = GetType();

        // Записать в лог
        Print("[CONSUME] Player: " + uid + " Item: " + itemType + " Amount: " + amount);
    }
}
```

### Example 4: HUD элемент

```c
modded class Hud
{
    protected ref TextWidget m_EXPText;

    override void OnInit()
    {
        super.OnInit();

        // Создать текстовый виджет
        m_EXPText = TextWidget.Cast(layout_root.FindAnyWidget("EXPText"));
    }

    override void OnUpdate(float delta_time)
    {
        super.OnUpdate(delta_time);

        PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
        if (player && m_EXPText)
        {
            // Обновить текст EXP
            m_EXPText.SetText("EXP: " + GetPlayerEXP(player).ToString());
        }
    }
}
```

---

## 🔗 Related Topics

→ [PlayerBase API](PLAYERBASE_API.md) - OnInit, OnUpdate
→ [Actions](../topics/03_actions.md) - ActionBase система
→ [Items](../topics/01_items.md) - OnConsume, OnAttach

---

## 🚨 Troubleshooting

**Ошибка:** "Vanilla функционал сломался"
- **Причина:** Забыли вызвать super()
- **Решение:** ВСЕГДА вызывайте super.Метод() первым

**Ошибка:** "HUD не обновляется"
- **Причина:** Не добавили OnUpdate в modded class Hud
- **Решение:** Проверьте что modded class Hud имеет OnUpdate()

**Ошибка:** "EXP не сохраняется"
- **Причина:** Не вызвали JsonSaveFile
- **Решение:** Вызывайте Save() в OnStoreSave()

---

## ⚠️ SOURCE VERIFICATION

**Статус:** ⚠️ Mixed verification

**✅ ПРОВЕРЕНО (raw_data/):**
- ActionBase.OnFinishProgressServer() exists
- ActionContinuousBase.Setup() exists
- PlayerBase.OnInit(), OnUpdate(), SetActions() exist
- Edible_Base.OnConsume() exists

**⚠️ НЕ ПРОВЕРЕНО (mod-derived):**
- Точный синтаксис HUD модификации
- JsonFileLoader паттерн для player data
- File-based storage в $profile:
- Точные параметры ActionData

**Почему разделено?**
- Hook points - vanilla feature (✅)
- Конкретные реализации - mod patterns (⚠️)

---

## 🔍 Как использовать эту информацию

1. **Для концепций:** Используйте для понимания WHERE внедряться
2. **Для кода:**
   - Hook points (✅) - проверено в raw_data
   - Implementations (⚠️) - проверяйте на практике
3. **Для production:** Изучайте исходные моды

---

## 📋 Список известных Hook Points

### ✅ Проверено в raw_data:

| Hook Point | Класс | Когда вызывается |
|------------|-------|------------------|
| OnInit() | PlayerBase, EntityAI, ItemBase | При инициализации |
| OnUpdate() | PlayerBase | Каждый кадр |
| EEInit() | EntityAI | При создании entity |
| EEKilled() | EntityAI | При смерти |
| OnFinishProgressServer() | ActionBase | При завершении действия |
| Setup() | ActionContinuousBase | При старте действия |
| OnExecute() | ActionSingle | При выполнении |
| SetActions() | PlayerBase | При регистрации actions |
| OnConsume() | Edible_Base | При употреблении |

### ⚠️ Требует проверки:

| Hook Point | Класс | Когда вызывается |
|------------|-------|------------------|
| OnStoreSave() | PlayerBase | При сохранении (?) |
| OnStoreLoad() | PlayerBase | При загрузке (?) |
| GetGame().GetMission() | Mission | Доступ к миссии (?) |

---

*Последнее обновление: 2025-01-16*
*Источники: raw_data/ (✅) + Zen Skills/CF mods (⚠️)*
