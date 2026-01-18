# Auction/P2P Trading System Example

**Status:** ✅ Working Example
**Complexity:** Advanced
**Time to create:** 6 hours

---

## ⚡ 30-Second Summary

Complete player-to-player auction system with item listings, bidding, buyout, mail delivery, banking integration, and security measures.

---

## 🎯 Features

### Core Functionality
- ✅ **Auction Listings** - List items for auction
- ✅ **Bidding System** - Place bids on items
- ✅ **Buyout System** - Instant purchase
- ✅ **Mail Delivery** - Receive auction items via mail
- ✅ **Banking** - Currency storage and transfers
- ✅ **Marketplace UI** - Browse auctions
- ✅ **Security** - Anti-sam validation
- ✅ **Time Limits** - Auction expiration

---

## 📁 File Structure

```
auction_system/
├── README.md                    ← This file
├── config.cpp                   ← Terminal definitions
├── auctions.json                ← Auction data
└── scripts/
    ├── 3_Game/
    │   ├── AuctionManager.c     ← Core auction tracking
    │   ├── AuctionListing.c     ← Individual auction
    │   ├── BankManager.c        ← Currency storage
    │   └── MailDelivery.c       ← Item delivery
    ├── 4_World/
    │   └── Classes/
    │       ├── AuctionTerminal.c ← Auction terminal
    │       ├── Mailbox.c        ← Mailbox entity
    │       └── PlayerBase/
    │           └── playerbase_auction.c ← Player integration
    └── 5_Mission/
        └── AuctionBrowserUI.c   ← Marketplace UI
```

---

## 💡 Key Concepts

### 1. Auction Lifecycle

```
Player lists item
    ↓
Auction created (with starting bid & buyout)
    ↓
Other players browse marketplace
    ↓
Players place bids OR buyout instantly
    ↓
Auction ends (time OR buyout)
    ↓
Items sent via mail to winner
    ↓
Currency transferred to seller
```

---

### 2. Security Measures

**Anti-Scam:**
- Item validation (type, condition, attachments)
- Bid validation (amount, ownership)
- Permission checks (seller, bidder verification)
- Time limits (auction expiration)
- Refund system (outbid bidders refunded)

---

## 🔧 Implementation Overview

### AuctionManager.c

Core auction tracking system:

```c
class AuctionManager
{
    protected ref map<string, ref AuctionListing> m_Auctions;

    string CreateAuction(PlayerBase player, array<EntityAI> items, int startingBid, int buyoutPrice, float duration)
    {
        // Creates auction, returns auction ID
    }

    AuctionListing GetAuction(string auctionID)
    {
        // Returns auction by ID
    }

    array<ref AuctionListing> GetActiveAuctions()
    {
        // Returns all active auctions
    }
}
```

---

### AuctionListing.c

Individual auction:

```c
class AuctionListing
{
    protected string m_ID;
    protected string m_SellerID;
    protected ref array<ref AuctionItem> m_Items;
    protected int m_StartingBid;
    protected int m_CurrentBid;
    protected int m_BuyoutPrice;
    protected float m_EndTime;

    void PlaceBid(AuctionBid bid)
    {
        // Validates bid, refunds previous bidder
    }

    void Buyout(string buyerID)
    {
        // Instant purchase, transfers items and currency
    }

    void CompleteAuction()
    {
        // Ends auction, distributes items and currency
    }
}
```

---

### BankManager.c

Currency storage:

```c
class BankManager
{
    protected ref map<string, int> m_PlayerBalances;

    int GetBalance(string playerID)
    {
        // Returns player balance
    }

    void Deposit(string playerID, int amount)
    {
        // Adds currency to account
    }

    void Withdraw(string playerID, int amount)
    {
        // Removes currency from account
    }
}
```

---

### MailDelivery.c

Item delivery system:

```c
class MailDelivery
{
    void SendItems(string recipientID, array<ref AuctionItem> items, string message)
    {
        // Sends items to player's mailbox
    }

    array<ref MailParcel> GetMailbox(string playerID)
    {
        // Returns player's mailbox
    }
}
```

---

## 📝 Usage Examples

### Example 1: Browse Auctions

```
/browse_auctions
```

**Output:**
```
Active Auctions:
- PlayerName - AK74 - Current bid: 1500 (Buyout: 5000)
- Player2 - M4A1 - Current bid: 2000 (Buyout: 6000)
```

---

### Example 2: Create Auction

```
/create_auction
```

**Creates auction for item in hand with default prices.**

---

### Example 3: Place Bid

```
/bid auction_12345 2000
```

**Places bid of 2000 on auction.**

---

### Example 4: Buyout Auction

```
/buyout auction_12345
```

**Instantly purchases auction for buyout price.**

---

### Example 5: Check Balance

```
/balance
```

**Shows bank balance.**

---

### Example 6: Check Mailbox

```
/mail
```

**Shows received mail parcels.**

---

## ⚠️ Important Notes

### Security

**Item Validation:**
- Check item type (blacklist certain items)
- Verify item condition
- Validate attachments
- Confirm ownership

**Bid Validation:**
- Sufficient funds check
- Bid amount higher than current
- Bidder not seller
- Prevent self-bidding

### Time Management

**Auction Duration:**
- Default: 24 hours (86400 seconds)
- Configurable per auction
- Automatic expiration check

**Expiration Check:**
- Run every 60 seconds
- Auto-complete expired auctions
- Return items to seller if no bids

---

## 🔗 Related Examples

→ [Trader System](../trader_system/) - NPC trading
→ [Quest System](../quest_system/) - Quest rewards
→ [XP System](../xp_system/) - XP rewards

---

## 🐛 Troubleshooting

### Auction not appearing

**Check:**
1. Auction status is "active"
2. Auction not expired
3. Items valid for auction
4. Seller inventory space (if returned)

### Bids not working

**Check:**
1. Player has sufficient bank balance
2. Bid amount higher than current bid
3. Bidder is not seller
4. Auction is active

### Items not delivered

**Check:**
1. Mail delivery system initialized
2. Recipient mailbox exists
3. Items valid for mail
4. Winner has inventory space

### Currency not transferred

**Check:**
1. Bank system initialized
2. Balances saved correctly
3. Transaction logging enabled
4. Sufficient funds for buyout

---

## 📊 Configuration

### auctions.json

```json
{
    "Auctions": []
}
```

Auctions are saved and loaded from this file.

---

## 🎨 UI Integration

The system supports UI integration for:
- Auction browser (marketplace)
- Auction details
- Create auction form
- Bidding interface
- Mailbox viewer
- Bank account viewer

UI components are optional - chat commands work without UI.

---

*Last updated: 2025-01-18*
