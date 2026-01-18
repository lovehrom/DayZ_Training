# Auction/P2P Trading Systems - Beginner's Guide

**Status:** ✅ Complete Guide
**Complexity:** Advanced
**Time to implement:** 6-8 hours

---

## ⚡ 30-Second Summary

Player-to-player auction system allows players to list items for auction, place bids, buyout instantly, and receive items via mail delivery. Integrates with banking system for secure currency transactions.

---

## 🎯 What Are Auction Systems?

### Concept

Auction systems allow players to trade with each other without needing to be online at the same time. Players can list items, set starting bids, and let other players bid over time.

**Key features:**
- List items for auction
- Set starting bid and buyout price
- Place bids on items
- Instant buyout option
- Mail delivery system
- Banking integration
- Anti-scam protection

---

## 📁 File Structure

```
auction_system/
├── config.cpp                    ← Auction terminal definitions
├── auctions.json                 ← Auction data
└── scripts/
    ├── 3_Game/
    │   ├── AuctionManager.c      ← Core auction tracking
    │   ├── AuctionListing.c      ← Individual auction
    │   ├── BiddingSystem.c       ← Bid management
    │   ├── BankManager.c         ← Currency storage
    │   └── MailDelivery.c        ← Item delivery
    ├── 4_World/
    │   ├── AuctionTerminal.c     ← Auction terminal
    │   └── Mailbox.c             ← Mailbox entity
    └── 5_Mission/
        └── AuctionBrowserUI.c    ← Marketplace UI
```

---

## 💡 Key Concepts

### 1. Auction Lifecycle

```
Player lists item
    ↓
Auction created
    ↓
Other players browse
    ↓
Players place bids OR buyout
    ↓
Auction ends OR bought out
    ↓
Items sent via mail
    ↓
Currency transferred
```

---

### 2. Auction Listing

**Basic Structure:**

```json
{
    "ID": "auction_12345",
    "SellerID": "76561198012345678",
    "SellerName": "PlayerName",
    "Items": [...],
    "StartingBid": 1000,
    "CurrentBid": 1500,
    "BuyoutPrice": 5000,
    "EndTime": 1705693200,
    "Status": "active"
}
```

---

### 3. Bidding System

**Flow:**
1. Player views auction
2. Player enters bid amount
3. System validates bid (must be higher than current)
4. System checks player has currency
5. System deducts bid amount
6. If outbid, previous bidder refunded

---

### 4. Mail Delivery

**Flow:**
1. Auction completes
2. Items packaged as mail parcel
3. Parcel sent to winner's mailbox
4. Player notified
5. Player collects from mailbox

---

## 🔧 Implementation

### Step 1: Create AuctionManager.c

```c
class AuctionManager
{
    protected ref map<string, ref AuctionListing> m_Auctions;
    protected static ref AuctionManager m_Instance;

    static AuctionManager GetInstance()
    {
        if (!m_Instance)
        {
            m_Instance = new AuctionManager();
        }
        return m_Instance;
    }

    void AuctionManager()
    {
        m_Auctions = new map<string, ref AuctionListing>;
        LoadAuctions();
    }

    void LoadAuctions()
    {
        // Load from JSON
    }

    string CreateAuction(PlayerBase player, array<EntityAI> items, int startingBid, int buyoutPrice, float duration)
    {
        AuctionListing auction = new AuctionListing();
        auction.m_SellerID = player.GetIdentity().GetId();
        auction.m_SellerName = player.GetIdentity().GetName();
        auction.m_StartingBid = startingBid;
        auction.m_BuyoutPrice = buyoutPrice;
        auction.m_CurrentBid = startingBid;
        auction.m_EndTime = GetGame().GetTickTime() + duration;
        auction.m_Status = "active";

        // Add items to auction
        foreach (EntityAI item : items)
        {
            auction.AddItem(item);
            item.Delete(); // Remove from player inventory
        }

        m_Auctions.Set(auction.GetID(), auction);

        NotificationSystem.Notify(player, "Auction created: " + auction.GetID());

        return auction.GetID();
    }

    AuctionListing GetAuction(string auctionID)
    {
        if (m_Auctions.Contains(auctionID))
            return m_Auctions.Get(auctionID);

        return null;
    }

    array<ref AuctionListing> GetActiveAuctions()
    {
        array<ref AuctionListing> active = new array<ref AuctionListing>;

        foreach (string id, AuctionListing auction : m_Auctions)
        {
            if (auction.IsActive() && !auction.IsExpired())
                active.Insert(auction);
        }

        return active;
    }
}
```

---

### Step 2: Create AuctionListing.c

```c
class AuctionListing
{
    protected string m_ID;
    protected string m_SellerID;
    protected string m_SellerName;
    protected ref array<ref AuctionItem> m_Items;
    protected int m_StartingBid;
    protected int m_CurrentBid;
    protected int m_BuyoutPrice;
    protected float m_EndTime;
    protected string m_Status;

    protected ref array<ref AuctionBid> m_Bids;

    void AuctionListing()
    {
        m_ID = "auction_" + GetGame().GetTickTime().ToString();
        m_Items = new array<ref AuctionItem>;
        m_Bids = new array<ref AuctionBid>;
        m_Status = "active";
    }

    string GetID()
    {
        return m_ID;
    }

    string GetSellerName()
    {
        return m_SellerName;
    }

    int GetCurrentBid()
    {
        return m_CurrentBid;
    }

    int GetBuyoutPrice()
    {
        return m_BuyoutPrice;
    }

    float GetEndTime()
    {
        return m_EndTime;
    }

    bool IsActive()
    {
        return m_Status == "active";
    }

    bool IsExpired()
    {
        float currentTime = GetGame().GetTickTime();
        return currentTime >= m_EndTime;
    }

    void AddItem(EntityAI item)
    {
        AuctionItem auctionItem = new AuctionItem(item);
        m_Items.Insert(auctionItem);
    }

    void PlaceBid(AuctionBid bid)
    {
        if (bid.Amount <= m_CurrentBid)
        {
            Print("[Auction] Bid too low!");
            return;
        }

        // Refund previous bidder
        if (m_Bids.Count() > 0)
        {
            AuctionBid previousBid = m_Bids[m_Bids.Count() - 1];
            BankManager.GetInstance().Deposit(previousBid.BidderID, previousBid.Amount);
        }

        m_Bids.Insert(bid);
        m_CurrentBid = bid.Amount;

        Print("[Auction] New bid: " + bid.Amount + " by " + bid.BidderName);
    }

    void Buyout(string buyerID)
    {
        if (BankManager.GetInstance().HasBalance(buyerID, m_BuyoutPrice))
        {
            // Transfer currency
            BankManager.GetInstance().Withdraw(buyerID, m_BuyoutPrice);
            BankManager.GetInstance().Deposit(m_SellerID, m_BuyoutPrice);

            // Send items to buyer
            MailDelivery.GetInstance().SendItems(buyerID, m_Items, "Auction bought out!");

            // Refund bidders
            foreach (AuctionBid bid : m_Bids)
            {
                BankManager.GetInstance().Deposit(bid.BidderID, bid.Amount);
            }

            m_Status = "completed";
        }
    }

    void CompleteAuction()
    {
        if (m_Bids.Count() == 0)
        {
            // Return items to seller
            MailDelivery.GetInstance().SendItems(m_SellerID, m_Items, "Auction expired - items returned");
        }
        else
        {
            // Highest bid wins
            AuctionBid winningBid = m_Bids[m_Bids.Count() - 1];

            // Send items to winner
            MailDelivery.GetInstance().SendItems(winningBid.BidderID, m_Items, "You won the auction!");

            // Send currency to seller
            BankManager.GetInstance().Deposit(m_SellerID, winningBid.Amount);
        }

        m_Status = "completed";
    }
}

class AuctionItem
{
    string ClassName;
    float Health;
    ref array<string> Attachments;

    void AuctionItem(EntityAI item)
    {
        ClassName = item.GetType();
        Health = item.GetHealth01();

        Attachments = new array<string>;
        array<EntityAI> attachments = new array<EntityAI>;
        item.GetInventory().EnumerateInventory(InventoryMode.LOCAL, attachments);

        foreach (EntityAI att : attachments)
        {
            Attachments.Insert(att.GetType());
        }
    }
}

class AuctionBid
{
    string BidderID;
    string BidderName;
    int Amount;

    void AuctionBid(string bidderID, string bidderName, int amount)
    {
        BidderID = bidderID;
        BidderName = bidderName;
        Amount = amount;
    }
}
```

---

### Step 3: Create BankManager.c

```c
class BankManager
{
    protected ref map<string, int> m_PlayerBalances;
    protected static ref BankManager m_Instance;

    static BankManager GetInstance()
    {
        if (!m_Instance)
        {
            m_Instance = new BankManager();
        }
        return m_Instance;
    }

    void BankManager()
    {
        m_PlayerBalances = new map<string, int>;
        LoadBalances();
    }

    void LoadBalances()
    {
        string filePath = "$profile:Banking/balances.json";

        if (!FileExist(filePath))
            return;

        JsonFileLoader<map<string, int>>::LoadJsonFile(filePath, m_PlayerBalances);
    }

    void SaveBalances()
    {
        string filePath = "$profile:Banking/balances.json";

        JsonSaveManager json = new JsonSaveManager();
        json.SaveToFile(filePath, m_PlayerBalances);
    }

    int GetBalance(string playerID)
    {
        if (!m_PlayerBalances.Contains(playerID))
            m_PlayerBalances.Set(playerID, 0);

        return m_PlayerBalances.Get(playerID);
    }

    bool HasBalance(string playerID, int amount)
    {
        return GetBalance(playerID) >= amount;
    }

    void Deposit(string playerID, int amount)
    {
        int current = GetBalance(playerID);
        m_PlayerBalances.Set(playerID, current + amount);
        SaveBalances();
    }

    void Withdraw(string playerID, int amount)
    {
        int current = GetBalance(playerID);
        m_PlayerBalances.Set(playerID, current - amount);
        SaveBalances();
    }
}
```

---

### Step 4: Create MailDelivery.c

```c
class MailDelivery
{
    protected ref map<string, ref array<ref MailParcel>> m_PlayerMailboxes;
    protected static ref MailDelivery m_Instance;

    static MailDelivery GetInstance()
    {
        if (!m_Instance)
        {
            m_Instance = new MailDelivery();
        }
        return m_Instance;
    }

    void MailDelivery()
    {
        m_PlayerMailboxes = new map<string, ref array<ref MailParcel>>;
    }

    void SendItems(string recipientID, array<ref AuctionItem> items, string message)
    {
        MailParcel parcel = new MailParcel();
        parcel.Items = items;
        parcel.Message = message;

        if (!m_PlayerMailboxes.Contains(recipientID))
        {
            m_PlayerMailboxes.Set(recipientID, new array<ref MailParcel>);
        }

        m_PlayerMailboxes.Get(recipientID).Insert(parcel);

        // Notify recipient
        NotificationSystem.NotifyByID(recipientID, "You have received mail!");
    }

    array<ref MailParcel> GetMailbox(string playerID)
    {
        if (!m_PlayerMailboxes.Contains(playerID))
        {
            m_PlayerMailboxes.Set(playerID, new array<ref MailParcel>);
        }

        return m_PlayerMailboxes.Get(playerID);
    }
}

class MailParcel
{
    array<ref AuctionItem> Items;
    string Message;
}
```

---

### Step 5: Create AuctionTerminal.c

```c
class AuctionTerminal extends ItemBase
{
    override void SetActions()
    {
        super.SetActions();
        AddAction(ActionOpenAuctionMenu);
    }
}
```

---

### Step 6: PlayerBase Integration

```c
modded class PlayerBase
{
    override void OnConnect()
    {
        super.OnConnect();

        // Initialize systems
        AuctionManager.GetInstance();
        BankManager.GetInstance();
        MailDelivery.GetInstance();
    }

    bool ChatCommand_BrowseAuctions()
    {
        array<ref AuctionListing> auctions = AuctionManager.GetInstance().GetActiveAuctions();

        string message = "Active Auctions:";

        foreach (AuctionListing auction : auctions)
        {
            message += "\\n" + auction.GetSellerName() + " - Current bid: " + auction.GetCurrentBid();
        }

        NotificationSystem.Notify(this, message);

        return true;
    }

    bool ChatCommand_CreateAuction(array<string> args)
    {
        // Simplified example
        // In production, would use UI to select items, prices, etc.

        EntityAI item = GetItemInHands();

        if (!item)
        {
            NotificationSystem.Notify(this, "Hold item to auction!");
            return false;
        }

        array<EntityAI> items = new array<EntityAI>;
        items.Insert(item);

        string auctionID = AuctionManager.GetInstance().CreateAuction(this, items, 1000, 5000, 86400);

        NotificationSystem.Notify(this, "Auction created: " + auctionID);

        return true;
    }
}
```

---

## 📝 Usage Examples

### Example 1: Create Auction

```
/create_auction
```

---

### Example 2: Browse Auctions

```
/browse_auctions
```

---

### Example 3: Place Bid

```
/bid <auction_id> <amount>
```

---

### Example 4: Buyout Auction

```
/buyout <auction_id>
```

---

## ⚠️ Important Notes

### Security

**Always validate:**
- Item ownership
- Sufficient funds
- Bid amounts
- Time limits

### Anti-Scam

**Prevent:**
- Self-bidding
- Invalid items
- Duplicate bids
- Exploit attempts

---

## 🔗 Related Topics

→ [Trader Systems](topics/27_trader_systems.md) - NPC trading
→ [Quest Systems](topics/28_quest_systems.md) - Quest rewards
→ [XP Systems](topics/12_xp_systems.md) - XP rewards

---

## 🐛 Troubleshooting

### Auction not appearing

**Check:**
1. Auction status is "active"
2. Auction not expired
3. Items valid

### Bids not working

**Check:**
1. Player has sufficient funds
2. Bid amount higher than current
3. Bidder not seller

---

*Last updated: 2025-01-18*
