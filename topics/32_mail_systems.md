# Mail Systems for Beginners

**Status:** ✅ Beginner Guide
**Difficulty:** Intermediate
**Time to Implement:** 2-3 hours
**Related:** [Mail System Reference](../../reference/MAIL_SYSTEM.md) | [Mail Example](../../examples/mail_system/)

---

## ⚡ 30-Second Summary

Mail System lets players send items and messages to each other through mailbox entities, even when the recipient is offline. Like email, but for in-game items!

---

## 🎯 What You'll Learn

1. **What is Mail?** - Asynchronous item delivery
2. **Mailbox Entities** - Physical mailboxes in game world
3. **Sending Mail** - How to send items/messages
4. **Receiving Mail** - How to collect mail
5. **Auction Integration** - Automatic auction delivery

---

## 💡 What is Mail System?

**Mail System** = Asynchronous communication

**How it works:**
1. Player A puts items in mailbox + addresses to Player B
2. Mail stored in JSON file (even if Player B is offline)
3. Player B logs in later and opens mailbox
4. Player B receives items

**Use Cases:**
- ✅ Send items to offline players
- ✅ Auction winnings delivery
- ✅ Quest rewards distribution
- ✅ P2P trading without meeting

---

## 🔧 Basic Setup

### Step 1: Create Mailbox Entity

**config.cpp:**
```cpp
class CfgVehicles
{
    class Container_Base;

    class MailBox: Container_Base
    {
        scope = 2;
        displayName = "Mailbox";
        model = "\dz\structures\commercial\mailbox\mailbox.p3d";
        inventorySlot[] = {"Cargo"};
    };
}
```

---

### Step 2: Define Mail Data

```c
class MailEntry
{
    string Id;                  // Unique ID
    string SenderName;          // Who sent it
    string Subject;             // Subject line
    string Message;             // Message text
    bool Read;                  // Has recipient read?
    ref array<string> Items;    // Attached items

    void MailEntry()
    {
        Id = "MAIL_" + GetGame().GetTickTime().ToString();
        Read = false;
        Items = new array<string>;
    }
}
```

---

### Step 3: Send Mail

```c
void SendMail(PlayerIdentity sender, string recipientSID, string subject, string message, array<ItemBase> items)
{
    // Create mail
    MailEntry mail = new MailEntry();
    mail.SenderName = sender.GetName();
    mail.Subject = subject;
    mail.Message = message;

    // Attach items
    foreach (ItemBase item : items)
    {
        mail.Items.Insert(item.GetType());
        GetGame().ObjectDelete(item);  // Remove from sender
    }

    // Save to recipient inbox
    SaveMailToInbox(recipientSID, mail);

    // Add to sender outbox
    AddToOutbox(sender.GetPlainId(), mail);

    Print("[Mail] Sent to " + recipientSID);
}
```

---

### Step 4: Receive Mail

```c
void ReceiveMail(string mailID)
{
    string playerID = GetGame().GetPlayer().GetIdentity().GetPlainId();

    // Load inbox
    array<ref MailEntry> inbox = LoadInbox(playerID);

    // Find mail
    MailEntry mail = FindMailByID(inbox, mailID);
    if (!mail)
    {
        NotificationSystem.Notify(this, "Mail not found!");
        return;
    }

    // Mark as read
    mail.Read = true;
    SaveInbox(playerID, inbox);

    // Spawn items
    PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());

    foreach (string itemClass : mail.Items)
    {
        ItemBase item = player.GetHumanInventory().CreateInInventory(itemClass);

        if (!item)
        {
            // Inventory full, drop on ground
            vector pos = player.GetPosition();
            item = ItemBase.Cast(GetGame().CreateObject(itemClass, pos));
        }
    }

    // Show notification
    NotificationSystem.Notify(player, "Received from: " + mail.SenderName);
    NotificationSystem.Notify(player, mail.Subject);
}
```

---

## 🔄 Mail Persistence

### Save Inbox

```c
void SaveInbox(string playerID, array<ref MailEntry> inbox)
{
    string mailDir = "$profile:Mail/";

    // Create directory
    if (!FileExist(mailDir))
        MakeDirectory(mailDir);

    // Save to JSON
    string filePath = mailDir + playerID + "_inbox.json";
    JsonFileLoader<array<ref MailEntry>>::JsonSaveFile(filePath, inbox);
}
```

### Load Inbox

```c
array<ref MailEntry> LoadInbox(string playerID)
{
    string filePath = "$profile:Mail/" + playerID + "_inbox.json";

    if (!FileExist(filePath))
    {
        // Create empty inbox
        array<ref MailEntry> inbox = new array<ref MailEntry>;
        SaveInbox(playerID, inbox);
        return inbox;
    }

    // Load from JSON
    array<ref MailEntry> inbox = new array<ref MailEntry>;
    JsonFileLoader<array<ref MailEntry>>::JsonLoadFile(filePath, inbox);
    return inbox;
}
```

---

## 🎮 Integration Examples

### Auction Integration

```c
// Auction ends, send item to winner
void AuctionEnd(string winnerSID, string itemClass, int bidAmount)
{
    // Create auction mail
    MailEntry mail = new MailEntry();
    mail.SenderName = "Auction System";
    mail.Subject = "Auction Win!";
    mail.Message = "You won with bid: " + bidAmount.ToString();
    mail.Items.Insert(itemClass);

    // Send to winner
    SendMail(NULL, winnerSID, mail.Subject, mail.Message, NULL);
}
```

### Quest Rewards

```c
// Quest completion sends reward via mail
void CompleteQuest(PlayerBase player, string questName, array<string> rewardItems)
{
    string playerID = player.GetIdentity().GetPlainId();

    // Create reward mail
    MailEntry mail = new MailEntry();
    mail.SenderName = "Quest System";
    mail.Subject = "Quest Complete: " + questName;
    mail.Message = "Congratulations!";

    foreach (string item : rewardItems)
    {
        mail.Items.Insert(item);
    }

    // Send
    SaveMailToInbox(playerID, mail);

    NotificationSystem.Notify(player, "Quest reward sent to mailbox!");
}
```

---

## ⚠️ Common Issues

### Issue 1: Mail Not Saving

**Check:**
1. `$profile:Mail/` directory exists
2. File permissions allow writing
3. `JsonSaveFile()` succeeds

**Debug:**
```c
bool saved = JsonFileLoader<array<ref MailEntry>>::JsonSaveFile(filePath, inbox);
if (!saved)
    Print("[Mail] ERROR: Failed to save!");
```

### Issue 2: Items Not Spawning

**Check:**
1. Item class name is valid
2. Player has inventory space
3. `CreateInInventory()` succeeds

**Debug:**
```c
ItemBase item = player.GetHumanInventory().CreateInInventory(itemClass);
if (!item)
    Print("[Mail] Failed to spawn " + itemClass);
```

---

## 📝 Quick Reference

### Send Mail

```c
GetRPCManager().SendRPC("Mail", "SendMail",
    new Param4<string, string, string, array<ItemBase>>(recipientSID, subject, message, items),
    true,
    senderIdentity);
```

### Receive Mail

```c
GetRPCManager().SendRPC("Mail", "ReceiveMail",
    new Param1<string>(mailID),
    true,
    playerIdentity);
```

---

**Last Updated:** 2025-01-18
**Status:** ✅ Complete for Beginners
**Next:** See [Mail System Reference](../../reference/MAIL_SYSTEM.md) for advanced features
