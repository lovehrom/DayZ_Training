# Trader + Quest Integration Example

**Status:** ✅ Working Example
**Complexity:** Advanced
**Integration Level:** Trader System + Quest System

---

## ⚡ 30-Second Summary

Integration example showing how to combine Trader System with Quest System. Quests give trader currency as rewards, and traders offer special items for quest completion.

---

## 🎯 Features

### Integration Points

1. **Quest Rewards** - Give trader currency as quest rewards
2. **Trader Quests** - NPC traders give quests
3. **Quest Items** - Special quest items sold by traders
4. **Reputation System** - Quest completion increases trader reputation
5. **Discount System** - Higher reputation = better prices

---

## 📁 File Structure

```
trader_quest_integration/
├── README.md                    ← This file
├── config.cpp                   ← Trader & Quest NPC definitions
├── trader_quests.json           ← Quest data with trader rewards
└── scripts/
    ├── 3_Game/
    │   ├── TraderQuestManager.c ← Manages trader-quest integration
    │   └── ReputationSystem.c   ← Reputation tracking
    ├── 4_World/
    │   └── Classes/
    │       └── PlayerBase/
    │           └── playerbase_trader_quest.c ← Player integration
    └── 5_Mission/
        └── TraderQuestUI.c      ← Combined UI
```

---

## 💡 Key Concepts

### 1. Quest Gives Currency

**Quest Reward:**
```json
{
    "Type": "currency",
    "Amount": 1000
}
```

**Implementation:**
```c
void GiveReward(PlayerBase player, ref QuestRewardConfig reward)
{
    if (reward.Type == "currency")
    {
        CurrencyManager.GetInstance().AddCurrency(
            player.GetIdentity().GetId(),
            reward.Amount
        );
    }
}
```

---

### 2. Trader Gives Quests

**Trader NPC:**
```c
class QuestGiverTrader extends TraderNPC
{
    array<int> m_QuestIDs;

    void OnInteract(PlayerBase player)
    {
        // Show trader menu
        // Also offer available quests
        ShowTraderQuestMenu(player);
    }
}
```

---

### 3. Reputation System

**Reputation Tiers:**
- 0-100: Stranger (no discount)
- 100-500: Acquaintance (5% discount)
- 500-1000: Friend (10% discount)
- 1000+: Best Friend (20% discount)

---

## 🔧 Implementation

### TraderQuestManager.c

```c
class TraderQuestManager
{
    protected ref map<string, int> m_PlayerReputation;

    void TraderQuestManager()
    {
        m_PlayerReputation = new map<string, int>;
    }

    void AddReputation(string playerID, int amount)
    {
        int current = GetReputation(playerID);
        m_PlayerReputation.Set(playerID, current + amount);

        // Notify player
        NotificationSystem.NotifyByID(
            playerID,
            "Reputation increased! Current: " + (current + amount)
        );
    }

    int GetReputation(string playerID)
    {
        if (!m_PlayerReputation.Contains(playerID))
            m_PlayerReputation.Set(playerID, 0);

        return m_PlayerReputation.Get(playerID);
    }

    float GetDiscount(string playerID)
    {
        int reputation = GetReputation(playerID);

        if (reputation >= 1000)
            return 0.20; // 20% discount
        else if (reputation >= 500)
            return 0.10; // 10% discount
        else if (reputation >= 100)
            return 0.05; // 5% discount
        else
            return 0.0; // No discount
    }

    int GetDiscountedPrice(string playerID, int originalPrice)
    {
        float discount = GetDiscount(playerID);
        return originalPrice * (1.0 - discount);
    }
}
```

---

### Quest with Currency Reward

```json
{
    "ID": 3001,
    "Title": "Trader's Request",
    "Descriptions": [
        "The trader needs supplies!",
        "Collect 10 apples",
        "Return to the trader for currency reward"
    ],
    "ObjectiveText": "Collect 10 Apples (0/10)",
    "Rewards": [
        {
            "Type": "currency",
            "Amount": 500
        },
        {
            "Type": "reputation",
            "Amount": 100
        }
    ],
    "QuestGiverIDs": [1],
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

---

### Trader NPC with Quests

```c
class QuestGiverTrader extends TraderNPC
{
    protected ref array<int> m_QuestIDs;

    void QuestGiverTrader()
    {
        m_QuestIDs = new array<int;
        m_QuestIDs.Insert(3001); // Trader's Request
        m_QuestIDs.Insert(3002); // Rare Item Hunt
    }

    override void OnInteract(PlayerBase player)
    {
        // Show trader menu
        ShowTraderMenu(player);

        // Show available quests
        ShowAvailableQuests(player);
    }

    void ShowAvailableQuests(PlayerBase player)
    {
        foreach (int questID : m_QuestIDs)
        {
            QuestManager manager = QuestManager.GetInstance();
            QuestDefinition def = manager.GetQuestDefinition(questID);

            if (def && manager.CanAcceptQuest(player, questID))
            {
                NotificationSystem.Notify(player, "Quest available: " + def.Title);
            }
        }
    }
}
```

---

### Quest Completion Integration

```c
modded class QuestInstance
{
    override void GiveRewards(PlayerBase player)
    {
        foreach (ref QuestRewardConfig reward : m_Definition.Rewards)
        {
            switch (reward.Type)
            {
                case "currency":
                    // Give trader currency
                    CurrencyManager.GetInstance().AddCurrency(
                        player.GetIdentity().GetId(),
                        reward.Amount
                    );
                    NotificationSystem.Notify(player, "Reward: " + reward.Amount + " coins");
                    break;

                case "reputation":
                    // Increase trader reputation
                    TraderQuestManager.GetInstance().AddReputation(
                        player.GetIdentity().GetId(),
                        reward.Amount
                    );
                    break;

                case "item":
                    // Give item
                    EntityAI item = GetGame().CreateObject(reward.ClassName, player.GetPosition());
                    player.GetInventory().AddInInventory(item);
                    break;
            }
        }
    }
}
```

---

### Trader Buying with Discount

```c
modded class TraderNPC
{
    override bool BuyItem(PlayerBase player, string itemType)
    {
        TraderItem traderItem = FindItem(itemType);
        if (!traderItem)
            return false;

        // Apply reputation discount
        TraderQuestManager questManager = TraderQuestManager.GetInstance();
        int discountedPrice = questManager.GetDiscountedPrice(
            player.GetIdentity().GetId(),
            traderItem.BuyPrice
        );

        // Check currency
        CurrencyManager currency = CurrencyManager.GetInstance();
        if (!currency.HasCurrency(player.GetIdentity().GetId(), discountedPrice))
        {
            NotificationSystem.Notify(player, "Not enough currency! Need: " + discountedPrice);
            return false;
        }

        // Create item
        EntityAI item = GetGame().CreateObject(itemType, player.GetPosition());
        if (!player.GetInventory().AddInInventory(item))
        {
            item.Delete();
            NotificationSystem.Notify(player, "Inventory full!");
            return false;
        }

        // Deduct currency
        currency.AddCurrency(player.GetIdentity().GetId(), -discountedPrice);

        NotificationSystem.Notify(player, "Bought " + itemType + " for " + discountedPrice + " coins");

        return true;
    }
}
```

---

## 📝 Usage Examples

### Example 1: Accept Trader Quest

```
/accept_quest 3001
```

---

### Example 2: Complete Quest for Currency

```
/complete_quest 3001
```

**Rewards:**
- 500 coins
- 100 reputation

---

### Example 3: Buy from Trader with Discount

```
/buy AK74
```

**With 100 reputation (5% discount):**
- Original price: 1000
- Discounted price: 950

---

### Example 4: Check Reputation

```
/reputation
```

**Output:**
```
Reputation: 550 (Friend - 10% discount)
```

---

## ⚠️ Important Notes

### Reputation Persistence

**Save:**
- On quest completion
- On player disconnect

**Load:**
- On player connect

### Quest Availability

**Traders offer quests:**
- When player is nearby
- If player meets prerequisites
- Daily/weekly limits

---

## 🔗 Related Examples

→ [Trader System](../trader_system/) - Standalone trader
→ [Quest System](../quest_system/) - Standalone quests
→ [Quest+Auction Integration](../quest_auction_integration/) - Next integration

---

## 🐛 Troubleshooting

### Currency not given

**Check:**
1. CurrencyManager initialized
2. Quest reward type is "currency"
3. Player identity valid

### Reputation not increasing

**Check:**
1. Quest reward type is "reputation"
2. TraderQuestManager initialized
3. Reward amount > 0

### Discount not applied

**Check:**
1. Player reputation > 100
2. GetDiscountedPrice() called
3. TraderNPC uses discounted price

---

## 📊 Integration Benefits

1. **Economy Loop** - Quests → Currency → Trading → Quests
2. **Player Progression** - Reputation unlocks discounts
3. **Content Discovery** - Quests guide players to traders
4. **Engagement** - Regular quests keep players returning

---

*Last updated: 2025-01-18*
