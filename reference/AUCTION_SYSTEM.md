# Auction/P2P Trading System - Complete Reference

**Status:** ✅ Complete Reference
**Target:** Advanced modders
**Goal:** Comprehensive player-to-player auction and marketplace system

---

## ⚡ 30-Second Summary

Complete P2P trading system with auction listings, bidding system, buyout prices, mail delivery, banking integration, item validation, time limits, and comprehensive security measures to prevent scams.

---

## 📋 Table of Contents

1. [System Architecture](#system-architecture)
2. [Auction Listings](#auction-listings)
3. [Bidding System](#bidding-system)
4. [Buyout System](#buyout-system)
5. [Mail Delivery](#mail-delivery)
6. [Banking Integration](#banking-integration)
7. [Marketplace UI](#marketplace-ui)
8. [Validation & Security](#validation--security)
9. [Time Management](#time-management)
10. [Network Synchronization](#network-synchronization)
11. [Complete Implementation](#complete-implementation)
12. [Integration Patterns](#integration-patterns)
13. [Anti-Scam Measures](#anti-scam-measures)
14. [Troubleshooting](#troubleshooting)

---

## System Architecture

### System Components

```
Auction/P2P System
├── AuctionManager           ← Core auction tracking
├── AuctionListing           ← Individual auction
├── BiddingSystem           ← Bid management
├── BuyoutSystem            ← Instant purchase
├── MailDelivery            ← Item delivery
├── BankingSystem           ← Currency storage
├── AuctionUI               ← User interface
└── SecurityValidator       ← Anti-scam
```

### Data Flow

```
Player creates auction
    ↓
Listing added to marketplace
    ↓
Other players browse listings
    ↓
Player places bid or buys out
    ↓
Bid accepted/buyout triggered
    ↓
Items exchanged via mail
    ↓
Currency transferred
    ↓
Auction completed
```

---

## Auction Listings

### Listing Structure

```json
{
    "ID": "auction_12345",
    "SellerID": "76561198012345678",
    "SellerName": "PlayerName",
    "Items": [...],
    "StartingBid": 1000,
    "CurrentBid": 1500,
    "BuyoutPrice": 5000,
    "BidCount": 3,
    "StartTime": 1705606800,
    "EndTime": 1705693200,
    "Status": "active"
}
```

### AuctionListing Class

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
    protected int m_BidCount;
    protected float m_StartTime;
    protected float m_EndTime;
    protected string m_Status; // "active", "completed", "expired"

    protected ref array<ref AuctionBid> m_Bids;

    void AuctionListing()
    {
        m_ID = GenerateUniqueID();
        m_Items = new array<ref AuctionItem>;
        m_Bids = new array<ref AuctionBid>;
        m_Status = "active";
        m_StartTime = GetGame().GetTickTime();
        m_BidCount = 0;
    }

    string GenerateUniqueID()
    {
        return "auction_" + GetGame().GetTickTime().ToString();
    }

    string GetID()
    {
        return m_ID;
    }

    string GetSellerID()
    {
        return m_SellerID;
    }

    array<ref AuctionItem> GetItems()
    {
        return m_Items;
    }

    int GetStartingBid()
    {
        return m_StartingBid;
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
        // Validate bid amount
        if (bid.Amount <= m_CurrentBid)
        {
            Print("[Auction] Bid too low!");
            return;
        }

        // Refund previous highest bidder
        if (m_Bids.Count() > 0)
        {
            AuctionBid previousBid = m_Bids[m_Bids.Count() - 1];
            RefundBid(previousBid);
        }

        m_Bids.Insert(bid);
        m_CurrentBid = bid.Amount;
        m_BidCount++;

        // Notify seller
        NotifySeller(bid);

        // Notify outbid player
        NotifyOutbid(bid);
    }

    void RefundBid(AuctionBid bid)
    {
        // Return currency to bidder
        if (BankManager)
        {
            BankManager.GetInstance().Deposit(bid.BidderID, bid.Amount);
        }
    }

    void NotifySeller(AuctionBid bid)
    {
        // Notify seller of new bid
        PlayerBase seller = GetPlayerByID(m_SellerID);
        if (seller)
        {
            NotificationSystem.Notify(seller, "New bid: " + bid.Amount + " by " + bid.BidderName);
        }
    }

    void NotifyOutbid(AuctionBid bid)
    {
        // Notify previous highest bidder
        if (m_Bids.Count() > 1)
        {
            AuctionBid previousBid = m_Bids[m_Bids.Count() - 2];
            PlayerBase player = GetPlayerByID(previousBid.BidderID);
            if (player)
            {
                NotificationSystem.Notify(player, "You have been outbid!");
            }
        }
    }

    void CompleteAuction()
    {
        if (m_Bids.Count() == 0)
        {
            // No bids - return items to seller
            ReturnItemsToSeller();
        }
        else
        {
            // Highest bid wins
            AuctionBid winningBid = m_Bids[m_Bids.Count() - 1];

            // Transfer items to winner
            SendItemsToWinner(winningBid);

            // Transfer currency to seller
            SendCurrencyToSeller(winningBid);
        }

        m_Status = "completed";
    }

    void ReturnItemsToSeller()
    {
        // Send items back to seller via mail
        if (MailDelivery)
        {
            MailDelivery.GetInstance().SendItems(m_SellerID, m_Items, "Auction expired - items returned");
        }
    }

    void SendItemsToWinner(AuctionBid bid)
    {
        // Send items to winner via mail
        if (MailDelivery)
        {
            MailDelivery.GetInstance().SendItems(bid.BidderID, m_Items, "You won the auction!");
        }
    }

    void SendCurrencyToSeller(AuctionBid bid)
    {
        // Deposit currency to seller's bank
        if (BankManager)
        {
            BankManager.GetInstance().Deposit(m_SellerID, bid.Amount);
        }
    }

    void Buyout(string buyerID, string buyerName)
    {
        // Validate buyer has enough currency
        if (BankManager)
        {
            if (!BankManager.GetInstance().HasBalance(buyerID, m_BuyoutPrice))
            {
                Print("[Auction] Insufficient funds for buyout!");
                return;
            }

            // Deduct currency
            BankManager.GetInstance().Withdraw(buyerID, m_BuyoutPrice);

            // Send currency to seller
            BankManager.GetInstance().Deposit(m_SellerID, m_BuyoutPrice);

            // Send items to buyer
            if (MailDelivery)
            {
                MailDelivery.GetInstance().SendItems(buyerID, m_Items, "Auction bought out!");
            }

            // Refund all bidders
            foreach (AuctionBid bid : m_Bids)
            {
                RefundBid(bid);
            }
        }

        m_Status = "completed";
    }
}

class AuctionItem
{
    string ClassName;
    float Health;
    int Quantity;
    ref array<string> Attachments;

    void AuctionItem(EntityAI item)
    {
        ClassName = item.GetType();
        Health = item.GetHealth01();
        Quantity = 1;

        // Get attachments
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
    float BidTime;

    void AuctionBid(string bidderID, string bidderName, int amount)
    {
        BidderID = bidderID;
        BidderName = bidderName;
        Amount = amount;
        BidTime = GetGame().GetTickTime();
    }
}
```

---

## Bidding System

### Bidding Logic

```c
class BiddingSystem
{
    protected ref AuctionManager m_AuctionManager;

    bool PlaceBid(PlayerBase player, string auctionID, int amount)
    {
        AuctionListing auction = m_AuctionManager.GetAuction(auctionID);

        if (!auction)
        {
            NotificationSystem.Notify(player, "Auction not found!");
            return false;
        }

        if (!auction.IsActive())
        {
            NotificationSystem.Notify(player, "Auction is not active!");
            return false;
        }

        if (auction.IsExpired())
        {
            NotificationSystem.Notify(player, "Auction has expired!");
            return false;
        }

        // Check if bidder is seller
        if (auction.GetSellerID() == player.GetIdentity().GetId())
        {
            NotificationSystem.Notify(player, "You cannot bid on your own auction!");
            return false;
        }

        // Validate bid amount
        if (amount <= auction.GetCurrentBid())
        {
            NotificationSystem.Notify(player, "Bid must be higher than current bid!");
            return false;
        }

        // Check if player has enough currency
        if (BankManager)
        {
            if (!BankManager.GetInstance().HasBalance(player.GetIdentity().GetId(), amount))
            {
                NotificationSystem.Notify(player, "Insufficient funds!");
                return false;
            }

            // Deduct bid amount (refunded if outbid)
            BankManager.GetInstance().Withdraw(player.GetIdentity().GetId(), amount);
        }

        // Place bid
        AuctionBid bid = new AuctionBid(
            player.GetIdentity().GetId(),
            player.GetIdentity().GetName(),
            amount
        );

        auction.PlaceBid(bid);

        NotificationSystem.Notify(player, "Bid placed: " + amount);

        return true;
    }
}
```

---

## Buyout System

### Instant Purchase

```c
class BuyoutSystem
{
    protected ref AuctionManager m_AuctionManager;

    bool BuyoutAuction(PlayerBase player, string auctionID)
    {
        AuctionListing auction = m_AuctionManager.GetAuction(auctionID);

        if (!auction)
        {
            NotificationSystem.Notify(player, "Auction not found!");
            return false;
        }

        if (!auction.IsActive())
        {
            NotificationSystem.Notify(player, "Auction is not active!");
            return false;
        }

        if (auction.GetBuyoutPrice() <= 0)
        {
            NotificationSystem.Notify(player, "This auction has no buyout price!");
            return false;
        }

        // Perform buyout
        auction.Buyout(
            player.GetIdentity().GetId(),
            player.GetIdentity().GetName()
        );

        NotificationSystem.Notify(player, "Auction bought out for " + auction.GetBuyoutPrice());

        return true;
    }
}
```

---

## Mail Delivery

### Mail System

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
        parcel.SentTime = GetGame().GetTickTime();

        if (!m_PlayerMailbox.Contains(recipientID))
        {
            m_PlayerMailbox.Set(recipientID, new array<ref MailParcel>);
        }

        m_PlayerMailbox.Get(recipientID).Insert(parcel);

        // Notify recipient
        PlayerBase recipient = GetPlayerByID(recipientID);
        if (recipient)
        {
            NotificationSystem.Notify(recipient, "You have received mail!");
        }
    }

    array<ref MailParcel> GetMailbox(string playerID)
    {
        if (!m_PlayerMailbox.Contains(playerID))
        {
            m_PlayerMailbox.Set(playerID, new array<ref MailParcel>);
        }

        return m_PlayerMailbox.Get(playerID);
    }

    void CollectItems(PlayerBase player, int parcelIndex)
    {
        string playerID = player.GetIdentity().GetId();
        array<ref MailParcel> mailbox = GetMailbox(playerID);

        if (parcelIndex < 0 || parcelIndex >= mailbox.Count())
            return;

        MailParcel parcel = mailbox[parcelIndex];

        // Spawn items
        foreach (AuctionItem item : parcel.Items)
        {
            EntityAI entity = GetGame().CreateObject(item.ClassName, player.GetPosition());

            if (!entity)
                continue;

            // Set health
            if (entity.GetHealth01() != item.Health)
            {
                entity.SetHealth01("", "", item.Health);
            }

            // Add attachments
            foreach (string att : item.Attachments)
            {
                EntityAI attachment = GetGame().CreateObject(att, player.GetPosition());
                if (attachment)
                {
                    entity.GetInventory().AddInInventory(attachment);
                }
            }

            // Add to player inventory
            if (!player.GetInventory().AddInInventory(entity))
            {
                entity.Delete();
                NotificationSystem.Notify(player, "Inventory full! Item dropped.");
            }
        }

        // Remove parcel
        mailbox.Remove(parcelIndex);

        NotificationSystem.Notify(player, "Mail collected!");
    }
}

class MailParcel
{
    array<ref AuctionItem> Items;
    string Message;
    float SentTime;
    bool Collected;
}
```

---

## Banking Integration

### Banking System

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
        int currentBalance = GetBalance(playerID);
        m_PlayerBalances.Set(playerID, currentBalance + amount);

        Print("[Bank] Deposited " + amount + " to " + playerID + ". New balance: " + (currentBalance + amount));

        SaveBalances();
    }

    void Withdraw(string playerID, int amount)
    {
        int currentBalance = GetBalance(playerID);

        if (currentBalance < amount)
        {
            Print("[Bank] Insufficient funds for withdrawal!");
            return;
        }

        m_PlayerBalances.Set(playerID, currentBalance - amount);

        Print("[Bank] Withdrew " + amount + " from " + playerID + ". New balance: " + (currentBalance - amount));

        SaveBalances();
    }

    void Transfer(string fromPlayerID, string toPlayerID, int amount)
    {
        if (!HasBalance(fromPlayerID, amount))
        {
            Print("[Bank] Insufficient funds for transfer!");
            return;
        }

        Withdraw(fromPlayerID, amount);
        Deposit(toPlayerID, amount);
    }
}
```

---

## Marketplace UI

### Auction Browser

```c
class AuctionBrowserUI
{
    protected ref array<ref AuctionListing> m_Auctions;
    protected Widget m_RootWidget;

    void Show()
    {
        if (!m_RootWidget)
        {
            CreateLayout();
        }

        m_RootWidget.Show(true);
        RefreshAuctionList();
    }

    void CreateLayout()
    {
        m_RootWidget = GetGame().GetWorkspace().CreateWidgets("DayZ_Gui/Layouts/AuctionBrowser.layout");
    }

    void RefreshAuctionList()
    {
        m_Auctions = AuctionManager.GetInstance().GetActiveAuctions();

        // Update UI list
        GridWidget grid = m_RootWidget.FindAnyWidget("AuctionGrid");

        if (grid)
        {
            grid.ClearItems();

            foreach (AuctionListing auction : m_Auctions)
            {
                AddAuctionToGrid(auction, grid);
            }
        }
    }

    void AddAuctionToGrid(AuctionListing auction, GridWidget grid)
    {
        // Add auction to UI grid
        Widget itemWidget = grid.AddWidget("AuctionItem");

        TextWidget sellerWidget = itemWidget.FindAnyWidget("SellerName");
        TextWidget bidWidget = itemWidget.FindAnyWidget("CurrentBid");
        TextWidget timeWidget = itemWidget.FindAnyWidget("TimeRemaining");

        if (sellerWidget)
            sellerWidget.SetText(auction.GetSellerName());

        if (bidWidget)
            bidWidget.SetText(auction.GetCurrentBid().ToString());

        if (timeWidget)
        {
            float timeRemaining = auction.GetEndTime() - GetGame().GetTickTime();
            timeWidget.SetText FormatTime(timeRemaining);
        }
    }

    string FormatTime(float seconds)
    {
        int hours = seconds / 3600;
        int minutes = (seconds % 3600) / 60;

        return hours.ToString() + "h " + minutes.ToString() + "m";
    }
}
```

---

## Validation & Security

### Security Validator

```c
class AuctionValidator
{
    static bool ValidateItem(EntityAI item)
    {
        // Check if item is valid for auction
        if (!item)
            return false;

        // Check if item is allowed
        if (!IsItemAllowed(item.GetType()))
            return false;

        return true;
    }

    static bool IsItemAllowed(string className)
    {
        // Blacklist certain items
        array<string> blacklist = {
            "ExpansionBankNote", // Not allowed
            "VaultBox" // Not allowed
        };

        foreach (string forbidden : blacklist)
        {
            if (className == forbidden)
                return false;
        }

        return true;
    }

    static bool ValidateBid(PlayerBase player, int amount)
    {
        // Check if player has sufficient funds
        if (BankManager)
        {
            if (!BankManager.GetInstance().HasBalance(player.GetIdentity().GetId(), amount))
                return false;
        }

        return true;
    }

    static bool ValidateAuction(AuctionListing auction)
    {
        // Validate auction data
        if (!auction || !auction.GetItems())
            return false;

        if (auction.GetItems().Count() == 0)
            return false;

        if (auction.GetStartingBid() < 0)
            return false;

        return true;
    }
}
```

---

## Time Management

### Auction Expiration

```c
class AuctionExpirationManager
{
    protected float m_CheckInterval = 60.0; // Check every 60 seconds
    protected float m_LastCheck;

    void Update(float deltaTime)
    {
        float currentTime = GetGame().GetTickTime();

        if (currentTime - m_LastCheck < m_CheckInterval)
            return;

        m_LastCheck = currentTime;

        CheckExpiredAuctions();
    }

    void CheckExpiredAuctions()
    {
        array<ref AuctionListing> auctions = AuctionManager.GetInstance().GetActiveAuctions();

        foreach (AuctionListing auction : auctions)
        {
            if (auction.IsExpired())
            {
                Print("[Auction] Auction " + auction.GetID() + " has expired!");
                auction.CompleteAuction();
            }
        }
    }
}
```

---

## Network Synchronization

### RPC Messages

```c
class AuctionRPC
{
    static void SendAuctionList(PlayerBase player)
    {
        array<ref AuctionListing> auctions = AuctionManager.GetInstance().GetActiveAuctions();

        GetGame().RPCSingleParam(
            player,
            RPC_AUCTION_LIST,
            new Param1<ref array<ref AuctionListing>>(auctions),
            true,
            player.GetIdentity()
        );
    }

    static void PlaceBidRPC(PlayerBase player, string auctionID, int amount)
    {
        // Server-side validation
        if (!GetGame().IsServer())
            return;

        BiddingSystem bidding = new BiddingSystem();
        bidding.PlaceBid(player, auctionID, amount);
    }

    static void BuyoutRPC(PlayerBase player, string auctionID)
    {
        // Server-side validation
        if (!GetGame().IsServer())
            return;

        BuyoutSystem buyout = new BuyoutSystem();
        buyout.BuyoutAuction(player, auctionID);
    }
}
```

---

## Complete Implementation

### File Structure

```
auction_system/
├── config.cpp                    ← Auction definitions
├── auctions.json                 ← Auction data
└── scripts/
    ├── 3_Game/
    │   ├── AuctionManager.c      ← Core auction tracking
    │   ├── AuctionListing.c      ← Individual auction
    │   ├── BiddingSystem.c       ← Bid management
    │   ├── BuyoutSystem.c        ← Instant purchase
    │   ├── BankManager.c         ← Currency storage
    │   └── MailDelivery.c        ← Item delivery
    ├── 4_World/
    │   └── Classes/
    │       ├── AuctionTerminal.c ← Auction terminal
    │       ├── Mailbox.c         ← Mailbox entity
    │       └── PlayerBase/
    │           └── playerbase_auction.c ← Player integration
    └── 5_Mission/
        ├── AuctionBrowserUI.c    ← Marketplace UI
        ├── AuctionDetailsUI.c    ← Auction details
        └── CreateAuctionUI.c     ← Create auction UI
```

---

## Integration Patterns

### Currency System Integration

```c
class AuctionCurrencyIntegration
{
    void OnAuctionCompleted(AuctionListing auction)
    {
        // Transfer currency
        if (auction.GetBids().Count() > 0)
        {
            AuctionBid winningBid = auction.GetBids()[auction.GetBids().Count() - 1];

            // Deposit to seller
            BankManager.GetInstance().Deposit(
                auction.GetSellerID(),
                winningBid.Amount
            );
        }
    }
}
```

---

## Anti-Scam Measures

### Prevention Systems

1. **Item Validation**
   - Validate item type
   - Check item condition
   - Verify attachments

2. **Bid Validation**
   - Check bidder has funds
   - Validate bid amount
   - Prevent self-bidding

3. **Time Limits**
   - Auction expiration
   - Bid time limits
   - Auto-complete

4. **Permission Checks**
   - Ownership verification
   - Distance requirements
   - Admin oversight

---

## Troubleshooting

### Auction not appearing

**Check:**
1. Auction status is "active"
2. Auction not expired
3. Seller has valid items

### Bids not working

**Check:**
1. Player has sufficient funds
2. Bid amount higher than current
3. Bidder not seller

### Items not delivered

**Check:**
1. Mail delivery system loaded
2. Recipient mailbox exists
3. Items valid for mail

---

*Last updated: 2025-01-18*
