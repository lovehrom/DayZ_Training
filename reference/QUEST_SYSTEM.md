# Quest System - Complete Reference

**Status:** ✅ Complete Reference
**Target:** Advanced modders
**Goal:** Comprehensive quest system implementation guide

---

## ⚡ 30-Second Summary

Complete quest system for DayZ with JSON-based quest definitions, objective tracking (kill, collect, explore), quest chains, rewards (XP, items, currency), NPC quest givers, quest journal UI, and persistence.

---

## 📋 Table of Contents

1. [Quest System Architecture](#quest-system-architecture)
2. [Quest Definitions (JSON)](#quest-definitions-json)
3. [Quest Objectives](#quest-objectives)
4. [Quest Tracking](#quest-tracking)
5. [Quest Rewards](#quest-rewards)
6. [Quest Chains](#quest-chains)
7. [Quest Giver NPCs](#quest-giver-npcs)
8. [Quest Journal UI](#quest-journal-ui)
9. [Save/Load System](#saveload-system)
10. [Event Integration](#event-integration)
11. [Complete Implementation](#complete-implementation)
12. [Integration Patterns](#integration-patterns)
13. [Security Best Practices](#security-best-practices)
14. [Troubleshooting](#troubleshooting)

---

## Quest System Architecture

### System Components

```
Quest System
├── QuestManager           ← Core quest tracking
├── QuestDefinition        ← Quest data structure
├── QuestObjective         ← Objective logic
├── QuestReward           ← Reward distribution
├── QuestGiverNPC         ← NPC interaction
├── QuestJournalUI        ← User interface
└── QuestSaveLoad         ← Persistence
```

### Data Flow

```
Player accepts quest
    ↓
QuestManager creates active quest
    ↓
Player completes objectives
    ↓
QuestObjective tracks progress
    ↓
All objectives complete
    ↓
QuestReward distributes rewards
    ↓
Quest marked as completed
    ↓
Follow-up quest unlocked (if any)
```

---

## Quest Definitions (JSON)

### Basic Quest Structure

```json
{
    "ConfigVersion": 1,
    "ID": 1001,
    "Type": 1,
    "Title": "Hunt Wolves",
    "Descriptions": [
        "Wolves are attacking the village!",
        "Kill 5 wolves",
        "Return to the hunter for your reward"
    ],
    "ObjectiveText": "Kill 5 Wolves (0/5)",
    "FollowUpQuest": 1002,
    "Repeatable": 0,
    "IsDailyQuest": 0,
    "IsWeeklyQuest": 0,
    "CancelQuestOnPlayerDeath": 0,
    "Autocomplete": 0,
    "IsGroupQuest": 0,
    "Rewards": [...],
    "QuestGiverIDs": [1],
    "QuestTurnInIDs": [1],
    "Objectives": [...],
    "QuestColor": 0,
    "ReputationReward": 100,
    "PreQuestIDs": [],
    "Active": 1
}
```

### Quest Properties

| Property | Type | Description |
|----------|------|-------------|
| `ID` | int | Unique quest identifier |
| `Type` | int | Quest category (1=main, 2=side, 3=daily) |
| `Title` | string | Quest name |
| `Descriptions` | array<string> | Quest stages [intro, progress, completion] |
| `ObjectiveText` | string | Objective display text |
| `FollowUpQuest` | int | Next quest ID in chain (-1 if none) |
| `Repeatable` | bool | Can be repeated after completion |
| `IsDailyQuest` | bool | Resets daily |
| `IsWeeklyQuest` | bool | Resets weekly |
| `CancelQuestOnPlayerDeath` | bool | Cancel on death |
| `Autocomplete` | bool | Auto-complete when objectives done |
| `IsGroupQuest` | bool | Shared with group |
| `QuestGiverIDs` | array<int> | NPCs that give this quest |
| `QuestTurnInIDs` | array<int> | NPCs that complete this quest |
| `PreQuestIDs` | array<int> | Prerequisite quest IDs |
| `Active` | bool | Quest is enabled |

---

## Quest Objectives

### Objective Types

#### Type 1: Travel Objective

Visit a specific location.

```json
{
    "ObjectiveType": 1,
    "Position": {"x": 5000, "y": 0, "z": 3000},
    "Radius": 50.0,
    "ShowDistance": true
}
```

**Implementation:**

```c
class TravelObjective extends QuestObjective
{
    protected vector m_Position;
    protected float m_Radius;

    override bool CheckObjective(PlayerBase player)
    {
        float distance = vector.Distance(player.GetPosition(), m_Position);
        return distance <= m_Radius;
    }

    override string GetObjectiveText()
    {
        float distance = vector.Distance(player.GetPosition(), m_Position);
        return "Travel to location (" + distance.ToString() + "m)";
    }
}
```

---

#### Type 2: Target/Kill Objective

Kill specific entities (animals, zombies, players).

```json
{
    "ObjectiveType": 2,
    "Amount": 5,
    "ClassNames": [
        "Animal_CanisLupus_Grey",
        "Animal_CanisLupus_White"
    ],
    "AllowedWeapons": [],
    "CountSelfKill": 0,
    "MaxDistance": -1.0
}
```

**Properties:**
- `Amount`: Number of kills required
- `ClassNames`: Entity types to kill
- `AllowedWeapons`: Optional weapon restrictions
- `CountSelfKill`: Include self-inflicted kills
- `MaxDistance`: Maximum kill distance (-1 = unlimited)

**Implementation:**

```c
class TargetObjective extends QuestObjective
{
    protected int m_Amount;
    protected ref array<string> m_ClassNames;
    protected int m_CurrentCount;

    override void OnEntityKilled(EntityAI victim, EntityAI killer)
    {
        if (!IsTarget(victim.GetType()))
            return;

        m_CurrentCount++;

        if (m_CurrentCount >= m_Amount)
            SetCompleted(true);

        UpdateObjectiveText();
    }

    protected bool IsTarget(string className)
    {
        foreach (string target : m_ClassNames)
        {
            if (className == target)
                return true;
        }
        return false;
    }

    override string GetObjectiveText()
    {
        return "Kill targets: (" + m_CurrentCount + "/" + m_Amount + ")";
    }
}
```

---

#### Type 3: Craft Objective

Craft specific items.

```json
{
    "ObjectiveType": 3,
    "Amount": 3,
    "ClassNames": ["AK74", "M4A1"],
    "NeedAny": false
}
```

**Implementation:**

```c
class CraftObjective extends QuestObjective
{
    protected int m_Amount;
    protected ref array<string> m_ClassNames;
    protected bool m_NeedAny;
    protected int m_CurrentCount;

    override void OnItemCrafted(string itemType)
    {
        if (!IsTarget(itemType))
            return;

        m_CurrentCount++;

        if (m_CurrentCount >= m_Amount)
            SetCompleted(true);

        UpdateObjectiveText();
    }

    override string GetObjectiveText()
    {
        return "Craft items: (" + m_CurrentCount + "/" + m_Amount + ")";
    }
}
```

---

#### Type 4: Collection Objective

Collect/gather specific items.

```json
{
    "ObjectiveType": 4,
    "Collections": [{
        "Amount": 6,
        "ClassName": "CowSteakMeat",
        "QuantityPercent": -1
    }],
    "ShowDistance": 1,
    "NeedAnyCollection": 0
}
```

**Properties:**
- `Collections`: Array of collection requirements
- `Amount`: Quantity needed
- `ClassName`: Item type to collect
- `QuantityPercent`: Health percentage (-1 = any)
- `ShowDistance`: Show distance to nearest item
- `NeedAnyCollection`: Any collection counts (OR logic)

**Implementation:**

```c
class CollectionObjective extends QuestObjective
{
    protected ref array<ref CollectionRequirement> m_Collections;
    protected bool m_ShowDistance;

    override bool CheckObjective(PlayerBase player)
    {
        foreach (CollectionRequirement req : m_Collections)
        {
            if (!HasRequiredItems(player, req))
                return false;
        }
        return true;
    }

    protected bool HasRequiredItems(PlayerBase player, CollectionRequirement req)
    {
        int count = 0;

        array<EntityAI> items = new array<EntityAI>;
        player.GetInventory().EnumerateInventory(InventoryMode.LOCAL, items);

        foreach (EntityAI item : items)
        {
            if (item.GetType() == req.ClassName)
            {
                if (req.QuantityPercent < 0 || item.GetHealth01() >= req.QuantityPercent)
                    count++;
            }
        }

        return count >= req.Amount;
    }

    override string GetObjectiveText()
    {
        string text;
        foreach (CollectionRequirement req : m_Collections)
        {
            int current = GetCurrentCount(req.ClassName);
            text += req.ClassName + " (" + current + "/" + req.Amount + ") ";
        }
        return text;
    }
}

class CollectionRequirement
{
    string ClassName;
    int Amount;
    float QuantityPercent;
}
```

---

#### Type 5: Exploration Objective

Discover specific locations.

```json
{
    "ObjectiveType": 5,
    "Positions": [
        {"x": 5000, "y": 0, "z": 3000},
        {"x": 6000, "y": 0, "z": 4000}
    ],
    "Radius": 100.0,
    "VisitOrder": false
}
```

**Implementation:**

```c
class ExplorationObjective extends QuestObjective
{
    protected ref array<vector> m_Positions;
    protected float m_Radius;
    protected bool m_VisitOrder;
    protected ref array<bool> m_Visited;

    override bool CheckObjective(PlayerBase player)
    {
        for (int i = 0; i < m_Positions.Count(); i++)
        {
            if (m_Visited[i])
                continue;

            float distance = vector.Distance(player.GetPosition(), m_Positions[i]);
            if (distance <= m_Radius)
            {
                m_Visited[i] = true;
                OnLocationDiscovered(i);

                if (!m_VisitOrder)
                    continue; // Check next location
                else
                    break; // Must visit in order
            }
        }

        // Check if all visited
        foreach (bool visited : m_Visited)
        {
            if (!visited)
                return false;
        }
        return true;
    }

    override string GetObjectiveText()
    {
        int visitedCount = 0;
        foreach (bool visited : m_Visited)
        {
            if (visited) visitedCount++;
        }
        return "Discover locations: (" + visitedCount + "/" + m_Positions.Count() + ")";
    }
}
```

---

## Quest Tracking

### QuestManager Class

```c
class QuestManager
{
    protected ref map<string, ref array<ref QuestInstance>> m_PlayerQuests;
    protected ref map<int, ref QuestDefinition> m_QuestDefinitions;

    protected static ref QuestManager m_Instance;

    static QuestManager GetInstance()
    {
        if (!m_Instance)
        {
            m_Instance = new QuestManager();
        }
        return m_Instance;
    }

    void QuestManager()
    {
        m_PlayerQuests = new map<string, ref array<ref QuestInstance>>;
        m_QuestDefinitions = new map<int, ref QuestDefinition>;
        LoadQuestDefinitions();
    }

    void LoadQuestDefinitions()
    {
        string filePath = "$profile:Quests/quest_definitions.json";

        if (!FileExist(filePath))
        {
            Print("[Quest] No quest definitions file found!");
            return;
        }

        JsonFileLoader<array<ref QuestDefinition>>::LoadJsonFile(filePath, m_QuestDefinitions);

        Print("[Quest] Loaded " + m_QuestDefinitions.Count() + " quest definitions");
    }

    QuestDefinition GetQuestDefinition(int questID)
    {
        if (!m_QuestDefinitions.Contains(questID))
            return null;

        return m_QuestDefinitions.Get(questID);
    }

    array<ref QuestInstance> GetPlayerQuests(string steam64ID)
    {
        if (!m_PlayerQuests.Contains(steam64ID))
        {
            m_PlayerQuests.Set(steam64ID, new array<ref QuestInstance>);
        }

        return m_PlayerQuests.Get(steam64ID);
    }

    bool AcceptQuest(PlayerBase player, int questID)
    {
        QuestDefinition def = GetQuestDefinition(questID);
        if (!def)
        {
            Print("[Quest] Invalid quest ID: " + questID);
            return false;
        }

        // Check prerequisites
        if (!CheckPrerequisites(player, def))
        {
            NotificationSystem.Notify(player, "Prerequisites not met!");
            return false;
        }

        // Check if already active
        array<ref QuestInstance> quests = GetPlayerQuests(player.GetIdentity().GetId());
        foreach (QuestInstance quest : quests)
        {
            if (quest.GetQuestID() == questID)
            {
                NotificationSystem.Notify(player, "Quest already active!");
                return false;
            }
        }

        // Create quest instance
        QuestInstance instance = new QuestInstance(def);
        quests.Insert(instance);

        NotificationSystem.Notify(player, "Quest accepted: " + def.Title);

        // Notify quest start
        OnQuestStarted(player, questID);

        return true;
    }

    bool CheckPrerequisites(PlayerBase player, QuestDefinition def)
    {
        if (def.PreQuestIDs.Count() == 0)
            return true;

        array<ref QuestInstance> quests = GetPlayerQuests(player.GetIdentity().GetId());

        foreach (int preQuestID : def.PreQuestIDs)
        {
            bool completed = false;

            foreach (QuestInstance quest : quests)
            {
                if (quest.GetQuestID() == preQuestID && quest.IsCompleted())
                {
                    completed = true;
                    break;
                }
            }

            if (!completed)
                return false;
        }

        return true;
    }

    void UpdateQuestProgress(PlayerBase player, int objectiveType, Params data)
    {
        array<ref QuestInstance> quests = GetPlayerQuests(player.GetIdentity().GetId());

        foreach (QuestInstance quest : quests)
        {
            if (quest.IsCompleted())
                continue;

            quest.UpdateObjective(objectiveType, data);
        }
    }

    void CompleteQuest(PlayerBase player, int questID)
    {
        array<ref QuestInstance> quests = GetPlayerQuests(player.GetIdentity().GetId());

        foreach (QuestInstance quest : quests)
        {
            if (quest.GetQuestID() == questID && quest.CanComplete())
            {
                quest.SetCompleted(true);
                quest.GiveRewards(player);

                NotificationSystem.Notify(player, "Quest completed: " + quest.GetTitle());

                // Check for follow-up quest
                QuestDefinition def = GetQuestDefinition(questID);
                if (def.FollowUpQuest > 0)
                {
                    QuestDefinition followUp = GetQuestDefinition(def.FollowUpQuest);
                    if (followUp)
                    {
                        NotificationSystem.Notify(player, "New quest available: " + followUp.Title);
                    }
                }

                OnQuestCompleted(player, questID);
                break;
            }
        }
    }

    void OnQuestStarted(PlayerBase player, int questID)
    {
        // Hook for quest start events
    }

    void OnQuestCompleted(PlayerBase player, int questID)
    {
        // Hook for quest completion events
    }
}
```

---

### QuestInstance Class

```c
class QuestInstance
{
    protected ref QuestDefinition m_Definition;
    protected ref array<ref QuestObjective> m_Objectives;
    protected bool m_Completed;
    protected float m_StartTime;

    void QuestInstance(QuestDefinition def)
    {
        m_Definition = def;
        m_Objectives = new array<ref QuestObjective>;

        // Create objectives from definition
        foreach (ref QuestObjectiveConfig objConfig : def.Objectives)
        {
            QuestObjective obj = CreateObjective(objConfig);
            if (obj)
                m_Objectives.Insert(obj);
        }

        m_Completed = false;
        m_StartTime = GetGame().GetTickTime();
    }

    QuestObjective CreateObjective(ref QuestObjectiveConfig config)
    {
        switch (config.ObjectiveType)
        {
            case 1: return new TravelObjective(config);
            case 2: return new TargetObjective(config);
            case 3: return new CraftObjective(config);
            case 4: return new CollectionObjective(config);
            case 5: return new ExplorationObjective(config);
            default: return null;
        }
    }

    int GetQuestID()
    {
        return m_Definition.ID;
    }

    string GetTitle()
    {
        return m_Definition.Title;
    }

    bool IsCompleted()
    {
        return m_Completed;
    }

    bool CanComplete()
    {
        if (m_Completed)
            return false;

        foreach (QuestObjective obj : m_Objectives)
        {
            if (!obj.IsCompleted())
                return false;
        }
        return true;
    }

    void SetCompleted(bool completed)
    {
        m_Completed = completed;
    }

    void UpdateObjective(int objectiveType, Params data)
    {
        foreach (QuestObjective obj : m_Objectives)
        {
            if (obj.GetObjectiveType() == objectiveType && !obj.IsCompleted())
            {
                obj.UpdateProgress(data);
            }
        }
    }

    void GiveRewards(PlayerBase player)
    {
        foreach (ref QuestRewardConfig reward : m_Definition.Rewards)
        {
            GiveReward(player, reward);
        }
    }

    void GiveReward(PlayerBase player, ref QuestRewardConfig reward)
    {
        switch (reward.Type)
        {
            case "item":
                GiveItemReward(player, reward);
                break;
            case "xp":
                GiveXPReward(player, reward);
                break;
            case "currency":
                GiveCurrencyReward(player, reward);
                break;
            case "reputation":
                GiveReputationReward(player, reward);
                break;
        }
    }

    void GiveItemReward(PlayerBase player, ref QuestRewardConfig reward)
    {
        for (int i = 0; i < reward.Amount; i++)
        {
            EntityAI item = GetGame().CreateObject(reward.ClassName, player.GetPosition());

            if (!item)
                continue;

            if (!player.GetInventory().AddInInventory(item))
            {
                item.Delete();
                NotificationSystem.Notify(player, "Inventory full! Item dropped on ground.");
            }
        }
    }

    void GiveXPReward(PlayerBase player, ref QuestRewardConfig reward)
    {
        // Integrate with XP system
        if (XPManager)
        {
            XPManager.GetInstance().AddXP(player.GetIdentity().GetId(), reward.Amount);
        }
    }

    void GiveCurrencyReward(PlayerBase player, ref QuestRewardConfig reward)
    {
        // Integrate with currency system
        if (CurrencyManager)
        {
            CurrencyManager.GetInstance().AddCurrency(player.GetIdentity().GetId(), reward.Amount);
        }
    }

    void GiveReputationReward(PlayerBase player, ref QuestRewardConfig reward)
    {
        // Integrate with faction system
        if (FactionManager)
        {
            FactionManager.GetInstance().AddReputation(player, reward.Faction, reward.Amount);
        }
    }
}
```

---

## Quest Rewards

### Reward Configuration

```json
{
    "Rewards": [
        {
            "Type": "item",
            "ClassName": "AK74",
            "Amount": 1,
            "HealthPercent": 100.0,
            "Attachments": ["Mag_STANAG_30Rnd", "Battery9V"]
        },
        {
            "Type": "xp",
            "Skill": "Hunting",
            "Amount": 500
        },
        {
            "Type": "currency",
            "Amount": 1000
        },
        {
            "Type": "reputation",
            "Faction": "Traders",
            "Amount": 100
        }
    ]
}
```

### QuestRewardConfig Class

```c
class QuestRewardConfig
{
    string Type;           // "item", "xp", "currency", "reputation"
    string ClassName;      // For item rewards
    int Amount;            // Quantity
    float HealthPercent;   // Item health
    string Skill;          // For XP rewards
    string Faction;        // For reputation rewards
}
```

---

## Quest Chains

### Follow-Up Quests

```json
{
    "ID": 1001,
    "Title": "Part 1: The Beginning",
    "FollowUpQuest": 1002,
    ...
},
{
    "ID": 1002,
    "Title": "Part 2: The Journey",
    "PreQuestIDs": [1001],
    "FollowUpQuest": 1003,
    ...
},
{
    "ID": 1003,
    "Title": "Part 3: The Conclusion",
    "PreQuestIDs": [1002],
    ...
}
```

### Quest Chain Management

```c
class QuestChainManager
{
    void OnQuestCompleted(PlayerBase player, int questID)
    {
        QuestDefinition def = QuestManager.GetInstance().GetQuestDefinition(questID);

        if (!def || def.FollowUpQuest <= 0)
            return;

        // Automatically offer follow-up quest
        QuestDefinition followUp = QuestManager.GetInstance().GetQuestDefinition(def.FollowUpQuest);

        if (followUp)
        {
            // Check if player meets prerequisites
            if (CheckPrerequisites(player, followUp))
            {
                NotificationSystem.Notify(player, "New quest available: " + followUp.Title);

                // Mark follow-up as available for this player
                SetQuestAvailable(player, followUp.ID);
            }
        }
    }

    void SetQuestAvailable(PlayerBase player, int questID)
    {
        string steam64ID = player.GetIdentity().GetId();

        array<int> availableQuests = GetAvailableQuests(steam64ID);
        availableQuests.Insert(questID);

        SaveAvailableQuests(steam64ID, availableQuests);
    }
}
```

---

## Quest Giver NPCs

### QuestGiverNPC Class

```c
class QuestGiverNPC extends DayZInfected
{
    protected ref array<int> m_QuestIDs;
    protected string m_GiverName;
    protected float m_InteractRadius = 3.0;

    void QuestGiverNPC()
    {
        m_QuestIDs = new array<int>;
        m_GiverName = "Quest Giver";

        // Non-aggressive
        SetAiming(false);

        // Load quest IDs from config
        LoadQuestIDs();
    }

    override void OnInit()
    {
        super.OnInit();

        // Make NPC invulnerable
        SetHealth("GlobalHealth", "Health", 1000);
    }

    void LoadQuestIDs()
    {
        // Load from config or set directly
        m_QuestIDs.Insert(1001);
        m_QuestIDs.Insert(1002);
    }

    array<int> GetQuestIDs()
    {
        return m_QuestIDs;
    }

    bool CanInteract(PlayerBase player)
    {
        float distance = vector.Distance(player.GetPosition(), GetPosition());
        return distance <= m_InteractRadius;
    }

    void OnInteract(PlayerBase player)
    {
        if (!CanInteract(player))
            return;

        ShowQuestMenu(player);
    }

    void ShowQuestMenu(PlayerBase player)
    {
        // Show quest menu via RPC
        GetGame().RPCSingleParam(this, RPC_SHOW_QUEST_MENU, new Param1<ref array<int>>(m_QuestIDs), true, player.GetIdentity());
    }

    void OfferQuest(PlayerBase player, int questID)
    {
        QuestManager.GetInstance().AcceptQuest(player, questID);
    }

    void TurnInQuest(PlayerBase player, int questID)
    {
        QuestManager.GetInstance().CompleteQuest(player, questID);
    }
}
```

---

### Quest Interaction System

```c
modded class PlayerBase
{
    override void OnConnect()
    {
        super.OnConnect();

        // Register quest event handlers
        QuestManager.GetInstance();
    }

    bool ChatCommand_Quests()
    {
        ShowQuestList();
        return true;
    }

    void ShowQuestList()
    {
        array<ref QuestInstance> quests = QuestManager.GetInstance().GetPlayerQuests(GetIdentity().GetId());

        string message = "Active Quests:";

        foreach (QuestInstance quest : quests)
        {
            if (!quest.IsCompleted())
            {
                message += "\\n" + quest.GetTitle();
            }
        }

        NotificationSystem.Notify(this, message);
    }

    bool ChatCommand_AcceptQuest(array<string> args)
    {
        if (args.Count() < 2)
        {
            NotificationSystem.Notify(this, "Usage: /accept_quest <quest_id>");
            return false;
        }

        int questID = args[1].ToInt();
        return QuestManager.GetInstance().AcceptQuest(this, questID);
    }

    override void OnEntityKilled(EntityAI victim, EntityAI killer)
    {
        super.OnEntityKilled(victim, killer);

        // Update kill objectives
        Param2<EntityAI, EntityAI> params = new Param2<EntityAI, EntityAI>(victim, killer);
        QuestManager.GetInstance().UpdateQuestProgress(this, 2, params);
    }

    override void OnItemCrafted(EntityAI item)
    {
        super.OnItemCrafted(item);

        // Update craft objectives
        Param1<string> params = new Param1<string>(item.GetType());
        QuestManager.GetInstance().UpdateQuestProgress(this, 3, params);
    }
}
```

---

## Quest Journal UI

### QuestJournalUI Class

```c
class QuestJournalUI
{
    protected ref QuestJournalData m_Data;
    protected Widget m_RootWidget;

    void QuestJournalUI()
    {
        m_Data = new QuestJournalData();
        LoadQuestData();
    }

    void LoadQuestData()
    {
        string steam64ID = GetGame().GetPlayer().GetIdentity().GetId();
        m_Data.Quests = QuestManager.GetInstance().GetPlayerQuests(steam64ID);
    }

    void Show()
    {
        if (!m_RootWidget)
        {
            CreateLayout();
        }

        m_RootWidget.Show(true);
        UpdateQuestList();
    }

    void Hide()
    {
        if (m_RootWidget)
        {
            m_RootWidget.Show(false);
        }
    }

    void CreateLayout()
    {
        // Create UI layout
        m_RootWidget = GetGame().GetWorkspace().CreateWidgets("DayZ_Gui/Layouts/QuestJournal.layout");
    }

    void UpdateQuestList()
    {
        // Update quest list display
        foreach (QuestInstance quest : m_Data.Quests)
        {
            AddQuestToUI(quest);
        }
    }

    void AddQuestToUI(QuestInstance quest)
    {
        // Add quest to UI list
        TextWidget titleWidget = m_RootWidget.FindAnyWidget("QuestTitle");
        TextWidget objectiveWidget = m_RootWidget.FindAnyWidget("QuestObjectives");

        if (titleWidget)
        {
            titleWidget.SetText(quest.GetTitle());
        }

        if (objectiveWidget)
        {
            objectiveWidget.SetText(quest.GetObjectiveText());
        }
    }
}

class QuestJournalData
{
    array<ref QuestInstance> Quests;
}
```

---

## Save/Load System

### Quest Persistence

```c
class QuestSaveLoad
{
    void SavePlayerQuests(string steam64ID)
    {
        array<ref QuestInstance> quests = QuestManager.GetInstance().GetPlayerQuests(steam64ID);

        string filePath = "$profile:Quests/" + steam64ID + "_quests.json";

        JsonSaveManager json = new JsonSaveManager();
        json.SaveToFile(filePath, quests);

        Print("[Quest] Saved " + quests.Count() + " quests for player " + steam64ID);
    }

    array<ref QuestInstance> LoadPlayerQuests(string steam64ID)
    {
        string filePath = "$profile:Quests/" + steam64ID + "_quests.json";

        if (!FileExist(filePath))
            return new array<ref QuestInstance>;

        array<ref QuestInstance> quests = new array<ref QuestInstance>;

        JsonFileLoader<array<ref QuestInstance>>::LoadJsonFile(filePath, quests);

        Print("[Quest] Loaded " + quests.Count() + " quests for player " + steam64ID);

        return quests;
    }
}

modded class PlayerBase
{
    override void OnConnect()
    {
        super.OnConnect();

        // Load player quests
        QuestSaveLoad saveLoad = new QuestSaveLoad();
        array<ref QuestInstance> quests = saveLoad.LoadPlayerQuests(GetIdentity().GetId());

        QuestManager.GetInstance().SetPlayerQuests(GetIdentity().GetId(), quests);
    }

    override void OnDisconnect()
    {
        super.OnDisconnect();

        // Save player quests
        QuestSaveLoad saveLoad = new QuestSaveLoad();
        saveLoad.SavePlayerQuests(GetIdentity().GetId());
    }
}
```

---

## Event Integration

### Event Handlers

```c
class QuestEventHandler
{
    void RegisterEvents()
    {
        // Register kill events
        GetGame().GetCallQueue().CallLater(UpdateKillEvents, 1000, true);

        // Register craft events
        // Register collection events
        // Register travel events
    }

    void UpdateKillEvents()
    {
        // Check for kill objectives
    }
}
```

---

## Complete Implementation

### File Structure

```
quest_system/
├── config.cpp                    ← Quest definitions
├── quests.json                   ← Quest data
└── scripts/
    ├── 3_Game/
    │   ├── QuestManager.c        ← Core quest tracking
    │   ├── QuestInstance.c       ← Quest instance
    │   ├── QuestDefinition.c     ← Quest data structure
    │   ├── QuestObjective.c      ← Base objective
    │   │   ├── TravelObjective.c
    │   │   ├── TargetObjective.c
    │   │   ├── CraftObjective.c
    │   │   ├── CollectionObjective.c
    │   │   └── ExplorationObjective.c
    │   ├── QuestReward.c         ← Reward system
    │   └── QuestSaveLoad.c       ← Persistence
    ├── 4_World/
    │   ├── QuestGiverNPC.c       ← NPC interaction
    │   └── QuestBoard.c          ← Quest board object
    └── 5_Mission/
        └── QuestJournal.c        ← UI
```

---

## Integration Patterns

### XP System Integration

```c
class QuestXPReward extends QuestReward
{
    void GiveReward(PlayerBase player)
    {
        if (XPManager)
        {
            XPManager.GetInstance().AddXP(player.GetIdentity().GetId(), m_Amount);
        }
    }
}
```

### Faction System Integration

```c
class QuestFactionReward extends QuestReward
{
    void GiveReward(PlayerBase player)
    {
        if (FactionManager)
        {
            FactionManager.GetInstance().AddReputation(player, m_Faction, m_Amount);
        }
    }
}
```

---

## Security Best Practices

### Input Validation

```c
bool AcceptQuest(PlayerBase player, int questID)
{
    // Validate quest ID
    if (questID <= 0)
        return false;

    // Validate quest exists
    QuestDefinition def = GetQuestDefinition(questID);
    if (!def)
        return false;

    // Validate prerequisites
    if (!CheckPrerequisites(player, def))
        return false;

    // Validate player
    if (!player || !player.GetIdentity())
        return false;

    return true;
}
```

### Server-Side Authority

```c
// Always validate on server
void CompleteQuest(PlayerBase player, int questID)
{
    if (!GetGame().IsServer())
        return;

    // Server-side validation
    QuestInstance quest = GetPlayerQuest(player, questID);
    if (!quest || !quest.CanComplete())
        return;

    // Give rewards on server only
    quest.GiveRewards(player);
}
```

---

## Troubleshooting

### Quest not saving

**Check:**
1. Save path is correct
2. Player Steam64 ID is valid
3. Write permissions on $profile:Quests/

### Quest objectives not updating

**Check:**
1. Event handlers registered
2. Objective type matches event type
3. Quest instance exists for player

### Quest rewards not given

**Check:**
1. Reward configuration is correct
2. Player has inventory space (for items)
3. Integration systems (XP, currency) are loaded

---

*Last updated: 2025-01-18*
