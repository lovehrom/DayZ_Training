# Faction Systems

**Status:** ⚠️ Mod-derived
**Related:** [Storage](14_storage.md) | [NPC](15_zombie_ai.md)

---

## ⚡ 30-Second Summary

Система фракций: 5 фракций (Hive, Blu, Gialli, Verde, Rossi), reputation, elite variants, disease modifiers.

---

## 🎯 5-Minute Solution

### Faction config

```cpp
class CfgVehicles
{
    class Survivor;
    class HiveFaction: Survivor
    {
        faction = "Hive";
        reputation = 1000;
    };
}
```

### Faction check

```c
bool IsSameFaction(PlayerBase a, PlayerBase b)
{
    return a.GetFaction() == b.GetFaction();
}
```

---

## 💡 Examples

### Пример 1: Faction damage

```c
float CalculateDamage(PlayerBase attacker, PlayerBase target)
{
    float damage = baseDamage;

    if (IsSameFaction(attacker, target))
    {
        damage *= 0.1; // Friendly fire reduction
    }

    return damage;
}
```

### Пример 2: Reputation system

```c
void AddReputation(PlayerBase player, int amount)
{
    player.SetReputation(player.GetReputation() + amount);

    if (player.GetReputation() > 1000)
    {
        PromoteToElite(player);
    }
}
```

---

## 🔗 Related Topics

→ [Storage](14_storage.md) - Faction storage
→ [NPC](15_zombie_ai.md) - Custom guards
→ [Reference: Faction Systems](../reference/FACTION_SYSTEMS.md) - Полная документация

---

*Последнее обновление: 2025-01-17*
*Источник: TheHive (3572908305)*
