# Auto Stack Management (Mod-derived)

**Status:** ⚠️ Mod-derived (требует проверки)
**Related:** [Inventory System](INVENTORY_SYSTEM.md) | [EntityAI Lifecycle](ENTITYAI_LIFECYCLE.md)

---

## ⚡ 30-Second Summary

Умная система автоматического складывания предметов с player-specific cooldowns и type-aware объединением.

---

## 🎯 5-Minute Solution: Simple Auto-Stack

### Basic stacking on pickup

```c
modded class ItemBase
{
    protected ref map<string, float> m_LastStackTime = new map<string, float>;

    override void OnInventoryEnter(Man player)
    {
        super.OnInventoryEnter(player);

        if (GetGame().IsServer())
        {
            TryAutoStack(player);
        }
    }

    void TryAutoStack(Man player)
    {
        string playerId = player.GetIdentity().GetId();
        float currentTime = GetGame().GetTickTime();

        // Check cooldown (60 seconds)
        if (m_LastStackTime.Find(playerId) != -1)
        {
            if (currentTime - m_LastStackTime.Get(playerId) < 60)
            {
                return;
            }
        }

        // Find stackable items
        array<EntityAI> items = new array<EntityAI>;
        PlayerBase.Cast(player).GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, items);

        foreach (EntityAI item : items)
        {
            if (item.GetType() == GetType() && item != this)
            {
                // Combine stacks
                CombineStacks(ItemBase.Cast(item));
                break;
            }
        }

        m_LastStackTime.Set(playerId, currentTime);
    }

    void CombineStacks(ItemBase target)
    {
        int thisQuantity = GetQuantity();
        int targetQuantity = target.GetQuantity();
        int maxQuantity = GetQuantityMax();

        int available = maxQuantity - targetQuantity;

        if (available > 0)
        {
            int transfer = Math.Min(thisQuantity, available);

            target.SetQuantity(targetQuantity + transfer);
            SetQuantity(thisQuantity - transfer);

            if (GetQuantity() <= 0)
            {
                GetGame().ObjectDelete(this);
            }
        }
    }
}
```

---

## 📚 Deep Dive

## Auto Stack - Proper (3639591480) - Intelligent Stacking

### Player-specific cooldown system

```c
modded class ItemBase
{
    protected ref map<string, float> PlayerStackCooldowns = new map<string, float>;
    const float STACK_COOLDOWN = 60.0; // 60 seconds

    bool CanAutoStackForPlayer(PlayerBase player)
    {
        string playerId = player.GetIdentity().GetId();
        float currentTime = GetGame().GetTickTime();

        if (PlayerStackCooldowns.Find(playerId) == -1)
        {
            return true; // No cooldown yet
        }

        float lastStackTime = PlayerStackCooldowns.Get(playerId);

        if (currentTime - lastStackTime >= STACK_COOLDOWN)
        {
            return true; // Cooldown expired
        }

        return false; // Still on cooldown
    }

    void UpdateCooldown(PlayerBase player)
    {
        string playerId = player.GetIdentity().GetId();
        float currentTime = GetGame().GetTickTime();

        PlayerStackCooldowns.Set(playerId, currentTime);
    }
}
```

**Source:** `K:\Работа\Dayz_WorckPlace\Mod\Примеры\3639591480\Addons\RDM_AutoStack\RDM_AutoStack\Scripts\4_World\Entities\ItemBase.c:3-4, 52-63`

### Smart item combination with type checking

```c
modded class ItemBase
{
    void AutoStack(PlayerBase player)
    {
        if (!CanAutoStackForPlayer(player))
            return;

        array<EntityAI> inventoryItems = new array<EntityAI>;
        player.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, inventoryItems);

        foreach (EntityAI inventoryItem : inventoryItems)
        {
            ItemBase targetItem = ItemBase.Cast(inventoryItem);

            if (!targetItem || targetItem == this)
                continue;

            // Check if items are stackable
            if (!IsStackable(targetItem))
                continue;

            // Check if same type
            if (targetItem.GetType() != GetType())
                continue;

            // Check if target has space
            if (targetItem.GetQuantity() >= targetItem.GetQuantityMax())
                continue;

            // Combine stacks
            int availableSpace = targetItem.GetQuantityMax() - targetItem.GetQuantity();
            int transferAmount = Math.Min(GetQuantity(), availableSpace);

            targetItem.AddQuantity(transferAmount);
            AddQuantity(-transferAmount);

            if (GetQuantity() <= 0)
            {
                GetGame().ObjectDelete(this);
                break;
            }
        }

        UpdateCooldown(player);
    }

    bool IsStackable(ItemBase item)
    {
        // Override in children for custom logic
        return true;
    }
}
```

**Source:** `K:\Работа\Dayz_WorckPlace\Mod\Примеры\3639591480\Addons\RDM_AutoStack\RDM_AutoStack\Scripts\4_World\Entities\ItemBase.c:68-117`

### Trigger on inventory enter

```c
modded class ItemBase
{
    override void OnInventoryEnter(Man player)
    {
        super.OnInventoryEnter(player);

        if (GetGame().IsServer())
        {
            PlayerBase playerBase = PlayerBase.Cast(player);

            // Auto-stack after short delay
            GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(AutoStack, 500, false, playerBase);
        }
    }
}
```

**Source:** `K:\Работа\Dayz_WorckPlace\Mod\Примеры\3639591480\Addons\RDM_AutoStack\RDM_AutoStack\Scripts\4_World\Entities\ItemBase.c:16-66`

---

## 💡 Examples

### Example 1: Stack ammo on pickup

```c
class AmmoBox_AutoStack extends AmmoBox
{
    override void OnInventoryEnter(Man player)
    {
        super.OnInventoryEnter(player);

        if (GetGame().IsServer())
        {
            PlayerBase pb = PlayerBase.Cast(player);

            // Find same ammo in inventory
            array<EntityAI> items = new array<EntityAI>;
            pb.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, items);

            foreach (EntityAI item : items)
            {
                AmmoBox ammo = AmmoBox.Cast(item);

                if (ammo && ammo != this && ammo.GetType() == GetType())
                {
                    // Combine ammo stacks
                    if (ammo.GetQuantity() < ammo.GetQuantityMax())
                    {
                        int space = ammo.GetQuantityMax() - ammo.GetQuantity();
                        int transfer = Math.Min(GetQuantity(), space);

                        ammo.SetQuantity(ammo.GetQuantity() + transfer);
                        SetQuantity(GetQuantity() - transfer);

                        if (GetQuantity() <= 0)
                        {
                            GetGame().ObjectDelete(this);
                            break;
                        }
                    }
                }
            }
        }
    }
}
```

### Example 2: Ignore specific items from stacking

```c
class MedicalItem_NoAutoStack extends ItemBase
{
    override bool IsStackable(ItemBase item)
    {
        // Don't stack medical items
        return false;
    }
}
```

---

## 🔗 Related Topics

→ [Inventory System](INVENTORY_SYSTEM.md) - EnumerateInventory patterns
→ [EntityAI Lifecycle](ENTITYAI_LIFECYCLE.md) - OnInventoryEnter hook
→ [Storage System](STORAGE_SYSTEM.md) - Item quantity management

---

## ⚠️ SOURCE VERIFICATION

**Статус:** ⚠️ Mod-derived

**Что проверено:**
- ✅ Player-specific cooldowns: ItemBase.c:3-4, 52-63
- ✅ Smart combination logic: ItemBase.c:68-117
- ✅ OnInventoryEnter trigger: ItemBase.c:16-66

**Что НЕ проверено:**
- ❌ Работает ли без Auto Stack мода
- ❌ Производительность с большим количеством предметов
- ❌ Конфликты с другими модами инвентаря

---

## 💡 Tips

### Best Practices

1. **Используйте cooldown** чтобы предотвратить спам stacking
2. **Проверяйте max quantity** перед объединением
3. **Удаляйте пустые стаканы** автоматически
4. **Используйте CallQueue** для задержки (500ms recommended)
5. **Проверяйте IsServer()** перед выполнением stacking

### Performance Considerations

- **EnumerateInventory** может быть медленным с большим инвентарём
- **Используйте BREAK** после успешного stack для оптимизации
- **Ограничьте cooldown** минимум 30-60 секунд
- **Рассмотрите BATCHING** для multiple items

### Common Pitfalls

- ❌ **Забыть проверить IsServer()** → дублирование на клиенте
- ❌ **Не удалять пустые стаканы** → засорение инвентаря
- ❌ **Игнорировать max quantity** → integer overflow
- ❌ **Стекать разные типы** → потеря предметов

---

*Последнее обновление: 2025-01-16*
*Источник: Auto Stack - Proper (3639591480)*
