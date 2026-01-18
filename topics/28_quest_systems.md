# Quest Systems - Beginner's Guide

**Status:** ✅ Complete Guide
**Complexity:** Advanced
**Time to implement:** 4-6 hours

---

## ⚡ 30-Second Summary

Quest system allows you to create RPG-style tasks with objectives (kill, collect, explore), rewards (items, XP, currency), quest chains, and NPC quest givers. Perfect for adding progression and narrative to your server.

---

## 🎯 What Are Quest Systems?

### Concept

Quest systems give players objectives to complete for rewards. They're the backbone of RPG progression in games like World of Warcraft, Skyrim, or Fallout.

**In DayZ, quests can:**
- Guide players to content
- Reward exploration
- Create progression
- Tell stories
- Encourage PvP/PvE
- Build economy

### Quest Types

| Type | Description | Example |
|------|-------------|---------|
| **Kill Quests** | Kill specific enemies | "Kill 5 wolves" |
| **Collection Quests** | Gather items | "Collect 10 apples" |
| **Craft Quests** | Craft items | "Craft 3 bandages" |
| **Exploration Quests** | Visit locations | "Discover the airfield" |
| **Delivery Quests** | Transport items | "Deliver package to trader" |

---

## 📁 File Structure

```
quest_system/
├── config.cpp                    ← Quest NPC definitions
├── quests.json                   ← Quest data
└── scripts/
    ├── 3_Game/
    │   ├── QuestManager.c        ← Core quest tracking
    │   ├── QuestInstance.c       ← Active quest
    │   └── QuestObjective.c      ← Objectives
    ├── 4_World/
    │   └── QuestGiverNPC.c       ← Quest giver
    └── 5_Mission/
        └── QuestJournal.c        ← Quest menu
```

---

## 💡 Key Concepts

### 1. Quest Lifecycle

```
Player talks to NPC
    ↓
Quest offered
    ↓
Player accepts quest
    ↓
Objectives tracked (kills, collections, etc.)
    ↓
All objectives complete
    ↓
Player returns to NPC
    ↓
Rewards given
    ↓
Quest completed
```

---

### 2. Quest Definition (JSON)

**Basic Quest:**

```json
{
    "ID": 1001,
    "Title": "Hunt Wolves",
    "Descriptions": [
        "Wolves are attacking the village!",
        "Kill 5 wolves",
        "Return to the hunter for your reward"
    ],
    "ObjectiveText": "Kill 5 Wolves (0/5)",
    "FollowUpQuest": 1002,
    "Repeatable": false,
    "Rewards": [...],
    "QuestGiverIDs": [1],
    "QuestTurnInIDs": [1],
    "Objectives": [...]
}
```

---

### 3. Objective Types

#### Kill Objective

```json
{
    "ObjectiveType": 2,
    "Amount": 5,
    "ClassNames": ["Animal_CanisLupus_Grey", "Animal_CanisLupus_White"]
}
```

#### Collection Objective

```json
{
    "ObjectiveType": 4,
    "Collections": [{
        "Amount": 10,
        "ClassName": "Apple",
        "QuantityPercent": -1
    }]
}
```

---

### 4. Quest Rewards

```json
{
    "Rewards": [
        {
            "Type": "item",
            "ClassName": "AK74",
            "Amount": 1
        },
        {
            "Type": "xp",
            "Amount": 500
        },
        {
            "Type": "currency",
            "Amount": 1000
        }
    ]
}
```

---

## 🔧 Implementation

### Step 1: Create QuestManager.c

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

        array<ref QuestDefinition> quests = new array<ref QuestDefinition>;
        JsonFileLoader<array<ref QuestDefinition>>::LoadJsonFile(filePath, quests);

        foreach (QuestDefinition quest : quests)
        {
            m_QuestDefinitions.Set(quest.ID, quest);
        }

        Print("[Quest] Loaded " + m_QuestDefinitions.Count() + " quest definitions");
    }

    bool AcceptQuest(PlayerBase player, int questID)
    {
        QuestDefinition def = m_QuestDefinitions.Get(questID);
        if (!def)
        {
            Print("[Quest] Invalid quest ID: " + questID);
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

                break;
            }
        }
    }

    array<ref QuestInstance> GetPlayerQuests(string steam64ID)
    {
        if (!m_PlayerQuests.Contains(steam64ID))
        {
            m_PlayerQuests.Set(steam64ID, new array<ref QuestInstance>);
        }

        return m_PlayerQuests.Get(steam64ID);
    }
}
```

---

### Step 2: Create QuestInstance.c

```c
class QuestInstance
{
    protected ref QuestDefinition m_Definition;
    protected ref array<ref QuestObjective> m_Objectives;
    protected bool m_Completed;

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
    }

    QuestObjective CreateObjective(ref QuestObjectiveConfig config)
    {
        switch (config.ObjectiveType)
        {
            case 2: return new TargetObjective(config);
            case 4: return new CollectionObjective(config);
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
                // Spawn item
                EntityAI item = GetGame().CreateObject(reward.ClassName, player.GetPosition());
                player.GetInventory().AddInInventory(item);
                break;

            case "xp":
                // Add XP (requires XP system)
                break;

            case "currency":
                // Add currency (requires currency system)
                break;
        }
    }
}
```

---

### Step 3: Create QuestObjective.c

```c
class QuestObjective
{
    protected int m_ObjectiveType;
    protected bool m_Completed;

    int GetObjectiveType()
    {
        return m_ObjectiveType;
    }

    bool IsCompleted()
    {
        return m_Completed;
    }

    void SetCompleted(bool completed)
    {
        m_Completed = completed;
    }

    void UpdateProgress(Params data)
    {
        // Override in subclasses
    }

    string GetObjectiveText()
    {
        // Override in subclasses
        return "";
    }
}

class TargetObjective extends QuestObjective
{
    protected int m_Amount;
    protected ref array<string> m_ClassNames;
    protected int m_CurrentCount;

    void TargetObjective(ref QuestObjectiveConfig config)
    {
        m_ObjectiveType = config.ObjectiveType;
        m_Amount = config.Amount;
        m_ClassNames = config.ClassNames;
        m_CurrentCount = 0;
    }

    override void UpdateProgress(Params data)
    {
        Param2<EntityAI, EntityAI> params = Param2<EntityAI, EntityAI>.Cast(data);

        if (!params)
            return;

        EntityAI victim = params.param1;
        string victimType = victim.GetType();

        // Check if victim is target
        foreach (string target : m_ClassNames)
        {
            if (victimType == target)
            {
                m_CurrentCount++;

                if (m_CurrentCount >= m_Amount)
                    SetCompleted(true);

                break;
            }
        }
    }

    override string GetObjectiveText()
    {
        return "Kill targets: (" + m_CurrentCount + "/" + m_Amount + ")";
    }
}

class CollectionObjective extends QuestObjective
{
    protected ref array<ref CollectionRequirement> m_Collections;
    protected PlayerBase m_Player;

    void CollectionObjective(ref QuestObjectiveConfig config)
    {
        m_ObjectiveType = config.ObjectiveType;
        m_Collections = config.Collections;
    }

    void SetPlayer(PlayerBase player)
    {
        m_Player = player;
    }

    override bool IsCompleted()
    {
        if (!m_Player)
            return false;

        foreach (CollectionRequirement req : m_Collections)
        {
            if (!HasRequiredItems(req))
                return false;
        }
        return true;
    }

    protected bool HasRequiredItems(CollectionRequirement req)
    {
        int count = 0;

        array<EntityAI> items = new array<EntityAI>;
        m_Player.GetInventory().EnumerateInventory(InventoryMode.LOCAL, items);

        foreach (EntityAI item : items)
        {
            if (item.GetType() == req.ClassName)
            {
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

    int GetCurrentCount(string className)
    {
        int count = 0;

        array<EntityAI> items = new array<EntityAI>;
        m_Player.GetInventory().EnumerateInventory(InventoryMode.LOCAL, items);

        foreach (EntityAI item : items)
        {
            if (item.GetType() == className)
                count++;
        }

        return count;
    }
}
```

---

### Step 4: Create QuestGiverNPC.c

```c
class QuestGiverNPC extends DayZInfected
{
    protected ref array<int> m_QuestIDs;
    protected float m_InteractRadius = 3.0;

    void QuestGiverNPC()
    {
        m_QuestIDs = new array<int>;

        // Non-aggressive
        SetAiming(false);

        // Load quest IDs
        m_QuestIDs.Insert(1001);
    }

    override void OnInit()
    {
        super.OnInit();

        // Make NPC invulnerable
        SetHealth("GlobalHealth", "Health", 1000);
    }

    bool CanInteract(PlayerBase player)
    {
        float distance = vector.Distance(player.GetPosition(), GetPosition());
        return distance <= m_InteractRadius;
    }
}
```

---

### Step 5: PlayerBase Integration

```c
modded class PlayerBase
{
    override void OnConnect()
    {
        super.OnConnect();

        // Initialize quest manager
        QuestManager.GetInstance();
    }

    override void OnEntityKilled(EntityAI victim, EntityAI killer)
    {
        super.OnEntityKilled(victim, killer);

        // Update kill objectives
        Param2<EntityAI, EntityAI> params = new Param2<EntityAI, EntityAI>(victim, killer);
        QuestManager.GetInstance().UpdateQuestProgress(this, 2, params);
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
}
```

---

### Step 6: Create quests.json

```json
{
    "Quests": [
        {
            "ID": 1001,
            "Title": "Hunt Wolves",
            "Descriptions": [
                "Wolves are attacking the village!",
                "Kill 5 wolves",
                "Return to the hunter for your reward"
            ],
            "ObjectiveText": "Kill 5 Wolves (0/5)",
            "FollowUpQuest": -1,
            "Repeatable": false,
            "Rewards": [
                {
                    "Type": "item",
                    "ClassName": "AK74",
                    "Amount": 1
                },
                {
                    "Type": "currency",
                    "Amount": 1000
                }
            ],
            "QuestGiverIDs": [1],
            "QuestTurnInIDs": [1],
            "Objectives": [
                {
                    "ObjectiveType": 2,
                    "Amount": 5,
                    "ClassNames": ["Animal_CanisLupus_Grey", "Animal_CanisLupus_White"]
                }
            ]
        },
        {
            "ID": 1002,
            "Title": "Gather Apples",
            "Descriptions": [
                "We need food for the village!",
                "Collect 10 apples",
                "Bring them to the trader"
            ],
            "ObjectiveText": "Collect 10 Apples (0/10)",
            "FollowUpQuest": -1,
            "Repeatable": true,
            "Rewards": [
                {
                    "Type": "item",
                    "ClassName": "BandageDressing",
                    "Amount": 5
                }
            ],
            "QuestGiverIDs": [2],
            "QuestTurnInIDs": [2],
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
    ]
}
```

---

## 📝 Usage Examples

### Example 1: Create Kill Quest

```json
{
    "ID": 2001,
    "Title": "Zombie Hunter",
    "ObjectiveText": "Kill 10 Zombies (0/10)",
    "Rewards": [
        {"Type": "item", "ClassName": "M4A1", "Amount": 1}
    ],
    "Objectives": [
        {
            "ObjectiveType": 2,
            "Amount": 10,
            "ClassNames": ["ZombieMale", "ZombieFemale"]
        }
    ]
}
```

---

### Example 2: Create Collection Quest

```json
{
    "ID": 2002,
    "Title": "Medic Supplies",
    "ObjectiveText": "Collect 5 Bandages (0/5)",
    "Rewards": [
        {"Type": "currency", "Amount": 500}
    ],
    "Objectives": [
        {
            "ObjectiveType": 4,
            "Collections": [{
                "Amount": 5,
                "ClassName": "BandageDressing",
                "QuantityPercent": -1
            }]
        }
    ]
}
```

---

### Example 3: Quest Chain

```json
{
    "ID": 3001,
    "Title": "Part 1: Beginnings",
    "FollowUpQuest": 3002,
    ...
},
{
    "ID": 3002,
    "Title": "Part 2: Journey",
    "PreQuestIDs": [3001],
    "FollowUpQuest": 3003,
    ...
},
{
    "ID": 3003,
    "Title": "Part 3: End",
    "PreQuestIDs": [3002],
    ...
}
```

---

## ⚠️ Important Notes

### Quest Persistence

**When to save:**
- On player disconnect
- After quest completion
- After objective progress

**Where to save:**
- `$profile:Quests/steam64id_quests.json`

### Quest IDs

**Best practices:**
- Use ranges for quest types
  - 1000-1999: Main quests
  - 2000-2999: Side quests
  - 3000-3999: Daily quests
  - 4000-4999: Event quests

### Objective Updates

**Don't update every frame:**
- Kill objectives: Update on kill event
- Collection objectives: Update on item pickup
- Travel objectives: Check every 5 seconds

---

## 🔗 Related Topics

→ [XP Systems](topics/12_xp_systems.md) - XP rewards
→ [Trader Systems](topics/27_trader_systems.md) - Quest trader integration
→ [Faction Systems](topics/20_faction_systems.md) - Faction quests
→ [Time Events](topics/22_time_events.md) - Timed quests

---

## 🐛 Troubleshooting

### Quest not loading

**Check:**
1. quests.json file exists
2. JSON syntax is valid
3. LoadQuestDefinitions() called

### Objectives not updating

**Check:**
1. Event handlers registered
2. ObjectiveType matches event type
3. Quest instance created for player

### Rewards not given

**Check:**
1. Reward type is correct
2. Player has inventory space
3. Integration systems loaded

---

*Last updated: 2025-01-18*
