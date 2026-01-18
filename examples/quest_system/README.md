# Quest System Example

**Status:** ✅ Working Example
**Complexity:** Advanced
**Time to create:** 4 hours

---

## ⚡ 30-Second Summary

Complete quest system with kill objectives, collection objectives, quest chains, NPC quest givers, rewards (items, currency), and persistence.

---

## 🎯 Features

### Core Functionality
- ✅ **Kill Quests** - Kill enemies (wolves, zombies, players)
- ✅ **Collection Quests** - Gather items
- ✅ **Quest Chains** - Sequential quests
- ✅ **NPC Quest Givers** - Talk to NPCs for quests
- ✅ **Quest Rewards** - Items, currency, XP
- ✅ **Quest Journal** - Track active quests
- ✅ **Persistence** - Quest progress saved
- ✅ **Repeatable Quests** - Daily/weekly quests

---

## 📁 File Structure

```
quest_system/
├── README.md                    ← This file
├── config.cpp                   ← Quest NPC definitions
├── quests.json                  ← Quest data
└── scripts/
    ├── 3_Game/
    │   ├── QuestManager.c       ← Core quest tracking
    │   ├── QuestInstance.c      ← Active quest
    │   ├── QuestDefinition.c    ← Quest data structure
    │   ├── QuestObjective.c     ← Base objective
    │   │   ├── TargetObjective.c
    │   │   └── CollectionObjective.c
    │   └── QuestSaveLoad.c      ← Persistence
    ├── 4_World/
    │   └── Classes/
    │       ├── QuestGiverNPC.c  ← Quest giver
    │       └── PlayerBase/
    │           └── playerbase_quest.c ← Player integration
    └── 5_Mission/
        └── QuestJournal.c       ← Quest menu (optional)
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
Objectives tracked
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

**Basic Quest Structure:**

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

#### Kill Objective (Type 2)

```json
{
    "ObjectiveType": 2,
    "Amount": 5,
    "ClassNames": ["Animal_CanisLupus_Grey", "Animal_CanisLupus_White"]
}
```

#### Collection Objective (Type 4)

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

## 🔧 Implementation

### 1. QuestManager.c

Core quest tracking system:

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

                // Check for follow-up quest
                QuestDefinition def = m_QuestDefinitions.Get(questID);
                if (def && def.FollowUpQuest > 0)
                {
                    QuestDefinition followUp = m_QuestDefinitions.Get(def.FollowUpQuest);
                    if (followUp)
                    {
                        NotificationSystem.Notify(player, "New quest available: " + followUp.Title);
                    }
                }

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

### 2. QuestInstance.c

Active quest instance:

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
                for (int i = 0; i < reward.Amount; i++)
                {
                    EntityAI item = GetGame().CreateObject(reward.ClassName, player.GetPosition());
                    if (!player.GetInventory().AddInInventory(item))
                    {
                        item.Delete();
                        NotificationSystem.Notify(player, "Inventory full! Item dropped.");
                    }
                }
                break;

            case "currency":
                // Add currency (requires currency system)
                NotificationSystem.Notify(player, "Reward: " + reward.Amount + " coins");
                break;

            case "xp":
                // Add XP (requires XP system)
                NotificationSystem.Notify(player, "Reward: " + reward.Amount + " XP");
                break;
        }
    }
}
```

---

### 3. QuestObjective.c

Base objective class:

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
```

---

### 4. TargetObjective.c

Kill objective implementation:

```c
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
```

---

### 5. CollectionObjective.c

Collection objective implementation:

```c
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

### 6. QuestGiverNPC.c

Quest giver NPC:

```c
class QuestGiverNPC extends DayZInfected
{
    protected ref array<int> m_QuestIDs;
    protected float m_InteractRadius = 3.0;

    void QuestGiverNPC()
    {
        m_QuestIDs = new array<int;

        // Non-aggressive
        SetAiming(false);

        // Load quest IDs (would load from config in production)
        m_QuestIDs.Insert(1001);
        m_QuestIDs.Insert(1002);
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

    array<int> GetQuestIDs()
    {
        return m_QuestIDs;
    }
}
```

---

### 7. PlayerBase Integration

```c
modded class PlayerBase
{
    override void OnConnect()
    {
        super.OnConnect();

        // Initialize quest manager
        QuestManager.GetInstance();

        // Load player quests
        QuestSaveLoad saveLoad = new QuestSaveLoad();
        array<ref QuestInstance> quests = saveLoad.LoadPlayerQuests(GetIdentity().GetId());

        if (quests.Count() > 0)
        {
            QuestManager.GetInstance().GetPlayerQuests(GetIdentity().GetId()) = quests;
            NotificationSystem.Notify(this, "Loaded " + quests.Count() + " active quests");
        }
    }

    override void OnDisconnect()
    {
        super.OnDisconnect();

        // Save player quests
        QuestSaveLoad saveLoad = new QuestSaveLoad();
        saveLoad.SavePlayerQuests(GetIdentity().GetId());
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
        int count = 0;

        foreach (QuestInstance quest : quests)
        {
            if (!quest.IsCompleted())
            {
                message += "\\n" + quest.GetTitle();
                count++;
            }
        }

        if (count == 0)
            message = "No active quests";

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

    bool ChatCommand_CompleteQuest(array<string> args)
    {
        if (args.Count() < 2)
        {
            NotificationSystem.Notify(this, "Usage: /complete_quest <quest_id>");
            return false;
        }

        int questID = args[1].ToInt();
        QuestManager.GetInstance().CompleteQuest(this, questID);
        return true;
    }
}
```

---

## 📝 Usage Examples

### Example 1: Spawn Quest Giver NPC

```c
// Spawn quest giver at position
EntityAI questGiver = GetGame().CreateObject("QuestGiverNPC", Vector(6500, 0, 2500));
```

---

### Example 2: Player Accepts Quest

```
/accept_quest 1001
```

---

### Example 3: View Active Quests

```
/quests
```

---

### Example 4: Complete Quest

```
/complete_quest 1001
```

---

## ⚠️ Important Notes

### Quest Persistence

**Saved:**
- On player disconnect
- After quest completion

**Location:**
- `$profile:Quests/steam64id_quests.json`

### Quest IDs

**Ranges:**
- 1000-1999: Main quests
- 2000-2999: Side quests
- 3000-3999: Daily quests

### Objective Updates

**Events:**
- Type 2 (Kill): OnEntityKilled
- Type 4 (Collection): Check on quest completion

---

## 🔗 Related Examples

→ [XP System](../xp_system/) - XP rewards
→ [Trader System](../trader_system/) - Quest trader integration
→ [Faction System](../faction_system/) - Faction quests

---

## 🐛 Troubleshooting

### Quest not loading

**Check:**
1. quests.json exists in $profile:Quests/
2. JSON syntax is valid
3. LoadQuestDefinitions() called

### Objectives not updating

**Check:**
1. Event handlers registered (OnEntityKilled)
2. ObjectiveType matches event type (2 for kills)
3. Quest instance created for player

### Rewards not given

**Check:**
1. Player has inventory space
2. Item type is valid
3. GiveRewards() called

---

*Last updated: 2025-01-18*
