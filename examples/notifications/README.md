# Notification System Example

**Status:** ✅ Working Example
**Source:** NotificationsSystem (3624261153)
**Complexity:** Simple

---

## ⚡ 30-Second Summary

Server-to-client notifications with RPC, broadcast to all players, colored messages, and custom icons.

---

## 🎯 Features

- ✅ **Client Notifications** - Send to single player
- ✅ **Broadcast** - Send to all players
- ✅ **Custom Messages** - Any text
- ✅ **RPC Communication** - Server → Client

---

## 💡 Key Concepts

### RPC Notification

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

## 🔗 Related Topics

→ [Notifications](../topics/23_notifications.md) - Mechanics
→ [Network](../topics/08_network.md) - RPC basics

---

*Последнее обновление: 2025-01-18*
