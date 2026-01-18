# Full Economy Integration - Trader + Quest + Auction

**Status:** ✅ Working Example
**Complexity:** Expert
**Integration Level:** Trader System + Quest System + Auction System

---

## ⚡ 30-Second Summary

Complete economy ecosystem combining Trader, Quest, and Auction systems. Players earn currency from quests, buy/sell with traders, participate in auctions, and progress through reputation tiers.

---

## 🎯 Features

### Complete Economy Loop

```
Quests → Currency → Traders → Items → Auctions → Currency → Quests
```

### Integration Points

1. **Trader System**
   - NPC traders buy/sell items
   - Reputation-based discounts
   - Special quest items

2. **Quest System**
   - Give trader currency as rewards
   - Auction objectives
   - Reputation rewards
   - Special quests from traders

3. **Auction System**
   - P2P item trading
   - Quest rewards tradable
   - Auction quests
   - Mail delivery

---

## 📁 File Structure

```
full_economy_integration/
├── README.md                        ← This file
├── config.cpp                       ← All NPC definitions
├── economy_quests.json              ← Quest data
├── economy_config.json              ← Economy settings
└── scripts/
    ├── 3_Game/
    │   ├── EconomyManager.c         ← Core economy manager
    │   ├── CurrencyManager.c        ← Unified currency
    │   ├── ReputationManager.c      ← Unified reputation
    │   └── EconomyEventHandler.c    ← Event handling
    ├── 4_World/
    │   └── Classes/
    │       ├── EconomyTrader.c      ← Integrated trader
    │       ├── QuestGiver.c         ← Quest giver NPC
    │       ├── AuctionTerminal.c    ← Auction terminal
    │       └── PlayerBase/
    │           └── playerbase_economy.c ← Complete player integration
    └── 5_Mission/
        └── EconomyUI.c              ← Unified UI
```

---

## 💡 Key Concepts

### 1. Unified Currency System

**Single Currency Pool:**
- Quests give currency
- Traders accept currency
- Auctions use currency
- Bank stores currency

```c
class CurrencyManager
{
    protected ref map<string, int> m_Balances;

    int GetBalance(string playerID)
    {
        return m_Balances.Get(playerID);
    }

    void AddCurrency(string playerID, int amount)
    {
        int current = GetBalance(playerID);
        m_Balances.Set(playerID, current + amount);
    }

    bool HasBalance(string playerID, int amount)
    {
        return GetBalance(playerID) >= amount;
    }
}
```

---

### 2. Reputation System

**Unified Reputation:**
- Completing quests → +Reputation
- Trading with traders → +Reputation
- Successful auctions → +Reputation

**Reputation Tiers:**
```
0-99:      Stranger      - No benefits
100-499:   Acquaintance   - 5% discount, access to basic quests
500-999:   Friend         - 10% discount, access to rare quests
1000-2499: Ally           - 15% discount, auction listing fee waived
2500+:     Partner        - 20% discount, exclusive quests
```

---

### 3. Economy Flow

**Player Journey:**

1. **New Player** (0 reputation)
   - Accepts starter quests
   - Completes quests → Earns currency
   - Buys basic items from traders

2. **Progressing Player** (500 reputation)
   - Completes trader quests
   - Sells items on auction
   - Gets 10% discount at traders

3. **Advanced Player** (2500+ reputation)
   - Access to exclusive quests
   - No auction fees
   - 20% discount everywhere
   - Can create player shops

---

## 🔧 Implementation

### EconomyManager.c

```c
class EconomyManager
{
    protected ref CurrencyManager m_CurrencyManager;
    protected ref ReputationManager m_ReputationManager;
    protected ref QuestManager m_QuestManager;
    protected ref AuctionManager m_AuctionManager;
    protected ref TraderManager m_TraderManager;

    protected static ref EconomyManager m_Instance;

    static EconomyManager GetInstance()
    {
        if (!m_Instance)
        {
            m_Instance = new EconomyManager();
        }
        return m_Instance;
    }

    void EconomyManager()
    {
        m_CurrencyManager = CurrencyManager.GetInstance();
        m_ReputationManager = ReputationManager.GetInstance();
        m_QuestManager = QuestManager.GetInstance();
        m_AuctionManager = AuctionManager.GetInstance();
        m_TraderManager = TraderManager.GetInstance();
    }

    // Quest Completion
    void OnQuestCompleted(PlayerBase player, QuestInstance quest)
    {
        string playerID = player.GetIdentity().GetId();

        // Give currency reward
        foreach (ref QuestRewardConfig reward : quest.GetRewards())
        {
            if (reward.Type == "currency")
            {
                m_CurrencyManager.AddCurrency(playerID, reward.Amount);
            }

            if (reward.Type == "reputation")
            {
                m_ReputationManager.AddReputation(playerID, reward.Amount);
            }
        }

        // Check for follow-up quests
        QuestDefinition def = m_QuestManager.GetQuestDefinition(quest.GetQuestID());
        if (def && def.FollowUpQuest > 0)
        {
            QuestDefinition followUp = m_QuestManager.GetQuestDefinition(def.FollowUpQuest);

            // Check reputation requirement
            if (followUp.ReputationRequirement <= m_ReputationManager.GetReputation(playerID))
            {
                NotificationSystem.Notify(player, "New quest available: " + followUp.Title);
            }
        }
    }

    // Trader Transaction
    void OnTraderPurchase(PlayerBase player, string itemType, int amount)
    {
        string playerID = player.GetIdentity().GetId();

        // Calculate discount
        float discount = m_ReputationManager.GetDiscount(playerID);
        int finalAmount = amount * (1.0 - discount);

        // Deduct currency
        m_CurrencyManager.AddCurrency(playerID, -finalAmount);

        // Add reputation (small amount for trading)
        m_ReputationManager.AddReputation(playerID, 5);
    }

    // Auction Completion
    void OnAuctionCompleted(AuctionListing auction)
    {
        // Give reputation to seller
        m_ReputationManager.AddReputation(auction.GetSellerID(), 10);

        // Give reputation to buyer
        if (auction.GetBids().Count() > 0)
        {
            AuctionBid winningBid = auction.GetBids()[auction.GetBids().Count() - 1];
            m_ReputationManager.AddReputation(winningBid.BidderID, 10);
        }

        // Update auction quest objectives
        UpdateAuctionQuests(auction);
    }

    void UpdateAuctionQuests(AuctionListing auction)
    {
        array<Man> players = new array<Man>;
        GetGame().GetPlayers(players);

        foreach (Man man : players)
        {
            PlayerBase player = PlayerBase.Cast(man);
            if (player)
            {
                array<ref QuestInstance> quests = m_QuestManager.GetPlayerQuests(
                    player.GetIdentity().GetId()
                );

                foreach (QuestInstance quest : quests)
                {
                    quest.UpdateAuctionObjective(auction);
                }
            }
        }
    }
}
```

---

### ReputationManager.c

```c
class ReputationManager
{
    protected ref map<string, int> m_PlayerReputation;

    protected static ref ReputationManager m_Instance;

    static ReputationManager GetInstance()
    {
        if (!m_Instance)
        {
            m_Instance = new ReputationManager();
        }
        return m_Instance;
    }

    void ReputationManager()
    {
        m_PlayerReputation = new map<string, int>;
        LoadReputation();
    }

    void LoadReputation()
    {
        string filePath = "$profile:Economy/reputation.json";

        if (!FileExist(filePath))
            return;

        JsonFileLoader<map<string, int>>::LoadJsonFile(filePath, m_PlayerReputation);
    }

    void SaveReputation()
    {
        string filePath = "$profile:Economy/reputation.json";

        JsonSaveManager json = new JsonSaveManager();
        json.SaveToFile(filePath, m_PlayerReputation);
    }

    int GetReputation(string playerID)
    {
        if (!m_PlayerReputation.Contains(playerID))
            m_PlayerReputation.Set(playerID, 0);

        return m_PlayerReputation.Get(playerID);
    }

    void AddReputation(string playerID, int amount)
    {
        int current = GetReputation(playerID);
        m_PlayerReputation.Set(playerID, current + amount);

        // Check for tier up
        int newReputation = current + amount;
        CheckTierUp(playerID, current, newReputation);

        SaveReputation();
    }

    void CheckTierUp(string playerID, int oldRep, int newRep)
    {
        string oldTier = GetTierName(oldRep);
        string newTier = GetTierName(newRep);

        if (oldTier != newTier)
        {
            NotificationSystem.NotifyByID(
                playerID,
                "Reputation increased! You are now: " + newTier
            );
        }
    }

    string GetTierName(int reputation)
    {
        if (reputation >= 2500)
            return "Partner (20% discount)";
        else if (reputation >= 1000)
            return "Ally (15% discount)";
        else if (reputation >= 500)
            return "Friend (10% discount)";
        else if (reputation >= 100)
            return "Acquaintance (5% discount)";
        else
            return "Stranger (no discount)";
    }

    float GetDiscount(string playerID)
    {
        int reputation = GetReputation(playerID);

        if (reputation >= 2500)
            return 0.20;
        else if (reputation >= 1000)
            return 0.15;
        else if (reputation >= 500)
            return 0.10;
        else if (reputation >= 100)
            return 0.05;
        else
            return 0.0;
    }

    bool CanAcceptQuest(string playerID, int reputationRequirement)
    {
        return GetReputation(playerID) >= reputationRequirement;
    }
}
```

---

### Economy Quest Examples

#### Starter Quest

```json
{
    "ID": 5001,
    "Title": "First Steps",
    "Descriptions": [
        "Welcome to the economy!",
        "Collect 10 apples",
        "Sell them to a trader"
    ],
    "ObjectiveText": "Collect 10 Apples and sell to trader",
    "ReputationRequirement": 0,
    "Rewards": [
        {
            "Type": "currency",
            "Amount": 500
        },
        {
            "Type": "reputation",
            "Amount": 50
        }
    ],
    "Objectives": [
        {
            "ObjectiveType": 4,
            "Collections": [{
                "Amount": 10,
                "ClassName": "Apple",
                "QuantityPercent": -1
            }]
        }
    ]
}
```

#### Advanced Quest (Reputation Gated)

```json
{
    "ID": 5002,
    "Title": "Master Trader",
    "Descriptions": [
        "Only the most trusted can accept this quest.",
        "Buy an AK74 from trader",
        "Sell it on auction for profit"
    ],
    "ObjectiveText": "Buy AK74 from trader, sell on auction",
    "ReputationRequirement": 500,
    "Rewards": [
        {
            "Type": "currency",
            "Amount": 5000
        },
        {
            "Type": "reputation",
            "Amount": 200
        }
    ],
    "Objectives": [
        {
            "ObjectiveType": 6,
            "AuctionAction": "sell",
            "ItemClassName": "AK74",
            "MaxPrice": 1500
        }
    ]
}
```

#### Exclusive Quest (Partner Only)

```json
{
    "ID": 5003,
    "Title": "Economy Master",
    "Descriptions": [
        "You have mastered the economy!",
        "Complete 10 auctions",
        "Earn 10000 reputation total"
    ],
    "ObjectiveText": "Complete 10 auctions, earn 10000 reputation",
    "ReputationRequirement": 2500,
    "Rewards": [
        {
            "Type": "auction_item",
            "ClassName": "M4A1",
            "StartingBid": 0,
            "BuyoutPrice": 0,
            "Message": "Exclusive reward for Economy Masters!"
        }
    ],
    "Objectives": [
        {
            "ObjectiveType": 6,
            "AuctionAction": "complete",
            "RequiredCount": 10
        }
    ]
}
```

---

### PlayerBase Integration

```c
modded class PlayerBase
{
    protected ref EconomyManager m_EconomyManager;

    override void OnConnect()
    {
        super.OnConnect();

        // Initialize economy
        m_EconomyManager = EconomyManager.GetInstance();

        // Show economy status
        ShowEconomyStatus();
    }

    void ShowEconomyStatus()
    {
        string playerID = GetIdentity().GetId();

        int currency = CurrencyManager.GetInstance().GetBalance(playerID);
        int reputation = ReputationManager.GetInstance().GetReputation(playerID);
        string tier = ReputationManager.GetInstance().GetTierName(reputation);

        string message = "Economy Status:\\n";
        message += "Currency: " + currency + "\\n";
        message += "Reputation: " + reputation + "\\n";
        message += "Tier: " + tier;

        NotificationSystem.Notify(this, message);
    }

    bool ChatCommand_Economy()
    {
        ShowEconomyStatus();
        return true;
    }

    bool ChatCommand_AcceptQuest(array<string> args)
    {
        if (args.Count() < 2)
        {
            NotificationSystem.Notify(this, "Usage: /accept_quest <quest_id>");
            return false;
        }

        int questID = args[1].ToInt();
        string playerID = GetIdentity().GetId();

        // Check reputation requirement
        QuestDefinition def = QuestManager.GetInstance().GetQuestDefinition(questID);
        if (def && def.ReputationRequirement > 0)
        {
            if (!ReputationManager.GetInstance().CanAcceptQuest(playerID, def.ReputationRequirement))
            {
                NotificationSystem.Notify(this, "Requires reputation: " + def.ReputationRequirement);
                return false;
            }
        }

        return QuestManager.GetInstance().AcceptQuest(this, questID);
    }
}
```

---

## 📝 Usage Examples

### Example 1: Check Economy Status

```
/economy
```

**Output:**
```
Economy Status:
Currency: 5000
Reputation: 550
Tier: Friend (10% discount)
```

---

### Example 2: Accept Reputation-Gated Quest

```
/accept_quest 5002
```

**Requires:** 500 reputation

---

### Example 3: Buy from Trader with Discount

```
/buy AK74
```

**With 550 reputation (10% discount):**
- Original: 1000
- Discounted: 900

---

### Example 4: Complete Quest for Rewards

```
/complete_quest 5001
```

**Rewards:**
- 500 currency
- 50 reputation

---

### Example 5: Participate in Auction

```
/bid auction_12345 2000
```

**Effect:**
- Places bid
- On completion: +10 reputation

---

## ⚠️ Important Notes

### Reputation Persistence

**Saved:**
- On reputation change
- On player disconnect

**Location:**
- `$profile:Economy/reputation.json`

### Currency Flow

**Sources:**
- Quest rewards
- Selling to traders
- Auction sales

**Sinks:**
- Buying from traders
- Auction bids
- Auction buyouts

### Quest Availability

**Reputation Gates:**
- 0 rep: Basic quests
- 100 rep: Intermediate quests
- 500 rep: Advanced quests
- 2500 rep: Exclusive quests

---

## 🔗 Related Examples

→ [Trader+Quest Integration](../trader_quest_integration/) - Partial integration
→ [Quest+Auction Integration](../quest_auction_integration/) - Partial integration
→ [Full Economy Integration](../full_economy_integration/) - This example

---

## 🐛 Troubleshooting

### Currency not syncing

**Check:**
1. CurrencyManager singleton initialized
2. Save/Load functions working
3. Player identity valid

### Reputation not increasing

**Check:**
1. Quest completion event fired
2. ReputationManager.AddReputation() called
3. SaveReputation() working

### Discount not applied

**Check:**
1. Player reputation > 100
2. GetDiscount() returning correct value
3. Trader using discounted price

### Quest not available

**Check:**
1. Reputation requirement met
2. Prerequisite quests completed
3. Quest is active

---

## 📊 Economy Balance

### Recommended Settings

**Quest Rewards:**
- Easy: 100-500 currency, 10-50 reputation
- Medium: 500-2000 currency, 50-200 reputation
- Hard: 2000-10000 currency, 200-500 reputation

**Trader Prices:**
- Buy from player: 50-80% of sell price
- Sell to player: 120-150% of buy price

**Auction Fees:**
- Listing fee: 5% (waived at 1000+ reputation)
- Sale fee: 10% (reduced to 5% at 2500+ reputation)

---

## 🎯 Integration Benefits

1. **Complete Economy Loop** - Self-sustaining economy
2. **Player Progression** - Clear advancement path
3. **Multiple Playstyles** - Questers, traders, auctioneers
4. **Social Interaction** - P2P trading, competition
5. **Replayability** - Reputation tiers unlock new content
6. **Server Engagement** - Regular players return for quests/auctions

---

*Last updated: 2025-01-18*
