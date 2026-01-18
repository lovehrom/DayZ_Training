# Mail System Example

**Status:** ✅ Working Example
**Complexity:** Intermediate
**Time to Implement:** 2-3 hours
**Related:** [Mail System Reference](../../reference/MAIL_SYSTEM.md) | [Mail Topic](../../topics/32_mail_systems.md)

---

## ⚡ 30-Second Summary

Complete working Mail System that allows players to send items and messages to each other through mailbox entities. Includes JSON persistence, auction integration, and offline delivery.

---

## 🎯 Features

- ✅ **Mailbox Entities** - Physical mailboxes in game world
- ✅ **Send Mail** - Send items and messages to other players
- ✅ **Receive Mail** - Collect mail from mailbox
- ✅ **Offline Delivery** - Mail persists even when recipient is offline
- ✅ **JSON Persistence** - Mail saved in `$profile:Mail/`
- ✅ **Auction Integration** - Automatic auction winnings delivery
- ✅ **Quest Rewards** - Send quest rewards via mail
- ✅ **Item Attachments** - Send items with mail

---

## 📁 File Structure

```
mail_system/
├── README.md                        ← This file
├── config.cpp                       ← Mailbox entity definition
└── scripts/
    ├── 3_Game/
    │   ├── MailData.c              ← Mail data structures
    │   ├── MailStorage.c           ← JSON persistence
    │   ├── MailServer.c            ← Server-side mail handling
    │   └── MailRPC.c               ← RPC handlers
    └── 4_World/
        └── Classes/
            ├── Entities/
            │   └── MailBox.c       ← Mailbox entity
            └── Action/
                └── ActionOpenMailBox.c ← Open mailbox action
```

---

## 💡 Installation

### Step 1: Copy Files

Copy `mail_system` folder to your DayZ server.

### Step 2: Update Server Config

Add to `server.cfg`:
```
@mail_system
```

### Step 3: Place Mailboxes

Use Eden Editor or `init.c`:
```c
vector pos = "5000 0 5000";
Object mailbox = GetGame().CreateObject("MailBox", pos);
```

---

## 🎮 Usage

### Send Mail

Player A sends mail to Player B:

```c
// Client: Send mail request
GetRPCManager().SendRPC("MailSystem", "SendMail",
    new Param4<string, string, string, int>(recipientSID, subject, message, itemCount),
    true,
    player.GetIdentity());
```

### Receive Mail

Player B opens mailbox:

```c
// Client: Request inbox
GetRPCManager().SendRPC("MailSystem", "RequestInbox",
    NULL,
    true,
    player.GetIdentity());

// Server responds with mail list
// Client selects mail to receive
```

---

## 🔗 Integration Examples

### Auction Integration

```c
// Auction ends, send item to winner
void SendAuctionWin(string winnerSID, array<ItemBase> items, int bidAmount)
{
    MailServer.SendAuctionParcel(winnerSID, items, bidAmount);
}
```

### Quest Rewards

```c
// Quest completion
void CompleteQuest(PlayerBase player, array<string> rewardItems)
{
    MailServer.SendQuestReward(player.GetIdentity().GetPlainId(), rewardItems);
}
```

---

**Last Updated:** 2025-01-18
**Status:** ✅ Working Example
