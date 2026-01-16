# Custom RPC Systems

**Status:** ⚠️ Mod-derived (требует проверки)
**Related:** [RPC Networking](RPC_NETWORKING.md) | [GetGame API](GETGAME_API.md)

---

## ⚡ 30-Second Summary

Кастомные RPC системы для broadcast сообщений, звуков и уведомлений всем игрокам на сервере.

---

## 🎯 5-Minute Solution: Broadcast RPC всем игрокам

### Custom RPC ID

```c
static const int RPC_CUSTOM_SOUND = 77801;  // Уникальный ID > 10000
```

### Broadcast функция

```c
void BroadcastToAllPlayers()
{
    array<Man> players = new array<Man>;
    GetGame().GetPlayers(players);

    foreach (Man man : players)
    {
        PlayerBase pb = PlayerBase.Cast(man);
        if (pb && pb.GetIdentity())
        {
            GetGame().RPCSingleParam(pb, RPC_CUSTOM_SOUND, null, true, pb.GetIdentity());
        }
    }
}
```

---

## 📚 Deep Dive

## SliverWeapons (3644228096) - Global Sound Broadcast

### Custom RPC ID

```c
static const int RPC_ADMINHAMMER_KILL_SOUND = 77801;
```

### Broadcast sound всем игрокам

```c
void BroadcastKillSound()
{
    array<Man> players = new array<Man>;
    GetGame().GetPlayers(players);

    foreach (Man man : players)
    {
        PlayerBase pb;
        if (Class.CastTo(pb, man))
        {
            GetGame().RPCSingleParam(pb, RPC_ADMINHAMMER_KILL_SOUND, null, true, pb.GetIdentity());
        }
    }
}
```

## NotificationsSystem (3624261153) - Player Notifications

### Param4 для уведомлений

```c
void SendNotificationToAll(string title, string text, string icon, float time)
{
    array<Man> players = new array<Man>;
    GetGame().GetPlayers(players);

    Param4<string, string, string, float> p = new Param4<string, string, string, float>(title, text, icon, time);

    foreach (Man m : players)
    {
        PlayerBase pBase = PlayerBase.Cast(m);
        if (pBase && pBase.GetIdentity())
        {
            GetGame().RPCSingleParam(pBase, RPC_SEND_NOTIF, p, true, pBase.GetIdentity());
        }
    }
}
```

---

## 💡 Examples

### Example 1: Scheduled notification

```c
modded class MissionServer
{
    void CheckScheduledNotifications()
    {
        int year, month, day, hour, minute;
        GetGame().GetWorld().GetDate(year, month, day, hour, minute);

        if (hour == 6 && minute == 0)
        {
            SendNotificationToAll("Доброе утро!", "Сервер перезапустится через 1 час", "set:dayz_gui icon:info", 30.0);
        }
    }
}
```

---

## 🔗 Related Topics

→ [RPC Networking](RPC_NETWORKING.md) - Vanilla RPC система
→ [GetGame API](GETGAME_API.md) - GetGame() методы
→ [Notification System](NOTIFICATION_SYSTEM.md) - Player notifications

---

## ⚠️ SOURCE VERIFICATION

**Статус:** ⚠️ Mod-derived

**Что проверено:**
- ✅ SliverWeapons: RPC_ADMINHAMMER_KILL_SOUND = 77801, GetGame().GetPlayers() broadcast
- ✅ NotificationsSystem: Param4<string, string, string, float>, RPC_SEND_NOTIF
- ✅ Custom RPC ID > 10000 для избежания конфликтов

**Что НЕ проверено:**
- ❌ Полный список vanilla RPC IDs
- ❌ Работает ли без модов

---

## 🚨 Troubleshooting

**Ошибка:** RPC не доходит
- **Решение:** Используйте уникальный ID > 10000

**Ошибка:** GetIdentity() возвращает NULL
- **Решение:** Проверяйте `if (player.GetIdentity())` перед отправкой

---

## 💡 Tips

### Best Practices

1. **Уникальные RPC ID:** Используйте ID > 10000
2. **Guaranteed delivery:** Для критичных сообщений используйте `true`
3. **Rate limiting:** Не спамьте RPC

---

*Последнее обновление: 2025-01-16*
*Источники: SliverWeapons (3644228096), NotificationsSystem (3624261153)*
