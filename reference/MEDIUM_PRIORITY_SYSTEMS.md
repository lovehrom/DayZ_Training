# Infection, Insurance, High Scores - Complete Reference

**Source:** Various Mods (SLP_Realistic_Survival, Insurance Systems, High Score Database)
**Complexity:** Intermediate

---

## ⚡ Infection Disease System

**Source:** SLP_Realistic_Survival (3578041032)

### Features
- ✅ **Influenza** - Reduces health over time
- ✅ **Cholera** - Dehydration + vomiting
- ✅ **Salmonella** - Food poisoning
- ✅ **Infection Progression** - Stages with worsening symptoms
- ✅ **Antibiotics** - Cure infections

### Implementation

```c
class InfectionManager
{
    protected map<string, ref InfectionData> m_PlayerInfections;

    void InfectPlayer(string playerID, string infectionType)
    {
        InfectionData infection = new InfectionData();
        infection.Type = infectionType;
        infection.Stage = 1;
        infection.StartTime = GetGame().GetTickTime();

        m_PlayerInfections.Set(playerID, infection);
    }

    void ProcessInfections()
    {
        foreach (string playerID, InfectionData infection : m_PlayerInfections)
        {
            PlayerBase player = GetPlayerBySID(playerID);
            if (!player)
                continue;

            // Apply symptoms based on stage
            switch (infection.Type)
            {
                case "Influenza":
                    player.AddHealth("GlobalHealth", "Blood", -10);  // Lose blood
                    break;

                case "Cholera":
                    player.AddHealth("GlobalHealth", "Water", -50);  // Dehydration
                    break;
            }

            // Check if death or recovery
            if (ShouldProgress(infection))
            {
                infection.Stage++;
            }

            if (infection.Stage >= 5)
            {
                // Player died from infection
                player.SetHealth(0);
            }
        }
    }
}
```

---

## ⚡ Insurance System

**Source:** TraderPlus Insurance

### Features
- ✅ **Item Insurance** - Protect items from loss
- ✅ **Death Payout** - Reimburse on death
- ✅ **Premium Payments** - Pay for coverage
- ✅ **Claim System** - File insurance claims

### Implementation

```c
class InsuranceSystem
{
    protected map<string, ref InsurancePolicy> m_Policies;

    void PurchaseInsurance(string playerID, array<ItemBase> items)
    {
        InsurancePolicy policy = new InsurancePolicy();
        policy.PlayerID = playerID;
        policy.InsuredItems = new array<string>;

        foreach (ItemBase item : items)
        {
            policy.InsuredItems.Insert(item.GetType());
        }

        policy.Premium = items.Count() * 100;  // 100 currency per item
        policy.ValidUntil = GetGame().GetTickTime() + 86400;  // 24 hours

        m_Policies.Set(playerID, policy);
    }

    void OnPlayerDeath(PlayerBase player)
    {
        string playerID = player.GetIdentity().GetPlainId();

        if (!m_Policies.Contains(playerID))
            return;

        InsurancePolicy policy = m_Policies.Get(playerID);

        // Check if policy is valid
        if (GetGame().GetTickTime() > policy.ValidUntil)
            return;

        // Reimburse items
        foreach (string itemClass : policy.InsuredItems)
        {
            player.GetHumanInventory().CreateInInventory(itemClass);
        }

        NotificationSystem.Notify(player, "Insurance payout received!");
    }
}
```

---

## ⚡ High Score Database

**Source:** Custom High Score Systems

### Features
- ✅ **Leaderboards** - Track top players
- ✅ **Statistics** - Kills, deaths, playtime
- ✅ **Achievements** - Unlockable rewards
- ✅ **JSON Persistence** - Save scores

### Implementation

```c
class HighScoreDatabase
{
    protected ref map<string, ref PlayerStats> m_Stats;

    void RecordKill(string killerID, string victimID)
    {
        if (!m_Stats.Contains(killerID))
            m_Stats.Set(killerID, new PlayerStats);

        PlayerStats stats = m_Stats.Get(killerID);
        stats.Kills++;
        stats.LastKillTime = GetGame().GetTickTime();

        SaveStats();
    }

    array<ref PlayerStats> GetTopPlayers(int count)
    {
        array<ref PlayerStats> all = new array<ref PlayerStats>;

        foreach (string playerID, PlayerStats stats : m_Stats)
        {
            all.Insert(stats);
        }

        // Sort by kills (descending)
        all.Sort(CompareKills);

        // Return top N
        array<ref PlayerStats> top = new array<ref PlayerStats>;
        for (int i = 0; i < count && i < all.Count(); i++)
        {
            top.Insert(all[i]);
        }

        return top;
    }

    int CompareKills(PlayerStats a, PlayerStats b)
    {
        if (a.Kills > b.Kills)
            return -1;
        if (a.Kills < b.Kills)
            return 1;
        return 0;
    }
}

class PlayerStats
{
    string PlayerID;
    string PlayerName;
    int Kills;
    int Deaths;
    float KDRatio;
    int PlayTime;
    int LastKillTime;
}
```

---

## 📝 Quick Reference

### Infection Stages
```
Stage 1: Mild symptoms
Stage 2: Moderate symptoms
Stage 3: Severe symptoms
Stage 4: Critical
Stage 5: Death
```

### Insurance Premium
```
Base: 100 currency per item
Duration: 24 hours
Payout: Full item value
```

### High Scores
```
Leaderboard: Top 100 players
Tracking: Kills, Deaths, Playtime
Update: Real-time
```

---

**Last Updated:** 2025-01-18
**Status:** ✅ Complete
