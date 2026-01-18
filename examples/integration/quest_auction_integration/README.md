# Quest + Auction Integration Example

**Status:** ✅ Working Example
**Complexity:** Advanced
**Integration Level:** Quest System + Auction System

---

## ⚡ 30-Second Summary

Integration example showing how to combine Quest System with Auction System. Quests reward auction items, and completing auctions can fulfill quest objectives.

---

## 🎯 Features

### Integration Points

1. **Auction Quests** - Quests to buy/sell items on auction
2. **Auction Rewards** - Quests reward rare auction items
3. **Bid Objectives** - Quest objectives require placing bids
4. **Auction Completion** - Completing auctions fulfills quest objectives
5. **Mail Integration** - Quest rewards delivered via mail

---

## 📁 File Structure

```
quest_auction_integration/
├── README.md                    ← This file
├── auction_quests.json          ← Quest data with auction objectives
└── scripts/
    ├── 3_Game/
    │   ├── AuctionQuestManager.c ← Manages auction-quest integration
    │   └── AuctionQuestObjective.c ← Special auction objective
    ├── 4_World/
    │   └── Classes/
    │       └── PlayerBase/
    │           └── playerbase_auction_quest.c ← Player integration
    └── 5_Mission/
        └── AuctionQuestUI.c     ← Combined UI
```

---

## 💡 Key Concepts

### 1. Auction Quest Objective

**Quest Objective:**
```json
{
    "ObjectiveType": 6,
    "AuctionAction": "buy",
    "ItemClassName": "AK74",
    "MaxPrice": 5000
}
```

**Implementation:**
```c
class AuctionQuestObjective extends QuestObjective
{
    protected string m_AuctionAction; // "buy" or "sell"
    protected string m_ItemClassName;
    protected int m_MaxPrice;
    protected bool m_Completed;

    override void OnAuctionCompleted(AuctionListing auction)
    {
        if (m_AuctionAction == "buy" && auction.GetWinnerID() == GetPlayerID())
        {
            if (auction.ContainsItem(m_ItemClassName) && auction.GetFinalPrice() <= m_MaxPrice)
            {
                m_Completed = true;
            }
        }
    }
}
```

---

### 2. Quest Rewards via Auction

**Quest Reward:**
```json
{
    "Type": "auction_item",
    "ClassName": "M4A1",
    "StartingBid": 0,
    "BuyoutPrice": 1000
}
```

**Implementation:**
```c
void GiveAuctionItemReward(PlayerBase player, ref QuestRewardConfig reward)
{
    // Create auction with quest item
    array<EntityAI> items = new array<EntityAI>;
    items.Insert(CreateQuestItem(reward.ClassName));

    // Create free auction (starting bid 0)
    string auctionID = AuctionManager.GetInstance().CreateAuction(
        player,
        items,
        reward.StartingBid,
        reward.BuyoutPrice,
        86400 // 24 hours
    );

    NotificationSystem.Notify(player, "Quest reward auction created: " + auctionID);
}
```

---

### 3. Auction Completion Triggers Quest

**When Auction Completes:**
```c
class AuctionQuestManager
{
    void OnAuctionCompleted(AuctionListing auction)
    {
        // Check all players' quest objectives
        array<Man> players = new array<Man>;
        GetGame().GetPlayers(players);

        foreach (Man man : players)
        {
            PlayerBase player = PlayerBase.Cast(man);
            if (player)
            {
                UpdateAuctionQuestObjectives(player, auction);
            }
        }
    }

    void UpdateAuctionQuestObjectives(PlayerBase player, AuctionListing auction)
    {
        array<ref QuestInstance> quests = QuestManager.GetInstance().GetPlayerQuests(
            player.GetIdentity().GetId()
        );

        foreach (QuestInstance quest : quests)
        {
            if (quest.IsCompleted())
                continue;

            quest.UpdateAuctionObjective(auction);
        }
    }
}
```

---

## 🔧 Implementation

### AuctionQuestObjective.c

```c
class AuctionQuestObjective extends QuestObjective
{
    protected string m_AuctionAction; // "buy", "sell", "bid"
    protected string m_ItemClassName;
    protected int m_MaxPrice;
    protected int m_MinBidAmount;
    protected bool m_Completed;

    void AuctionQuestObjective(ref QuestObjectiveConfig config)
    {
        m_ObjectiveType = 6; // Auction objective type
        m_AuctionAction = config.AuctionAction;
        m_ItemClassName = config.ItemClassName;
        m_MaxPrice = config.MaxPrice;
        m_MinBidAmount = config.MinBidAmount;
        m_Completed = false;
    }

    override void UpdateProgress(Params data)
    {
        Param1<AuctionListing> param = Param1<AuctionListing>.Cast(data);

        if (!param)
            return;

        AuctionListing auction = param.param1;

        switch (m_AuctionAction)
        {
            case "buy":
                CheckBuyObjective(auction);
                break;
            case "sell":
                CheckSellObjective(auction);
                break;
            case "bid":
                CheckBidObjective(auction);
                break;
        }
    }

    void CheckBuyObjective(AuctionListing auction)
    {
        // Check if player won auction with specific item
        if (auction.GetWinnerID() == GetPlayerID())
        {
            if (auction.ContainsItem(m_ItemClassName))
            {
                if (auction.GetFinalPrice() <= m_MaxPrice)
                {
                    m_Completed = true;
                    Print("[Quest] Auction buy objective completed!");
                }
            }
        }
    }

    void CheckSellObjective(AuctionListing auction)
    {
        // Check if player sold item on auction
        if (auction.GetSellerID() == GetPlayerID())
        {
            if (auction.ContainsItem(m_ItemClassName))
            {
                if (auction.GetFinalPrice() >= m_MaxPrice) // m_MaxPrice is min sale price
                {
                    m_Completed = true;
                    Print("[Quest] Auction sell objective completed!");
                }
            }
        }
    }

    void CheckBidObjective(AuctionListing auction)
    {
        // Check if player placed bid
        foreach (AuctionBid bid : auction.GetBids())
        {
            if (bid.BidderID == GetPlayerID())
            {
                if (bid.Amount >= m_MinBidAmount)
                {
                    m_Completed = true;
                    Print("[Quest] Auction bid objective completed!");
                    break;
                }
            }
        }
    }

    override bool IsCompleted()
    {
        return m_Completed;
    }

    override string GetObjectiveText()
    {
        switch (m_AuctionAction)
        {
            case "buy":
                return "Buy " + m_ItemClassName + " for max " + m_MaxPrice + " coins";
            case "sell":
                return "Sell " + m_ItemClassName + " for min " + m_MaxPrice + " coins";
            case "bid":
                return "Place bid of at least " + m_MinBidAmount + " coins";
            default:
                return "Auction objective";
        }
    }
}
```

---

### Quest with Auction Objective

```json
{
    "ID": 4001,
    "Title": "Smart Shopper",
    "Descriptions": [
        "Buy an AK74 on the auction house",
        "Pay no more than 5000 coins",
        "Complete the auction to finish the quest"
    ],
    "ObjectiveText": "Buy AK74 for max 5000 coins",
    "Rewards": [
        {
            "Type": "currency",
            "Amount": 2000
        }
    ],
    "Objectives": [
        {
            "ObjectiveType": 6,
            "AuctionAction": "buy",
            "ItemClassName": "AK74",
            "MaxPrice": 5000
        }
    ]
}
```

---

### Quest with Auction Reward

```json
{
    "ID": 4002,
    "Title": "Master Trader",
    "Descriptions": [
        "Complete 5 auctions",
        "Either buy or sell",
        "Receive a rare M4A1 as reward"
    ],
    "ObjectiveText": "Complete 5 auctions (0/5)",
    "Rewards": [
        {
            "Type": "auction_item",
            "ClassName": "M4A1",
            "StartingBid": 0,
            "BuyoutPrice": 0,
            "Message": "Quest reward - rare M4A1!"
        }
    ],
    "Objectives": [
        {
            "ObjectiveType": 6,
            "AuctionAction": "complete",
            "RequiredCount": 5
        }
    ]
}
```

---

### Auction Completion Hook

```c
modded class AuctionListing
{
    override void CompleteAuction()
    {
        // Complete auction normally
        super.CompleteAuction();

        // Notify quest system
        AuctionQuestManager.GetInstance().OnAuctionCompleted(this);
    }

    override void Buyout(string buyerID)
    {
        // Buyout normally
        super.Buyout(buyerID);

        // Notify quest system
        AuctionQuestManager.GetInstance().OnAuctionCompleted(this);
    }
}
```

---

### Quest Reward as Auction

```c
modded class QuestInstance
{
    override void GiveRewards(PlayerBase player)
    {
        foreach (ref QuestRewardConfig reward : m_Definition.Rewards)
        {
            switch (reward.Type)
            {
                case "auction_item":
                    GiveAuctionItemReward(player, reward);
                    break;

                default:
                    super.GiveRewards(player);
                    break;
            }
        }
    }

    void GiveAuctionItemReward(PlayerBase player, ref QuestRewardConfig reward)
    {
        // Create the reward item
        EntityAI item = GetGame().CreateObject(reward.ClassName, player.GetPosition());

        if (!item)
        {
            NotificationSystem.Notify(player, "Failed to create reward item!");
            return;
        }

        // Create auction with the item
        array<EntityAI> items = new array<EntityAI>;
        items.Insert(item);

        string auctionID = AuctionManager.GetInstance().CreateAuction(
            player,
            items,
            reward.StartingBid,
            reward.BuyoutPrice,
            86400 // 24 hours
        );

        NotificationSystem.Notify(player, "Quest reward auction created: " + auctionID);
        NotificationSystem.Notify(player, "Check your auctions to claim the item!");

        // Alternatively, send directly via mail
        // MailDelivery.GetInstance().SendItems(player.GetIdentity().GetId(), reward.Item, reward.Message);
    }
}
```

---

## 📝 Usage Examples

### Example 1: Accept Auction Quest

```
/accept_quest 4001
```

**Quest:** Buy AK74 for max 5000 coins on auction

---

### Example 2: Complete Auction (Objective)

```
/buyout auction_12345
```

**This completes the "buy" objective if:**
- Auction contains AK74
- Price ≤ 5000
- Player won the auction

---

### Example 3: Get Auction Reward

```
/complete_quest 4002
```

**Creates auction** with M4A1 reward item for player to claim.

---

### Example 4: Track Auction Quests

```
/quests
```

**Output:**
```
Active Quests:
- Smart Shopper - Buy AK74 for max 5000 coins (0/1)
- Master Trader - Complete 5 auctions (3/5)
```

---

## ⚠️ Important Notes

### Auction Quest Objective Types

**Type 6 (Auction) Actions:**
- `"buy"` - Win auction with specific item
- `"sell"` - Sell item via auction
- `"bid"` - Place bid on auction
- `"complete"` - Participate in auction

### Quest Reward Delivery

**Two options:**
1. **Create Auction** - Player must bid on own auction
2. **Direct Mail** - Item sent via mail system

---

## 🔗 Related Examples

→ [Quest System](../quest_system/) - Standalone quests
→ [Auction System](../auction_system/) - Standalone auctions
→ [Trader+Quest Integration](../trader_quest_integration/) - Previous integration
→ [Full Economy Integration](../full_economy_integration/) - Next integration

---

## 🐛 Troubleshooting

### Auction objective not completing

**Check:**
1. Auction action type matches ("buy", "sell", "bid")
2. Item class name matches exactly
3. Price requirements met
4. Auction completed successfully

### Quest reward auction not created

**Check:**
1. Reward type is "auction_item"
2. Item class name is valid
3. AuctionManager initialized
4. Player has valid identity

### Mail delivery not working

**Check:**
1. MailDelivery system initialized
2. Player mailbox exists
3. Item valid for mail

---

## 📊 Integration Benefits

1. **Quest Variety** - Auction objectives add diversity
2. **Economy Activity** - Quests drive auction participation
3. **Reward Flexibility** - Auction rewards are tradable
4. **Player Choice** - Players can sell quest rewards

---

*Last updated: 2025-01-18*
