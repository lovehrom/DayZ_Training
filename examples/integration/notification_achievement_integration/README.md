# Notifications + Achievement Integration Example

**Status:** ✅ Working Integration Example
**Complexity:** Advanced
**Combines:** notifications + xp_system (achievements)

---

## ⚡ 30-Second Summary

Integration example showing achievement notifications. When players achieve milestones (kills, level up, etc.), they receive both XP rewards and broadcast notifications.

---

## 🎯 Features

### Integration Mechanics
- ✅ **Achievement Tracking** - Track player milestones
- ✅ **XP Rewards** - Give XP on achievements
- ✅ **Broadcast Notifications** - Announce to server
- ✅ **Progress Tracking** - Track achievement progress
- ✅ **Milestone Rewards** - Bonus XP for milestones

---

## 📁 File Structure

```
notification_achievement_integration/
├── README.md                         ← This file
├── config.cpp                        ← Mod configuration
└── scripts/
    └── 4_World/
        └── Classes/
            ├── AchievementManager.c           ← Achievement tracking
            ├── Achievement.c                  ← Achievement data
            └── PlayerBase/
                └── playerbase_achievement.c   ← Integration
```

---

## 🔧 Integration Concept

### How It Works

**Individual Systems:**
- `notifications/` - Send messages to players
- `xp_system/` - Track player XP and levels

**Integration:**
1. Player performs action → Check achievements
2. Achievement complete → Award XP
3. Notification sent → Player notified
4. Broadcast → Server notified

### Achievement Types

| Achievement | Requirement | XP Reward |
|-------------|-------------|-----------|
| First Blood | Kill 1 zombie | 50 XP |
| Zombie Hunter | Kill 100 zombies | 1000 XP |
| Survivor | Survive 1 hour | 200 XP |
| Master Chef | Cook 10 meals | 500 XP |

---

## 💡 Implementation

### 1. Achievement.c

Achievement data:

```c
class Achievement
{
    string ID;
    string Name;
    string Description;
    int Target;         // Target count
    int Current;        // Current progress
    int XPReward;

    void Achievement(string id, string name, string desc, int target, int xp)
    {
        ID = id;
        Name = name;
        Description = desc;
        Target = target;
        Current = 0;
        XPReward = xp;
    }

    // Check if complete
    bool IsComplete()
    {
        return Current >= Target;
    }

    // Get progress percentage
    float GetProgress()
    {
        return (float(Current) / float(Target)) * 100.0;
    }

    // Add progress
    void AddProgress(int amount)
    {
        Current += amount;

        if (Current > Target)
            Current = Target;
    }

    // Reset progress
    void Reset()
    {
        Current = 0;
    }

    // Serialize for storage
    string Serialize()
    {
        return ID + "," + Current;
    }

    // Deserialize from storage
    static Achievement Deserialize(string data)
    {
        array<string> parts = new array<string>;
        data.Split(",", parts);

        string id = parts.Get(0);
        int current = parts.Get(1).ToInt();

        // Create achievement (lookup from registry)
        Achievement achievement = AchievementRegistry.Get(id);
        if (achievement)
        {
            achievement.Current = current;
        }

        return achievement;
    }
}
```

### 2. AchievementManager.c

Manage achievements:

```c
class AchievementManager
{
    protected ref map<string, ref Achievement> m_Achievements;
    protected PlayerBase m_Player;

    void AchievementManager(PlayerBase player)
    {
        m_Player = player;
        m_Achievements = new map<string, ref Achievement>;

        LoadAchievements();
    }

    void LoadAchievements()
    {
        // Create achievements
        m_Achievements.Set("first_blood", new Achievement("first_blood", "First Blood", "Kill your first zombie", 1, 50));
        m_Achievements.Set("zombie_hunter", new Achievement("zombie_hunter", "Zombie Hunter", "Kill 100 zombies", 100, 1000));
        m_Achievements.Set("survivor", new Achievement("survivor", "Survivor", "Survive for 1 hour", 3600, 200));
        m_Achievements.Set("master_chef", new Achievement("master_chef", "Master Chef", "Cook 10 meals", 10, 500));
    }

    // Add progress to achievement
    void AddProgress(string achievementID, int amount)
    {
        Achievement achievement = m_Achievements.Get(achievementID);

        if (!achievement)
            return;

        // Add progress
        achievement.AddProgress(amount);

        // Check if complete
        if (achievement.IsComplete())
        {
            OnAchievementComplete(achievement);
        }
        else
        {
            // Show progress notification
            float progress = achievement.GetProgress();
            NotificationSystem.Notify(m_Player, achievement.Name + ": " + progress + "%");
        }
    }

    // Achievement completed
    void OnAchievementComplete(Achievement achievement)
    {
        // Award XP
        SkillManager skillManager = m_Player.GetSkillManager();
        if (skillManager)
        {
            skillManager.AddXP(achievement.XPReward);
        }

        // Notify player
        NotificationSystem.Notify(m_Player, "Achievement Unlocked: " + achievement.Name + "!");
        NotificationSystem.Notify(m_Player, "+" + achievement.XPReward + " XP");

        // Broadcast to server
        BroadcastAchievement(achievement);

        // Log
        Print("[Achievement] " + m_Player.GetIdentity().GetName() + " unlocked " + achievement.Name);
    }

    // Broadcast achievement to server
    void BroadcastAchievement(Achievement achievement)
    {
        string message = m_Player.GetIdentity().GetName() + " unlocked achievement: " + achievement.Name;

        array<Man> players = new array<Man>;
        GetGame().GetPlayers(players);

        foreach (Man man : players)
        {
            PlayerBase player = PlayerBase.Cast(man);
            if (player && player != m_Player)
            {
                NotificationSystem.Notify(player, message);
            }
        }
    }

    // Get achievement progress
    Achievement GetAchievement(string id)
    {
        return m_Achievements.Get(id);
    }

    // Save achievements
    void Save()
    {
        array<string> saveData = new array<string>;

        foreach (string id, Achievement achievement : m_Achievements)
        {
            saveData.Insert(achievement.Serialize());
        }

        // Save to file
        string steam64ID = m_Player.GetIdentity().GetId();
        string filePath = "$profile:Achievements_" + steam64ID + ".json";

        JsonSaveManager json = new JsonSaveManager();
        json.SaveToFile(filePath, saveData);
    }

    // Load achievements
    void Load()
    {
        string steam64ID = m_Player.GetIdentity().GetId();
        string filePath = "$profile:Achievements_" + steam64ID + ".json";

        if (!FileExist(filePath))
            return;

        JsonSaveManager json = new JsonSaveManager();
        array<string> saveData;

        json.LoadFromFile(filePath, saveData);

        // Restore progress
        foreach (string data : saveData)
        {
            Achievement achievement = Achievement.Deserialize(data);

            if (achievement && m_Achievements.Contains(achievement.ID))
            {
                m_Achievements.Get(achievement.ID).Current = achievement.Current;
            }
        }
    }

    // Get all achievements
    map<string, ref Achievement> GetAchievements()
    {
        return m_Achievements;
    }
}
```

### 3. AchievementRegistry.c

Registry of all achievements:

```c
class AchievementRegistry
{
    protected static ref map<string, ref Achievement> m_Registry;

    static void RegisterAchievement(Achievement achievement)
    {
        if (!m_Registry)
            m_Registry = new map<string, ref Achievement>;

        m_Registry.Set(achievement.ID, achievement);
    }

    static Achievement Get(string id)
    {
        if (m_Registry && m_Registry.Contains(id))
            return m_Registry.Get(id);

        return null;
    }

    static map<string, ref Achievement> GetAll()
    {
        return m_Registry;
    }
}
```

### 4. PlayerBase Integration

```c
modded class PlayerBase
{
    protected ref AchievementManager m_AchievementManager;

    override void OnConnect()
    {
        super.OnConnect();

        // Create achievement manager
        m_AchievementManager = new AchievementManager(this);

        // Load saved progress
        m_AchievementManager.Load();

        NotificationSystem.Notify(this, "Welcome! Type /achievements to view progress");
    }

    override void OnDisconnect()
    {
        super.OnDisconnect();

        // Save achievements
        if (m_AchievementManager)
        {
            m_AchievementManager.Save();
        }
    }

    // Zombie killed
    override void EEKilled(Object killer)
    {
        super.EEKilled(killer);

        // Check if killed by player
        PlayerBase player = PlayerBase.Cast(killer);
        if (player)
        {
            // Add achievement progress
            player.m_AchievementManager.AddProgress("first_blood", 1);
            player.m_AchievementManager.AddProgress("zombie_hunter", 1);
        }
    }

    // Get achievement manager
    AchievementManager GetAchievementManager()
    {
        return m_AchievementManager;
    }

    // Chat command: /achievements
    bool ChatCommand_Achievements()
    {
        array<string> messages = new array<string>;
        messages.Insert("=== Achievements ===");

        map<string, ref Achievement> achievements = m_AchievementManager.GetAchievements();

        foreach (string id, Achievement achievement : achievements)
        {
            float progress = achievement.GetProgress();
            string status = achievement.IsComplete() ? "[COMPLETE]" : "[" + progress + "%]";

            messages.Insert(status + " " + achievement.Name + ": " + achievement.Description);
        }

        // Show all messages
        foreach (string msg : messages)
        {
            NotificationSystem.Notify(this, msg);
        }

        return true;
    }
}
```

---

## 📝 Usage Examples

### Example 1: Time-Based Achievement

```c
modded class PlayerBase
{
    protected float m_ConnectTime;

    override void OnConnect()
    {
        super.OnConnect();
        m_ConnectTime = GetGame().GetTickTime();
    }

    override void OnUpdate(float timeslice)
    {
        super.OnUpdate(timeslice);

        // Check survival time (every 60 seconds)
        if (GetGame().GetTickTime() - m_ConnectTime >= 60.0)
        {
            int secondsSurvived = GetGame().GetTickTime() - m_ConnectTime;

            // Add progress to survivor achievement
            m_AchievementManager.AddProgress("survivor", secondsSurvived);

            m_ConnectTime = GetGame().GetTickTime(); // Reset timer
        }
    }
}
```

### Example 2: Crafting Achievement

```c
class CraftAchievement extends ActionBase
{
    override void OnExecuteServer(ActionData action_data)
    {
        super.OnExecuteServer(action_data);

        PlayerBase player = action_data.m_Player;

        // Add progress to crafting achievement
        player.GetAchievementManager().AddProgress("master_chef", 1);
    }
}
```

### Example 3: Achievement Rewards

```c
class AchievementRewardManager
{
    static void GiveReward(PlayerBase player, Achievement achievement)
    {
        // Give XP
        player.GetSkillManager().AddXP(achievement.XPReward);

        // Special rewards
        if (achievement.ID == "zombie_hunter")
        {
            // Give special weapon
            EntityAI weapon = GetGame().CreateObject("GoldenAK74", player.GetPosition());
            player.GetInventory().AddInInventory(weapon);

            NotificationSystem.Notify(player, "Reward: Golden AK74!");
        }

        if (achievement.ID == "survivor")
        {
            // Give food
            for (int i = 0; i < 10; i++)
            {
                EntityAI food = GetGame().CreateObject("BakedBeans_Can", player.GetPosition());
                player.GetInventory().AddInInventory(food);
            }

            NotificationSystem.Notify(player, "Reward: 10 Canned Beans!");
        }
    }
}
```

### Example 4: Achievement Leaderboard

```c
class AchievementLeaderboard
{
    static ref map<string, int> m_CompletedAchievements;

    static void UpdateLeaderboard(string playerName, int achievementCount)
    {
        if (!m_CompletedAchievements)
            m_CompletedAchievements = new map<string, int>;

        m_CompletedAchievements.Set(playerName, achievementCount);

        // Sort and display
        DisplayLeaderboard();
    }

    static void DisplayLeaderboard()
    {
        array<string> players = new array<string>;
        array<int> counts = new array<int>;

        foreach (string player, int count : m_CompletedAchievements)
        {
            players.Insert(player);
            counts.Insert(count);
        }

        // Sort (bubble sort)
        for (int i = 0; i < counts.Count() - 1; i++)
        {
            for (int j = 0; j < counts.Count() - i - 1; j++)
            {
                if (counts[j] < counts[j + 1])
                {
                    // Swap
                    int tempCount = counts[j];
                    counts[j] = counts[j + 1];
                    counts[j + 1] = tempCount;

                    string tempPlayer = players[j];
                    players[j] = players[j + 1];
                    players[j + 1] = tempPlayer;
                }
            }
        }

        // Display top 10
        Print("=== Achievement Leaderboard ===");
        for (int k = 0; k < 10 && k < players.Count(); k++)
        {
            Print((k + 1) + ". " + players[k] + " - " + counts[k] + " achievements");
        }
    }
}
```

---

## 🔗 Related Examples

→ [Notifications](../notifications/) - Individual notification mechanics
→ [XP System](../xp_system/) - Individual XP mechanics
→ [Integration: Faction+Teleport](../faction_teleport_integration/) - Another integration

---

## ⚠️ Important Notes

### Achievement Types
- **Kill-based:** Zombie kills, player kills
- **Time-based:** Survival time, playtime
- **Action-based:** Crafting, building, cooking
- **Collection-based:** Items collected

### XP Rewards
- **Easy:** 50-200 XP
- **Medium:** 200-500 XP
- **Hard:** 500-1000 XP
- **Legendary:** 1000+ XP

### Notifications
- **Progress:** Updated on progress gain
- **Complete:** Shown when unlocked
- **Broadcast:** Server-wide announcement

---

*Last updated: 2025-01-18*
*Combines: notifications + xp_system*
