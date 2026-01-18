# Mail System - Complete Technical Reference

**Status:** ✅ Complete Reference
**Source:** hNz Mail System (Mod ID: 3592170970)
**Complexity:** Advanced
**Related:** [Mail Topic Guide](../../topics/32_mail_systems.md) | [Mail Example](../../examples/mail_system/)

---

## ⚡ 30-Second Summary

Mail System enables asynchronous communication between players through mailbox entities. Players can send items, messages, and auction parcels to each other even when the recipient is offline. Integrates with banking and auction systems for P2P trading.

---

## 🎯 Key Concepts

### What is a Mail System?

**Mail System** = Asynchronous item/message delivery

**Real-world analogy:**
- Like email, but for in-game items
- Sender puts items in mailbox → Recipient collects later
- Works even when recipient is offline

**DayZ Use Cases:**
- Send items to offline players
- Auction winnings delivery
- Quest rewards distribution
- P2P trading without meeting

### Mail System Components

**1. Mailbox Entities**
- Physical mailboxes in game world
- Static (placed by admin) or Kit (craftable by players)
- Player opens mailbox to access mail

**2. Mail Storage**
- JSON persistence per player
- Inbox (received mail)
- Outbox (pending/delivered mail)
- System notifications

**3. Mail Delivery**
- Asynchronous (immediate or delayed)
- Item attachment system
- Message text
- Sender/recipient tracking

**4. Auction Integration**
- Auction parcels
- Bid notifications
- Winning bidder delivery
- Automatic item transfer

---

## 📚 Core Architecture

### 1. Mailbox Entity Types

**Static Mailbox:**
```c
class hNz_MailBox_Static: Container_Base
{
    // Placed by admin
    // Permanent location
    // Infinite capacity
}
```

**Mailbox Kit:**
```c
class hNz_MailBox_Kit: ItemBase
{
    // Craftable by players
    // Places static mailbox
    // Single-use
}
```

**Portable Mailbox (Held):**
```c
class hNz_MailBox: Container_Base
{
    // Can be placed in inventory
    // Access mail anywhere
    // Limited capacity
}
```

---

### 2. Mail Data Structure

**Mail Entry:**
```c
class HNZ_MailEntry
{
    string Id;                  // Unique mail ID
    string SenderSID;           // Sender Steam64 ID
    string SenderName;          // Sender name
    string RecipientSID;        // Recipient Steam64 ID
    string RecipientName;       // Recipient name
    string Subject;             // Mail subject
    string Message;             // Mail body text
    bool Read;                  // Has recipient read?
    int SentTime;               // Unix timestamp sent
    int ExpireTime;             // Unix timestamp expires (optional)
    ref array<ItemBase> Items;  // Attached items
    string SystemOriginFile;    // System tag (auctions, quests, etc.)
}
```

**Outbox Entry:**
```c
class HNZ_MailOutRow
{
    string Id;                  // Mail ID
    string RecipientName;       // Recipient name
    string Subject;             // Subject
    int SentTime;               // Time sent
    bool Delivered;             // Has recipient received?
    int ItemCount;             // Number of items attached
}
```

---

### 3. RPC System

**Source:** `HNZ_Mail_Core.c:14-46`

**Server RPCs (Client → Server):**
```c
// Send mail
HNZM_RPC_SEND           // Send mail to player
HNZM_RPC_RECV           // Receive/read mail
HNZM_RPC_RECV_BY_ID     // Receive specific mail by ID
HNZM_RPC_LIST           // List inbox
HNZM_RPC_OUTBOX_LIST    // List outbox
HNZM_RPC_CANCEL_SEND    // Cancel pending mail
HNZM_RPC_REQ_PLAYERS    // Request player list (for sending)

// Auction integration
HNZM_RPC_AUC_LIST       // List auction parcels
HNZM_RPC_AUC_ADD        // Create auction parcel
HNZM_RPC_AUC_OPEN_REQ   // Open auction parcel
HNZM_RPC_AUC_BID        // Bid on auction
HNZM_RPC_AUC_BUYOUT     // Buyout auction
```

**Client RPCs (Server → Client):**
```c
HNZM_RPC_UI_MSG         // UI feedback/notification
HNZM_RPC_LIST_REPLY     // Inbox list
HNZM_RPC_OUTBOX_REPLY   // Outbox list
HNZM_RPC_PLAYERS_REPLY  // Player list
HNZM_RPC_AUC_LIST_REPLY // Auction list
HNZM_RPC_AUC_OPEN       // Open auction UI
HNZM_RPC_AUC_STATE      // Auction enabled state
```

---

## 🔧 Implementation Guide

### Step 1: Create Mailbox Entity

**config.cpp:**
```cpp
class CfgPatches
{
    class MyMailSystem
    {
        units[] = {"MailBox_Static", "MailBox_Kit"};
        requiredAddons[] = {"DZ_Data"};
    };
};

class CfgVehicles
{
    class Container_Base;

    // Static mailbox (placed by admin)
    class MailBox_Static: Container_Base
    {
        scope = 2;
        displayName = "Mailbox";
        model = "\dz\structures\commercial\mailbox\mailbox.p3d";

        // Inventory
        inventorySlot[] = {"Cargo"};

        // Mailbox properties
        maximumCargo = 100;  // Max mail slots
    };

    // Mailbox kit (craftable)
    class MailBox_Kit: ItemBase
    {
        scope = 2;
        displayName = "Mailbox Kit";

        // Consumable (places mailbox)
        consumable = 1;
    };
}
```

---

### Step 2: Define Mail Data Structures

```c
// Mail Entry
class MailEntry
{
    string Id;
    string SenderSID;
    string SenderName;
    string RecipientSID;
    string Subject;
    string Message;
    bool Read;
    int SentTime;
    int ExpireTime;
    ref array<string> ItemClassNames;  // Attached items (class names)

    void MailEntry()
    {
        Id = GenerateMailID();
        SentTime = GetGame().GetTickTime();
        Read = false;
        ItemClassNames = new array<string>;
    }

    string GenerateMailID()
    {
        return "MAIL_" + SenderSID + "_" + SentTime.ToString() + "_" + Math.RandomInt(1000, 9999).ToString();
    }
}

// Mail Storage
class MailStorage
{
    static ref map<string, ref array<ref MailEntry>> m_Inboxes;
    static ref map<string, ref array<ref MailEntry>> m_Outboxes;

    static void LoadInbox(string playerID)
    {
        if (!m_Inboxes)
            m_Inboxes = new map<string, ref array<ref MailEntry>>;

        if (m_Inboxes.Contains(playerID))
            return;  // Already loaded

        string filePath = "$profile:Mail/" + playerID + "_inbox.json";

        if (FileExist(filePath))
        {
            JsonFileLoader<array<ref MailEntry>>::JsonLoadFile(filePath, m_Inboxes[playerID]);
        }
        else
        {
            m_Inboxes.Set(playerID, new array<ref MailEntry>);
        }
    }

    static void SaveInbox(string playerID)
    {
        if (!m_Inboxes || !m_Inboxes.Contains(playerID))
            return;

        string mailDir = "$profile:Mail/";
        if (!FileExist(mailDir))
            MakeDirectory(mailDir);

        string filePath = mailDir + playerID + "_inbox.json";
        JsonFileLoader<array<ref MailEntry>>::JsonSaveFile(filePath, m_Inboxes[playerID]);
    }
}
```

---

### Step 3: Send Mail

```c
class MailServer
{
    void SendMail(PlayerIdentity sender, string recipientSID, string subject, string message, array<ItemBase> items)
    {
        string senderSID = sender.GetPlainId();
        string senderName = sender.GetName();

        // Create mail entry
        MailEntry mail = new MailEntry();
        mail.SenderSID = senderSID;
        mail.SenderName = senderName;
        mail.RecipientSID = recipientSID;
        mail.Subject = subject;
        mail.Message = message;

        // Attach items
        if (items)
        {
            foreach (ItemBase item : items)
            {
                mail.ItemClassNames.Insert(item.GetType());

                // Move item to mailbox storage
                // In real implementation, items go to special cargo
                // For simplicity, we delete and recreate on receive
                GetGame().ObjectDelete(item);
            }
        }

        // Load recipient inbox
        MailStorage.LoadInbox(recipientSID);

        // Add mail to recipient inbox
        if (!MailStorage.m_Inboxes.Contains(recipientSID))
        {
            MailStorage.m_Inboxes.Set(recipientSID, new array<ref MailEntry>);
        }

        MailStorage.m_Inboxes[recipientSID].Insert(mail);

        // Save
        MailStorage.SaveInbox(recipientSID);

        // Add to sender outbox
        AddToOutbox(senderSID, mail);

        // Notify recipient if online
        PlayerBase recipientPlayer = FindPlayerBySID(recipientSID);
        if (recipientPlayer)
        {
            GetRPCManager().SendRPC("MailSystem", "ReceiveNotification",
                new Param2<string, string>(senderName, subject),
                true,
                recipientPlayer.GetIdentity());
        }

        Print("[Mail] Sent from " + senderName + " to " + recipientSID);
    }

    void AddToOutbox(string senderSID, MailEntry mail)
    {
        MailStorage.LoadOutbox(senderSID);

        if (!MailStorage.m_Outboxes.Contains(senderSID))
        {
            MailStorage.m_Outboxes.Set(senderSID, new array<ref MailEntry>);
        }

        MailStorage.m_Outboxes[senderSID].Insert(mail);
        MailStorage.SaveOutbox(senderSID);
    }
}
```

---

### Step 4: Receive Mail

```c
class MailClient
{
    void ReceiveMail(string mailID)
    {
        string playerID = GetGame().GetPlayer().GetIdentity().GetPlainId();

        // Load inbox
        MailStorage.LoadInbox(playerID);

        if (!MailStorage.m_Inboxes.Contains(playerID))
            return;

        array<ref MailEntry> inbox = MailStorage.m_Inboxes[playerID];

        // Find mail
        MailEntry mail = NULL;
        foreach (MailEntry entry : inbox)
        {
            if (entry.Id == mailID)
            {
                mail = entry;
                break;
            }
        }

        if (!mail)
        {
            NotificationSystem.Notify(this, "Mail not found!");
            return;
        }

        // Mark as read
        mail.Read = true;
        MailStorage.SaveInbox(playerID);

        // Spawn items
        PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());

        foreach (string itemClass : mail.ItemClassNames)
        {
            ItemBase item = player.GetHumanInventory().CreateInInventory(itemClass);

            if (!item)
            {
                // Inventory full, drop on ground
                vector pos = player.GetPosition();
                item = ItemBase.Cast(GetGame().CreateObject(itemClass, pos));
            }
        }

        // Show message
        NotificationSystem.Notify(player, "Received from: " + mail.SenderName);
        NotificationSystem.Notify(player, "Subject: " + mail.Subject);
        NotificationSystem.Notify(player, mail.Message);
    }
}
```

---

### Step 5: List Mail

```c
void RequestInboxList(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
{
    if (type != CallType.Server)
        return;

    string playerID = sender.GetPlainId();

    // Load inbox
    MailStorage.LoadInbox(playerID);

    if (!MailStorage.m_Inboxes.Contains(playerID))
    {
        // Create empty inbox
        MailStorage.m_Inboxes.Set(playerID, new array<ref MailEntry>);
    }

    array<ref MailEntry> inbox = MailStorage.m_Inboxes[playerID];

    // Convert to display rows
    array<ref MailListRow> rows = new array<ref MailListRow>;

    foreach (MailEntry mail : inbox)
    {
        MailListRow row = new MailListRow;
        row.Id = mail.Id;
        row.SenderName = mail.SenderName;
        row.Subject = mail.Subject;
        row.SentTime = mail.SentTime;
        row.Read = mail.Read;
        row.ItemCount = mail.ItemClassNames.Count();

        rows.Insert(row);
    }

    // Send to client
    GetRPCManager().SendRPC("MailSystem", "InboxListReply",
        new Param1<array<ref MailListRow>>(rows),
        true,
        sender);
}
```

---

## 🔄 Auction Integration

### Auction Parcel System

**Auction Lot:**
```c
class AuctionLot
{
    int LotNo;                          // Lot number
    string SellerSID;                   // Seller Steam64 ID
    string SellerName;                  // Seller name
    ref array<string> ItemClassNames;   // Items in auction
    int StartingBid;                    // Starting price
    int BuyoutPrice;                    // Buyout price (optional)
    int CurrentBid;                     // Current highest bid
    string CurrentBidderSID;            // Current bidder
    int EndTimeUTC;                     // Auction end time
    int StartTimeUTC;                   // Auction start time
}
```

**Send Auction Parcel:**
```c
void SendAuctionParcel(string winnerSID, AuctionLot lot, array<ItemBase> items)
{
    // Create auction mail
    MailEntry mail = new MailEntry();
    mail.SenderSID = "SYSTEM";
    mail.SenderName = "#STR_Auction_System";
    mail.RecipientSID = winnerSID;
    mail.Subject = "Auction Win: " + lot.ItemClassNames[0];
    mail.Message = "You won the auction for " + lot.ItemClassNames[0] + " with bid: " + lot.CurrentBid.ToString();
    mail.SystemOriginFile = "auction_" + lot.LotNo.ToString();

    // Attach items (in real implementation, move from auction storage)
    foreach (ItemBase item : items)
    {
        mail.ItemClassNames.Insert(item.GetType());
        GetGame().ObjectDelete(item);
    }

    // Add to winner inbox
    MailStorage.LoadInbox(winnerSID);

    if (!MailStorage.m_Inboxes.Contains(winnerSID))
    {
        MailStorage.m_Inboxes.Set(winnerSID, new array<ref MailEntry>);
    }

    MailStorage.m_Inboxes[winnerSID].Insert(mail);
    MailStorage.SaveInbox(winnerSID);

    // Notify winner if online
    PlayerBase winner = FindPlayerBySID(winnerSID);
    if (winner)
    {
        GetRPCManager().SendRPC("MailSystem", "ReceiveNotification",
            new Param2<string, string>("Auction", "You won an auction!"),
            true,
            winner.GetIdentity());
    }
}
```

---

## 🛡️ Security Considerations

### 1. Mail Validation

```c
bool ValidateMail(PlayerIdentity sender, string recipientSID, array<ItemBase> items)
{
    // Check recipient exists
    bool recipientExists = HasPlayerEverJoined(recipientSID);
    if (!recipientExists)
    {
        GetRPCManager().SendRPC("MailSystem", "MailError",
            new Param1<string>("Recipient not found"),
            true,
            sender);
        return false;
    }

    // Check item count limit
    if (items && items.Count() > 10)
    {
        GetRPCManager().SendRPC("MailSystem", "MailError",
            new Param1<string>("Too many items (max 10)"),
            true,
            sender);
        return false;
    }

    // Check forbidden items
    foreach (ItemBase item : items)
    {
        if (IsForbiddenItem(item.GetType()))
        {
            GetRPCManager().SendRPC("MailSystem", "MailError",
                new Param1<string>("Cannot send: " + item.GetDisplayName()),
                true,
                sender);
            return false;
        }
    }

    return true;
}
```

### 2. Rate Limiting

```c
class MailRateLimiter
{
    private map<string, int> m_LastSendTime;
    private int m_CooldownSeconds;

    bool CanSendMail(string playerSID)
    {
        int currentTime = GetGame().GetTickTime();

        if (m_LastSendTime.Contains(playerSID))
        {
            int lastSend = m_LastSendTime[playerSID];
            if (currentTime - lastSend < m_CooldownSeconds)
            {
                return false;  // Cooldown not expired
            }
        }

        m_LastSendTime.Set(playerSID, currentTime);
        return true;
    }
}
```

### 3. Mail Expiration

```c
void CleanExpiredMail()
{
    int currentTime = GetGame().GetTickTime();
    int expireDays = 30;  // Mail expires after 30 days
    int expireSeconds = expireDays * 86400;

    foreach (string playerID, array<ref MailEntry> inbox : MailStorage.m_Inboxes)
    {
        array<ref MailEntry> toRemove = new array<ref MailEntry>;

        foreach (MailEntry mail : inbox)
        {
            if (currentTime - mail.SentTime > expireSeconds)
            {
                toRemove.Insert(mail);
            }
        }

        // Remove expired mail
        foreach (MailEntry mail : toRemove)
        {
            inbox.RemoveItem(mail);
        }

        if (toRemove.Count() > 0)
        {
            MailStorage.SaveInbox(playerID);
        }
    }
}
```

---

## 📊 Performance Considerations

### Storage Optimization

**Problem:** Mail files grow large over time

**Solutions:**

1. **Limit Inbox Size:**
```c
const int MAX_INBOX_SIZE = 100;

void AddToInbox(string playerID, MailEntry mail)
{
    MailStorage.LoadInbox(playerID);
    array<ref MailEntry> inbox = MailStorage.m_Inboxes[playerID];

    // Remove oldest if full
    if (inbox.Count() >= MAX_INBOX_SIZE)
    {
        inbox.RemoveOrdered(0);  // Remove oldest
    }

    inbox.Insert(mail);
}
```

2. **Archive Old Mail:**
```c
void ArchiveOldMail(string playerID)
{
    const int ARCHIVE_DAYS = 30;
    int cutoffTime = GetGame().GetTickTime() - (ARCHIVE_DAYS * 86400);

    array<ref MailEntry> inbox = MailStorage.m_Inboxes[playerID];
    array<ref MailEntry> archive = new array<ref MailEntry>;

    for (int i = inbox.Count() - 1; i >= 0; i--)
    {
        if (inbox[i].SentTime < cutoffTime)
        {
            archive.Insert(inbox[i]);
            inbox.Remove(i);
        }
    }

    // Save archive separately
    if (archive.Count() > 0)
    {
        string archivePath = "$profile:Mail/Archive/" + playerID + ".json";
        JsonFileLoader<array<ref MailEntry>>::JsonSaveFile(archivePath, archive);
    }

    MailStorage.SaveInbox(playerID);
}
```

---

## 🔗 Integration Examples

### Example 1: Quest Rewards

```c
// Quest completion sends reward via mail
void CompleteQuest(PlayerBase player, QuestReward reward)
{
    string playerID = player.GetIdentity().GetPlainId();

    // Create reward mail
    MailEntry mail = new MailEntry();
    mail.SenderSID = "QUEST_SYSTEM";
    mail.SenderName = "Quest Master";
    mail.RecipientSID = playerID;
    mail.Subject = "Quest Complete: " + reward.QuestName;
    mail.Message = "Congratulations! You completed: " + reward.QuestDescription;
    mail.SystemOriginFile = "quest_" + reward.QuestID.ToString();

    // Add reward items
    foreach (string itemClass : reward.ItemRewards)
    {
        mail.ItemClassNames.Insert(itemClass);
    }

    // Add currency
    if (reward.CurrencyReward > 0)
    {
        // Send currency via banking system
        BankingManager.Deposit(playerID, reward.CurrencyReward);
    }

    // Send mail
    MailStorage.LoadInbox(playerID);
    MailStorage.m_Inboxes[playerID].Insert(mail);
    MailStorage.SaveInbox(playerID);

    // Notify player
    NotificationSystem.Notify(player, "Quest reward sent to mailbox!");
}
```

### Example 2: Auction Winnings

```c
// Auction ends, send item to winner
void AuctionEnd(AuctionLot lot, string winnerSID)
{
    // Create auction parcel mail
    MailEntry mail = new MailEntry();
    mail.SenderSID = "AUCTION_SYSTEM";
    mail.SenderName = "Auction House";
    mail.RecipientSID = winnerSID;
    mail.Subject = "Auction Win: Lot #" + lot.LotNo.ToString();
    mail.Message = "You won with bid: " + lot.CurrentBid.ToString();
    mail.SystemOriginFile = "auction_" + lot.LotNo.ToString();

    // Attach auction items
    foreach (string itemClass : lot.ItemClassNames)
    {
        mail.ItemClassNames.Insert(itemClass);
    }

    // Send mail
    SendAuctionParcel(winnerSID, lot, mail);
}
```

### Example 3: Player Trading

```c
// P2P trading via mail
void TradeItems(PlayerIdentity sender, string recipientSID, array<ItemBase> items)
{
    if (!ValidateTrade(sender, recipientSID, items))
        return;

    // Create trade mail
    MailEntry mail = new MailEntry();
    mail.SenderSID = sender.GetPlainId();
    mail.SenderName = sender.GetName();
    mail.RecipientSID = recipientSID;
    mail.Subject = "Trade from " + mail.SenderName;
    mail.Message = "Items attached from trade";

    // Attach items
    foreach (ItemBase item : items)
    {
        mail.ItemClassNames.Insert(item.GetType());
        GetGame().ObjectDelete(item);
    }

    // Send
    MailStorage.LoadInbox(recipientSID);
    MailStorage.m_Inboxes[recipientSID].Insert(mail);
    MailStorage.SaveInbox(recipientSID);

    // Notify recipient
    PlayerBase recipient = FindPlayerBySID(recipientSID);
    if (recipient)
    {
        GetRPCManager().SendRPC("MailSystem", "ReceiveNotification",
            new Param2<string, string>(mail.SenderName, "Sent you items via mail"),
            true,
            recipient.GetIdentity());
    }
}
```

---

## 🐛 Common Issues & Troubleshooting

### Issue 1: Mail Not Received

**Symptoms:** Sender sent mail but recipient doesn't see it.

**Causes:**
1. JSON file not saved
2. Wrong recipient SID
3. File permission issues

**Solution:**
```c
// Verify save
bool SaveInbox(string playerID)
{
    string mailDir = "$profile:Mail/";

    // Create directory if needed
    if (!FileExist(mailDir))
    {
        if (!MakeDirectory(mailDir))
        {
            Print("[Mail] ERROR: Failed to create mail directory!");
            return false;
        }
    }

    string filePath = mailDir + playerID + "_inbox.json";

    // Save
    JsonFileLoader<array<ref MailEntry>>::JsonSaveFile(filePath, m_Inboxes[playerID]);

    // Verify file exists
    if (!FileExist(filePath))
    {
        Print("[Mail] ERROR: Failed to save inbox for " + playerID);
        return false;
    }

    return true;
}
```

---

### Issue 2: Items Lost on Receive

**Symptoms:** Recipient opens mail but items don't appear.

**Causes:**
1. Inventory full
2. Invalid item class name
3. Item deleted before spawn

**Solution:**
```c
void ReceiveItems(MailEntry mail)
{
    PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());

    foreach (string itemClass : mail.ItemClassNames)
    {
        // Try to spawn in inventory
        ItemBase item = player.GetHumanInventory().CreateInInventory(itemClass);

        if (!item)
        {
            // Inventory full, spawn on ground nearby
            vector playerPos = player.GetPosition();
            vector spawnPos = playerPos + player.GetDirection() * 1.0;  // 1 meter in front

            item = ItemBase.Cast(GetGame().CreateObject(itemClass, spawnPos));

            if (!item)
            {
                Print("[Mail] ERROR: Failed to spawn " + itemClass + " for mail " + mail.Id);
                continue;
            }
        }

        Print("[Mail] Spawned " + itemClass + " from mail " + mail.Id);
    }
}
```

---

### Issue 3: Mailbox Not Opening

**Symptoms:** Player interacts with mailbox but nothing happens.

**Causes:**
1. Action not registered
2. Wrong action condition
3. Menu not initialized

**Solution:**
```c
class ActionOpenMailbox: ActionInteractBase
{
    override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
    {
        // Check target is mailbox
        MailBox mailbox = MailBox.Cast(target.GetObject());
        if (!mailbox)
            return false;

        // Check range
        float distance = vector.Distance(player.GetPosition(), mailbox.GetPosition());
        if (distance > 3.0)  // 3 meters
            return false;

        return true;
    }

    override void OnStartClient(ActionData action_data)
    {
        super.OnStartClient(action_data);

        // Open mailbox menu
        OpenMailboxMenu();
    }
}
```

---

## 📝 Quick Reference

### Send Mail

```c
GetRPCManager().SendRPC("MailSystem", "SendMail",
    new Param4<string, string, string, array<ItemBase>>(recipientSID, subject, message, items),
    true,
    senderIdentity);
```

### Receive Mail

```c
void ReceiveMail(string mailID)
{
    // Load inbox
    MailStorage.LoadInbox(playerID);

    // Find mail
    MailEntry mail = FindMail(mailID);

    // Spawn items
    SpawnMailItems(mail);

    // Mark read
    mail.Read = true;
    MailStorage.SaveInbox(playerID);
}
```

### List Inbox

```c
GetRPCManager().SendRPC("MailSystem", "RequestInbox",
    NULL,
    true,
    playerIdentity);
```

---

**Last Updated:** 2025-01-18
**Status:** ✅ Complete
**Next:** See [Mail Topic Guide](../../topics/32_mail_systems.md) for beginner tutorial
