# Teleportation Systems

**Status:** ⚠️ Mod-derived (требует проверки)
**Related:** [Advanced Modding](11_advanced_modding.md) | [JSON Parsing](../reference/JSON_PARSING.md)

---

## ⚡ 30-Second Summary

Телепортация в DayZ: JSON конфигурация точек, баланс чек, paid teleportation. Полная документация: [TELEPORTATION_SYSTEMS.md](../reference/TELEPORTATION_SYSTEMS.md)

---

## 🎯 5-Minute Solution: Простая телепортация

### Step 1: Создайте JSON конфигурацию

**Файл:** `$profile:teleports.json`

```json
[
    {
        "Name": "Base",
        "Position": [5000, 0, 5000],
        "Cost": 0,
        "Description": "Главная база"
    },
    {
        "Name": "Trader",
        "Position": [6000, 0, 6000],
        "Cost": 100,
        "Description": "Трейдер (100 монет)"
    },
    {
        "Name": "Airport",
        "Position": [7000, 0, 7000],
        "Cost": 500,
        "Description": "Аэропорт (500 монет)"
    }
]
```

### Step 2: Создайте класс точки телепортации

```c
class TeleportNode
{
    string Name;
    vector Position;
    int Cost;
    string Description;
}
```

### Step 3: Создайте менеджер телепортации

```c
class TeleportManager
{
    protected ref array<ref TeleportNode> m_Nodes = new array<ref TeleportNode>;

    void LoadNodes()
    {
        string filePath = "$profile:teleports.json";

        if (FileExist(filePath))
        {
            JsonFileLoader<array<ref TeleportNode>>::JsonLoadFile(filePath, m_Nodes);
            Print("Loaded " + m_Nodes.Count() + " teleport nodes");
        }
        else
        {
            Print("No teleport config found at " + filePath);
        }
    }

    array<TeleportNode> GetNodes()
    {
        return m_Nodes;
    }

    bool CanTeleport(PlayerBase player, TeleportNode node)
    {
        // Проверяем баланс
        if (node.Cost > 0)
        {
            int balance = GetPlayerBalance(player);

            if (balance < node.Cost)
            {
                Print("Not enough currency. Need: " + node.Cost);
                return false;
            }
        }

        return true;
    }

    void TeleportPlayer(PlayerBase player, TeleportNode node)
    {
        if (!CanTeleport(player, node))
            return;

        // Списываем стоимость
        if (node.Cost > 0)
        {
            DeductCurrency(player, node.Cost);
        }

        // Телепортируем
        player.SetPosition(node.Position);

        // Оповещаем
        Print("Teleported to " + node.Name);
    }

    int GetPlayerBalance(PlayerBase player)
    {
        // Реализуйте систему валюты
        // Например, считаем монеты в инвентаре
        return 1000; // Заглушка
    }

    void DeductCurrency(PlayerBase player, int amount)
    {
        // Реализуйте списание валюты
    }
}
```

### Step 4: Создайте Action для телепортации

```c
class ActionTeleport: ActionSingleUseBase
{
    protected ref TeleportManager m_Manager;
    protected TeleportNode m_Node;

    void ActionTeleport(TeleportManager manager, TeleportNode node)
    {
        m_Manager = manager;
        m_Node = node;
    }

    override void CreateConditionComponents()
    {
        m_ConditionItem = new CCINone;
        m_ConditionTarget = new CCTNone;
    }

    override string GetText()
    {
        return "Teleport to " + m_Node.Name;
    }

    override void OnExecuteClient(ActionData action_data)
    {
        super.OnExecuteClient(action_data);
    }

    override void OnExecuteServer(ActionData action_data)
    {
        PlayerBase player = action_data.m_Player;
        m_Manager.TeleportPlayer(player, m_Node);
    }
}
```

---

## 📚 Deep Dive

### Advanced: Paid Teleportation (как в GuideSystem)

**GuideSystem** использует:
- JSON загрузку из `$profile:guides.json`
- Server-side GuideManager
- Paid teleportation с баланс чеком

**Пример JSON structure:**
```json
{
    "Guides": [
        {
            "Name": "Cherno",
            "Position": [6500, 0, 2500],
            "Cost": 100,
            "Currency": "Coins"
        }
    ]
}
```

**Полная документация:** [TELEPORTATION_SYSTEMS.md](../reference/TELEPORTATION_SYSTEMS.md)

---

## 💡 Examples

### Пример 1: Бесплатная телепортация

```c
void TeleportPlayerFree(PlayerBase player, vector position)
{
    // Проверяем что игрок в безопасности
    if (IsInCombat(player))
    {
        Print("Cannot teleport in combat!");
        return;
    }

    // Телепортируем
    player.SetPosition(position);
}

bool IsInCombat(PlayerBase player)
{
    // Проверяем最近的 урон
    float lastDamageTime = player.GetLastDamageTime();
    float currentTime = GetGame().GetTickTime();

    return (currentTime - lastDamageTime) < 300; // 5 минут
}
```

### Пример 2: Телепортация по запросу

```c
// Команда чата: /teleport Base
override bool OnChat(PlayerBase player, string message)
{
    if (message.StartsWith("/teleport "))
    {
        string location = message.Substring(10); // После "/teleport "

        TeleportNode node = FindNode(location);

        if (node)
        {
            m_TeleportManager.TeleportPlayer(player, node);
            return true;
        }
    }

    return false;
}

TeleportNode FindNode(string name)
{
    foreach (TeleportNode node : m_TeleportManager.GetNodes())
    {
        if (node.Name == name)
            return node;
    }

    return null;
}
```

### Пример 3: Group teleportation

```c
void TeleportGroup(PlayerBase leader, TeleportNode node)
{
    // Получаем все группы
    array<PlayerBase> groupMembers = GetGroupMembers(leader);

    // Проверяем баланс лидера
    int totalCost = node.Cost * groupMembers.Count();

    if (!m_TeleportManager.CanTeleport(leader, totalCost))
        return;

    // Телепортируем всю группу
    foreach (PlayerBase member : groupMembers)
    {
        m_TeleportManager.TeleportPlayer(member, node.Position);
    }
}

array<PlayerBase> GetGroupMembers(PlayerBase leader)
{
    // Реализуйте получение группы
    array<PlayerBase> members = new array<PlayerBase>;
    members.Insert(leader);
    return members;
}
```

---

## 🔗 Related Topics

→ [JSON Parsing](../reference/JSON_PARSING.md) - Работа с JSON
→ [Advanced Modding](11_advanced_modding.md) - Обзор mechanics
→ [Storage](14_storage.md) - Сохранение точек телепортации

---

## ⚠️ SOURCE VERIFICATION

**Status:** ⚠️ Mod-derived

**Источник:** Guide_System (3595854441)

**Что проверено:**
- ✅ JSON загрузка работает
- ✅ Paid teleportation работает
- ✅ Server-side менеджер работает

**Что НЕ проверено:**
- ❌ Работает ли без GuideSystem мода
- ❌ Безопасность телепортации (duping, glitching)

---

## 💡 Tips

### Best Practices:

1. **Используйте JSON конфигурацию** для легкого добавления точек
2. **Проверяйте баланс** перед телепортацией
3. **Проверяйте combat** чтобы избежать abuse
4. **Оповещайте игрока** о телепортации
5. **Логируйте телепортации** для античита

### Safety Checks:

```c
bool IsSafeToTeleport(PlayerBase player)
{
    // Не в транспорте
    if (player.GetCommand_Vehicle())
        return false;

    // Не в combate
    if (IsInCombat(player))
        return false;

    // Не unconscious
    if (player.IsUnconscious())
        return false;

    // Not restrained
    if (player.IsRestrained())
        return false;

    return true;
}
```

### Common Pitfalls:

- ❌ **Игнорировать combat check** → abuse
- ❌ **Забыть проверить баланс** → free teleportation
- ❌ **Телепортировать в транспорте** → glitching
- ❌ **Не логировать** → невозможно отследить abuse

---

*Последнее обновление: 2025-01-16*
*Источник: Guide_System (3595854441)*
