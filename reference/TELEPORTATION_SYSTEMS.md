# Teleportation Systems (Mod-derived)

**Status:** ⚠️ Mod-derived (требует проверки)
**Related:** [RPC System](RPC_NETWORKING.md) | [Storage System](STORAGE_SYSTEM.md)

---

## ⚡ 30-Second Summary

Системы телепортации: JSON координаты, RPC коммуникация, проверка баланса, платный доступ.

---

## 🎯 5-Minute Solution: Simple JSON Teleport

### JSON configuration

```json
{
    "guides": [
        {
            "name": "Cherno",
            "position": [6845.3, 5.2, 2541.7]
        },
        {
            "name": "Elektro",
            "position": [2456.1, 4.8, 5123.9]
        }
    ]
}
```

### Teleport command

```c
modded class PlayerBase
{
    static const int RPC_GUIDE_TELEPORT = 9387;

    void TeleportToGuide(vector position)
    {
        if (GetGame().IsServer())
        {
            SetPosition(position);
        }
        else
        {
            // Send RPC to server
            GetGame().RPCSingleParam(this, RPC_GUIDE_TELEPORT, new Param1<vector>(position), true, GetIdentity());
        }
    }
}
```

---

## 📚 Deep Dive

## Guide System (3595854441) - JSON Teleportation

### JSON-based node system

```c
class GuideManager
{
    protected ref array<ref GuideNode> m_Guides;

    void LoadGuides()
    {
        string filePath = "$profile:guides.json";

        if (FileExist(filePath))
        {
            JsonFileLoader<array<ref GuideNode>>::JsonLoadFile(filePath, m_Guides);
        }
    }
}

class GuideNode
{
    string Name;
    vector Position;
}
```

**Source:** `K:\Работа\Dayz_WorckPlace\Mod\Примеры\3595854441\instruction\guides.json:1-10`

### Paid teleportation with balance check

```c
class GuideSystem
{
    const int TELEPORT_COST = 1000;

    bool CanTeleport(PlayerBase player)
    {
        // Check player balance
        int balance = GetPlayerCurrency(player);

        if (balance < TELEPORT_COST)
        {
            NotificationSystem.SendNotification(player, "Недостаточно средств!");
            return false;
        }

        return true;
    }

    void ProcessTeleport(PlayerBase player, vector position)
    {
        if (CanTeleport(player))
        {
            DeductCurrency(player, TELEPORT_COST);
            player.SetPosition(position);
        }
    }
}
```

**Source:** `K:\Работа\Dayz_WorckPlace\Mod\Примеры\3595854441\addons\guideClient\guideClient\scripts\4_World\Entities\ManBase\playerbase.c:50-75`

### RPC communication

```c
modded class PlayerBase
{
    static const int ACTION_GUIDE = 9387;

    override void OnRPC(PlayerIdentity sender, Object target, int rpc_type, ParamsReadContext ctx)
    {
        super.OnRPC(sender, target, rpc_type, ctx);

        if (rpc_type == ACTION_GUIDE)
        {
            Param1<vector> positionParam = new Param1<vector>(vector.Zero);
            if (ctx.Read(positionParam))
            {
                vector position = positionParam.param1;
                SetPosition(position);
            }
        }
    }
}
```

**Source:** `K:\Работа\Dayz_WorckPlace\Mod\Примеры\3595854441\addons\guideClient\guideClient\scripts\4_World\Entities\ManBase\playerbase.c:3-13`

### Server-side initialization

```c
modded class MissionServer
{
    override void OnMissionStart()
    {
        super.OnMissionStart();

        // Initialize GuideManager
        GuideManager guideManager = new GuideManager();
        guideManager.LoadGuides();
    }
}
```

**Source:** `K:\Работа\Dayz_WorckPlace\Mod\Примеры\3595854441\addons\guideClient\guideClient\scripts\5_Mission\missionServer.c:1-9`

---

## Pack a Punch Machine (3569453138) - External Payment

### External payment integration

**Features:**
- External verification through danksserver.com
- Discord integration for payment verification
- Donation-based teleportation access

**Source:** `K:\Работа\Dayz_WorckPlace\Mod\Примеры\3569453138\README.txt:1`

---

## 💡 Examples

### Example 1: Free teleportation without payment

```c
class FreeTeleportSystem
{
    void TeleportPlayer(PlayerBase player, vector position)
    {
        if (GetGame().IsServer())
        {
            player.SetPosition(position);

            // Optional: Play teleport effect
            SEffectManager.PlaySound("teleport_soundset", player.GetPosition());
        }
    }
}
```

### Example 2: Multiple teleportation points

```json
{
    "locations": [
        {
            "id": "trader_zone",
            "name": "Trader Zone",
            "position": [5000, 0, 5000],
            "cost": 0
        },
        {
            "id": "pvp_zone",
            "name": "PvP Zone",
            "position": [3000, 0, 7000],
            "cost": 500
        }
    ]
}
```

---

## 🔗 Related Topics

→ [RPC System](RPC_NETWORKING.md) - RPC communication patterns
→ [Storage System](STORAGE_SYSTEM.md) - Saving player data
→ [Custom RPC](CUSTOM_RPC.md) - Custom RPC IDs > 10000

---

## ⚠️ SOURCE VERIFICATION

**Статус:** ⚠️ Mod-derived

**Что проверено:**
- ✅ JSON-based guide system: GuideSystem (3595854441)
- ✅ RPC teleport communication: playerbase.c:3-13
- ✅ Balance check before teleport: playerbase.c:50-75
- ✅ Server-side initialization: missionServer.c:1-9

**Что НЕ проверено:**
- ❌ Работает ли без GuideSystem мода
- ❌ Совместимость с анти-чит системами
- ❌ Производительность при большом количестве точек

---

## 💡 Tips

### Best Practices

1. **Всегда проверяйте баланс** до телепортации
2. **Используйте RPC** для клиент-сервер коммуникации
3. **Храните координаты в JSON** для лёгкого редактирования
4. **Добавьте cooldown** чтобы предотвратить спам
5. **Проверяйте зону** на безопасность перед телепортацией

### Security Considerations

- **Валидируйте координаты** на сервере (не доверяйте клиенту)
- **Проверяйте баланс** на серверной стороне
- **Логируйте телепортации** для отладки
- **Используйте уникальные RPC ID** (> 10000)

---

*Последнее обновление: 2025-01-16*
*Источники: GuideSystem (3595854441), Pack a Punch Machine (3569453138)*
