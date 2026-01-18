# Action System Complete

**Source:** raw_data/actions_examples/
**Analysis:** Complete action system patterns

---

## 🔍 Overview

DayZ uses **ActionManager** for all player interactions (crafting, building, eating, etc.).

---

## 📋 Action Base Class

```c
class ActionBase
{
    // Action metadata
    string m_DisplayName;
    string m_DisplayNameProgress;
    float m_Priority;

    // Can perform action?
    bool ActionCondition(PlayerBase player, ActionTarget target)

    // Start action
    void Start(ActionData action_data)

    // During action
    void OnUpdate(ActionData action_data)

    // End action
    void End(ActionData action_data)

    // Interrupt action
    void Interrupt(ActionData action_data)
}
```

---

## 📋 ActionData (Action Context)

```c
class ActionData
{
    PlayerBase m_Player;      // Who is performing
    Object m_Target;           // Target object
    ItemBase m_MainItem;       // Item in hands

    float m_Progress;          // 0.0 to 1.0
    bool m_Finished;           // Is complete?
}
```

---

## 📋 Create Custom Action

```c
class ActionCustomCraft extends ActionBase
{
    void ActionCustomCraft()
    {
        m_DisplayName = "#str_custom_craft";
        m_Priority = 0.5;
    }

    override void CreateConditionComponents()
    {
        // Conditions
        m_ConditionItem = new CCINonRuined();
        m_ConditionTarget = new CCTNone();
    }

    override bool ActionCondition(PlayerBase player, ActionTarget target)
    {
        // Check if player has required items
        if (!player.GetItemInHands())
            return false;

        return true;
    }

    override void Start(ActionData action_data)
    {
        // Start crafting animation
        action_data.m_Player.StartCommandAnim(ActionBase.CRAFT_ANIM);
    }

    override void OnUpdate(ActionData action_data)
    {
        // Update progress
        action_data.m_Progress += 0.01;

        if (action_data.m_Progress >= 1.0)
        {
            // Crafting complete
            OnComplete(action_data);
        }
    }

    void OnComplete(ActionData action_data)
    {
        // Create result item
        GetGame().CreateObject("CustomItem", action_data.m_Player.GetPosition());

        // Delete ingredient
        action_data.m_MainItem.Delete();
    }
}
```

---

## 📋 Register Action

```c
modded class ActionManager
{
    override void RegisterActions()
    {
        super.RegisterActions();

        // Register custom action
        RegisterAction(new ActionCustomCraft());
    }
}
```

---

## 📋 Action Conditions

```c
// Item conditions
CCINonRuined           // Item not ruined
CCIHasQuantity         // Item has quantity
CCIHasEnergy           // Item has energy

// Target conditions
CCTNone                // No target required
CCTObject              // Target is object
CCTNonRuined           // Target not ruined

// Player conditions
CCPNotRiddenByPlayer   // Player not riding
```

---

## 🔗 Related

→ [Actions](../topics/03_actions.md) - Action basics

---

*Last updated: 2025-01-18*
