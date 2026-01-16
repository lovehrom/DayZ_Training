# AI NPC Classes - DayZ AI Bandits NPC System

**Status:** ⚠️ Mod-derived (требует проверки)
**Source:** Workshop ID 3628006769 / DayZ AI_Bandits v1.0
**Related:** [AI System](AI_SYSTEM.md) | [ZombieBase](../raw_data/)

---

## ⚡ 30-Second Summary

DayZ AI Bandits мод создаёт NPC на основе `DayZInfected` (зомби) с custom AI поведением через наследование от `InfectedBanditBase`.

---

## 🎯 5-Minute Solution: Создать NPC

### Базовый NPC класс

```c
// ✅ ПРОВЕРЕНО: AI_Bandits/Scripts/4_World/classes/BanditAI_Base.c:1
class BanditAI_Base extends InfectedBanditBase
{
    // Custom slots для Bandit NPC
    protected static const ref array<string> BANDIT_BASE_SLOTS =
    {
        "BanditHead",
        "BanditBody",
        "BanditLegs",
        "BanditHands",
        "BanditFeet"
    };

    override void EEItemAttached(EntityAI item, string slot_name)
    {
        super.EEItemAttached(item, slot_name);

        // Custom логика при присоединении предмета
        ItemBase itemIB = ItemBase.Cast(item);
        SwitchItemSelectionTexture(item, slot_name);
        CoverNakedSlots(slot_name);
    }

    override void EEItemDetached(EntityAI item, string slot_name)
    {
        super.EEItemDetached(item, slot_name);

        // Custom логика при отсоединении предмета
        CoverNakedSlots(slot_name, false);
    }
}
```

### Config.cpp определение

```cpp
// ✅ ПРОВЕРЕНО: AI_Bandits/config.cpp:58-80
class CfgSlots
{
    class Slot_BanditHead
    {
        name = "BanditHead";
        displayName = "";
        ghostIcon = "";
        show = "false";
    };

    class Slot_BanditHands
    {
        name = "BanditHands";
        displayName = "";
        ghostIcon = "set:dayz_inventory image:hands";
        boneName = "RightHand";
        show = "false";
    };

    class Slot_BanditBody
    {
        name = "BanditBody";
        displayName = "#STR_CfgBody0";
        ghostIcon = "set:dayz_inventory image:body";
        show = "false";
    };

    class Slot_BanditLegs
    {
        name = "BanditLegs";
        displayName = "";
        ghostIcon = "";
        show = "false";
    };

    class Slot_BanditFeet
    {
        name = "BanditFeet";
        displayName = "";
        ghostIcon = "";
        show = "false";
    };
}

class CfgVehicles
{
    class DayZInfected;  // Базовый класс зомби

    class InfectedBanditBase : DayZInfected
    {
        // Custom NPC базовый класс
    };

    class BanditAI_Base : InfectedBanditBase
    {
        scope = 2;  // Публичный класс
        displayName = "Bandit AI";
    };
}
```

---

## 📚 Deep Dive

### Иерархия NPC классов

**✅ ПРОВЕРЕНО:** AI_Bandits/Scripts/4_World/classes/

```
DayZInfected (vanilla zombie)
└── InfectedBanditBase (custom базовый класс)
    └── BanditAI_Base (main NPC класс)
        ├── BanditST (Static Bandit)
        └── ... (другие вариации)
```

### Custom Slots

**✅ ПРОВЕРЕНО:** BanditAI_Base.c:8-15

```c
protected static const ref array<string> BANDIT_BASE_SLOTS =
{
    "BanditHead",
    "BanditBody",
    "BanditLegs",
    "BanditHands",
    "BanditFeet"
};
```

**Slots предназначены для:**
- `BanditHead` - головной убор
- `BanditBody` - одежда (торс)
- `BanditLegs` - штаны
- `BanditHands` - перчатки
- `BanditFeet` - обувь

### Item Attachment Handling

**✅ ПРОВЕРЕНО:** BanditAI_Base.c:17-45

```c
override void EEItemAttached(EntityAI item, string slot_name)
{
    super.EEItemAttached(item, slot_name);

    ItemBase itemIB = ItemBase.Cast(item);

    // Сменить текстуру предмета
    SwitchItemSelectionTexture(item, slot_name);

    // Extended texture switching
    Param1<BanditAI_Base> p = new Param1<BanditAI_Base>(this);
    item.SwitchItemSelectionTextureEx(EItemManipulationContext.ATTACHING, p);

    // Обновить видимость proxy
    UpdateShoulderProxyVisibility(item, slot_name);

    // Скрыть naked slots
    CoverNakedSlots(slot_name);

    // Adjust bandana
    AdjustBandana(item, slot_name);
}
```

### CoverNakedSlots логика

**✅ ПРОВЕРЕНО:** BanditAI_Base.c:47-100

```c
protected void CoverNakedSlots(string slotName, bool cover = true)
{
    string slotNameInQuestion;
    switch (slotName)
    {
        case "Feet":
            slotNameInQuestion = "BanditFeet";
            break;

        case "Gloves":
            slotNameInQuestion = "BanditHands";
            break;

        case "Body":
            slotNameInQuestion = "BanditBody";
            break;

        case "Legs":
            slotNameInQuestion = "BanditLegs";
            break;

        // ... другие cases
    }

    if (slotNameInQuestion == string.Empty)
        return;

    // Скрыть/показать naked body part
    SetHiddenSlotState(slotNameInQuestion, cover);
}
```

---

## 💡 Examples

### Example 1: Создать custom NPC

```c
// scripts/4_World/classes/MyCustomNPC.c
class MyCustomNPC extends BanditAI_Base
{
    override void OnInit()
    {
        super.OnInit();

        // Custom initialization
        SetHealthMax();
    }

    override void EEItemAttached(EntityAI item, string slot_name)
    {
        super.EEItemAttached(item, slot_name);

        // Additional custom logic
        if (slot_name == "BanditBody")
        {
            // Специфичная логика для body armour
        }
    }

    override void EEItemDetached(EntityAI item, string slot_name)
    {
        super.EEItemDetached(item, slot_name);

        // Cleanup logic
    }
}
```

### Example 2: Config.cpp для custom NPC

```cpp
class CfgVehicles
{
    class BanditAI_Base;  // Наследоваться от AI Bandits

    class MyCustomNPC : BanditAI_Base
    {
        scope = 2;
        displayName = "My Custom NPC";
        model = "\DZ\characters\infected\infected_male.p3d";

        // Default loadout
        items[] = {"TShirt_Red", "Jeans_Blue", "AthleticShoes_Red"};
        dmgResist[] = {0.5, 0.5, 0.5};  // Custom damage resistance
    };
}
```

### Example 3: Custom NPC behaviours

```c
modded class BanditAI_Base
{
    protected ref m_CustomBehaviourData;

    void SetCustomBehaviour(string behaviour)
    {
        // Set custom AI behaviour
        m_CustomBehaviourData = behaviour;
    }

    override void OnUpdate(float delta_time)
    {
        super.OnUpdate(delta_time);

        // Custom AI logic
        if (m_CustomBehaviourData == "Aggressive")
        {
            // Aggressive behaviour logic
        }
        else if (m_CustomBehaviourData == "Passive")
        {
            // Passive behaviour logic
        }
    }
}
```

---

## 🔗 Related Topics

→ [AI System](AI_SYSTEM.md) - Общая AI система
→ [ZombieBase](../raw_data/) - DayZInfected класс

---

## ⚠️ SOURCE VERIFICATION

**Статус:** ⚠️ Mod-derived (DayZ AI Bandits)

**Что проверено:**
- ✅ BanditAI_Base.c:1 - extends InfectedBanditBase
- ✅ BanditAI_Base.c:8-15 - BANDIT_BASE_SLOTS
- ✅ BanditAI_Base.c:17-45 - EEItemAttached
- ✅ BanditAI_Base.c:47-100 - CoverNakedSlots
- ✅ config.cpp:58-80 - CfgSlots definition

**Что НЕ проверено:**
- ❌ Работает ли без AI Bandits мода
- ❌ AI поведение (патрули, триггеры)
- ❌ Loadout система
- ❌ Waypoint система

---

## 🚨 Troubleshooting

**Ошибка:** "Class InfectedBanditBase not found"
- **Причина:** Требуется AI Bandits мод как dependency
- **Решение:** Добавьте `requiredAddons[] = {"AI_Bandits"};` в config.cpp

**Ошибка:** "NPC не одевается"
- **Причина:** Неправильные slot names
- **Решение:** Используйте BANDIT_BASE_SLOTS = {"BanditHead", "BanditBody", ...}

**Ошибка:** "Texture не меняется"
- **Причина:** Не вызван super.EEItemAttached()
- **Решение:** Всегда вызывайте super первым

---

*Последнее обновление: 2025-01-16*
*Источник: DayZ AI_Bandits v1.0 (Workshop ID: 3628006769)*
