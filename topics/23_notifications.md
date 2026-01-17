# Notification System

**Status:** ⚠️ Mod-derived
**Related:** [Network](08_network.md) | [RPC](../reference/RPC_NETWORKING.md)

---

## ⚡ 30-Second Summary

Notifications: GetGame().GetPlayers(), RPCSingleParam, Param4<string, string, string, float> для уведомлений, broadcast всем игрокам.

---

## 🎯 5-Minute Solution

### Send notification

```c
class NotificationSystem
{
    static void Notify(PlayerBase player, string message)
    {
        Param1<string> msg = new Param1<string>(message);
        GetGame().RPCSingleParam(player, ERPCs.RPC_USER_ACTION_MESSAGE, msg, true, player.GetIdentity());
    }
}
```

### Broadcast

```c
void BroadcastNotification(string message)
{
    array<Man> players = new array<Man>;
    GetGame().GetPlayers(players);

    foreach (Man man : players)
    {
        PlayerBase player = PlayerBase.Cast(man);
        if (player)
        {
            NotificationSystem.Notify(player, message);
        }
    }
}
```

---

## 💡 Examples

### Пример 1: Achievement

```c
void NotifyAchievement(PlayerBase player, string achievement)
{
    string msg = "Achievement: " + achievement;
    NotificationSystem.Notify(player, msg);
}
```

### Пример 2: System message

```c
void NotifyServer(string message)
{
    BroadcastNotification("[SERVER] " + message);
}
```

---

## 🔗 Related Topics

→ [Network](08_network.md) - RPC basics
→ [Reference: Notifications](../reference/NOTIFICATION_SYSTEM.md) - Полная документация

---

*Последнее обновление: 2025-01-17*
*Источник: NotificationsSystem (3624261153)*
