# Storage Systems - Complete Reference

**Source:** Vanilla DayZ + Custom Storage Mods
**Complexity:** Intermediate
**Related:** [Persistent Storage](PERSISTENT_STORAGE.md) | [Central Economy](CENTRAL_ECONOMY.md)

---

## ⚡ 30-Second Summary

Storage Systems in DayZ manage **data persistence** across server restarts. This includes player data, base storage, global databases, and cross-server synchronization.

---

## 🎯 Key Concepts

### Storage Types

**1. Player Storage**
- Character data (position, health, stats)
- Inventory items
- Custom data (XP, skills, quests)

**2. Base Storage**
- Tents, barrels, chests
- Building storage
- Vehicle cargo

**3. Global Storage**
- Server-wide databases
- Cross-server persistence
- Economy data

**4. Session Storage**
- Temporary data (lost on restart)
- Runtime state

---

## 🔧 Implementation Patterns

### Pattern 1: Vanilla Storage (GetGame().GetCallQueue())

**Source:** Vanilla DayZ

**Concept:** Use vanilla `GetCallQueue()` for persistence

**Example:**
```c
class VanillaStorage
{
    protected string m_PlayerID;
    protected PlayerBase m_Player;

    void SavePlayerData()
    {
        // Get vanilla save queue
        GetGame().GetCallQueue().CallLater(SaveData);
    }

    void SaveData()
    {
        // Vanilla handles saving automatically
        // Just update player variables
        m_Player.SetHealth(m_Player.GetHealth());
        // ... other data
    }
}
```

**Pros:**
- ✅ Built into DayZ
- ✅ Automatic saving
- ✅ No manual file I/O

**Cons:**
- ❌ Limited flexibility
- ❌ Can't save custom data easily
- ❌ Single-server only

---

### Pattern 2: JSON Storage (Custom)

**Source:** Custom mods (XP, Banking, etc.)

**Concept:** Save/load JSON files manually

**Example:**
```c
class JsonStorage
{
    protected string m_DataPath;

    void SaveData(map<string, int> data)
    {
        m_DataPath = "$profile:MyData/data.json";

        // Serialize to JSON
        string json = JsonSerializeToString(data);

        // Write to file
        File file = OpenFile(m_DataPath, FileMode.WRITE);
        if (file)
        {
            file.WriteString(json);
            file.Close();
        }
    }

    map<string, int> LoadData()
    {
        m_DataPath = "$profile:MyData/data.json";

        if (!FileExist(m_DataPath))
            return new map<string, int>;

        // Read from file
        File file = OpenFile(m_DataPath, FileMode.READ);
        string json;
        if (file)
        {
            file.ReadString(json);
            file.Close();
        }

        // Deserialize from JSON
        return (map<string, int>)JsonDeserialize(json, map<string, int>);
    }
}
```

**Pros:**
- ✅ Full control
- ✅ Can save any data
- ✅ Cross-server capable

**Cons:**
- ❌ Manual file I/O
- ❌ Must handle errors
- ❌ Can be slow for large data

---

### Pattern 3: CF ModStorage (Community Framework)

**Source:** Community Framework (1559212036)

**Concept:** Simplified JSON storage via CF

**Example:**
```c
class CFStorage
{
    void SaveData(string key, Class data)
    GetGame().GetStorage().SetSync(data, key);

    Class LoadData(string key, Class data)
    {
        GetGame().GetStorage().GetSync(data, key);
        return data;
    }
}
```

**Pros:**
- ✅ Simple API
- ✅ Automatic sync
- ✅ Cross-server support

**Cons:**
- ❌ Requires CF dependency
- ❌ Less control

---

## 📁 File Structure

### Recommended Storage Structure

```
$profile:/              ← Server profile folder
├── MyMod/
│   ├── players/         ← Player data
│   │   ├── player1.json
│   │   └── player2.json
│   ├── bases/           ← Base storage
│   │   ├── base1.json
│   │   └── base2.json
│   ├── global/          ← Global data
│   │   └── economy.json
│   └── config.json     ← Mod configuration
```

---

## 🔐 Data Structures

### Player Data Structure

```json
{
  "PlayerID": "steamid123",
  "PlayerName": "PlayerName",
  "Position": [5000, 0, 5000],
  "Health": 1.0,
  "Stats": {
    "Kills": 10,
    "Deaths": 5,
    "Playtime": 3600
  },
  "CustomData": {
    "Level": 5,
    "XP": 5000,
    "Faction": "Traders"
  },
  "Inventory": [
    {
      "Type": "AK74",
      "Quantity": 1,
      "Condition": 0.8
    }
  ]
}
```

---

### Base Storage Structure

```json
{
  "BaseID": "base123",
  "Owner": "player1",
  "Position": [6000, 0, 6000],
  "Items": [
    {
      "Type": "Tent",
      "Position": [0, 0, 0],
      "Inventory": [...]
    }
  ]
}
```

---

## ⚡ Performance Optimization

### Don't Save Every Frame

**❌ BAD:**
```c
override void EOnFrame(IEntity other, float timeSlice)
{
    SaveData();  // Saves every frame = LAG
}
```

**✅ GOOD:**
```c
protected float m_LastSave;

override void EOnFrame(IEntity other, float timeSlice)
{
    float now = GetGame().GetTickTime();

    // Save every 60 seconds
    if (now - m_LastSave > 60)
    {
        SaveData();
        m_LastSave = now;
    }
}
```

---

### Batch Saves

**Save multiple items at once:**
```c
class BatchStorage
{
    protected array<string> m_PendingSaves;

    void MarkForSave(string playerID)
    {
        if (!m_PendingSaves.Find(playerID))
            m_PendingSaves.Insert(playerID);
    }

    void FlushSaves()
    {
        foreach (string playerID : m_PendingSaves)
        {
            SavePlayer(playerID);
        }
        m_PendingSaves.Clear();
    }
}
```

---

### Use Asynchronous I/O

**Don't block main thread:**
```c
class AsyncStorage
{
    void SaveAsync(string playerID, PlayerData data)
    {
        // Save in background
        GetGame().GetCallQueue().CallLater(SaveData);
    }

    void SaveData()
    {
        // Actual save happens off main thread
        SaveToFile();
    }
}
```

---

## 🔒 Data Integrity

### Validation

**Validate loaded data:**
```c
bool ValidatePlayerData(PlayerData data)
{
    // Check required fields
    if (!data.PlayerID)
        return false;

    if (!data.PlayerName)
        return false;

    // Check ranges
    if (data.Health < 0 || data.Health > 1.0)
        return false;

    // Check array sizes
    if (data.Inventory.Count() > 100)
        return false;  // Too many items

    return true;
}
```

---

### Checksums

**Detect corruption:**
```c
string CalculateChecksum(string json)
{
    int hash = 0;
    foreach (char c : json)
    {
        hash += c;
    }
    return hash.ToString();
}

void SaveWithChecksum(string data)
{
    string checksum = CalculateChecksum(data);

    File file = OpenFile("data.json", FileMode.WRITE);
    file.WriteString(data);
    file.WriteString("\n// checksum: " + checksum);
    file.Close();
}

bool LoadWithChecksum()
{
    File file = OpenFile("data.json", FileMode.READ);
    string data;
    file.ReadString(data);
    file.Close();

    // Extract checksum
    int checksumLine = data.IndexOf("// checksum:");
    if (checksumLine == -1)
        return false;

    string savedChecksum = data.Substring(checksumLine + 13);
    string dataWithoutChecksum = data.Substring(0, checksumLine);

    // Verify
    string calculatedChecksum = CalculateChecksum(dataWithoutChecksum);
    return (savedChecksum == calculatedChecksum);
}
```

---

## 🌐 Cross-Server Storage

### Shared Database Folder

**Concept:** Multiple servers use same storage folder

**Setup:**
```c
class SharedStorage
{
    string GetSharedPath()
    {
        // Use shared network path
        return "Z:/SharedStorage/players/";
    }

    void SavePlayer(string playerID, PlayerData data)
    {
        string path = GetSharedPath() + playerID + ".json";
        // Save to shared path
    }
}
```

### Database Sync

**Sync data across servers:**
```c
class DatabaseSync
{
    void SyncPlayer(string playerID)
    {
        // Download from database
        PlayerData data = DownloadFromDB(playerID);

        // Load into game
        LoadPlayerData(data);
    }

    void UploadPlayer(string playerID)
    {
        // Get player data
        PlayerData data = GetPlayerData(playerID);

        // Upload to database
        UploadToDB(playerID, data);
    }
}
```

---

## 📝 Quick Reference

### Storage Types

| Type | Persistence | Complexity | Use Case |
|------|-------------|------------|----------|
| **Vanilla** | Auto | Low | Basic player data |
| **JSON** | Manual | Medium | Custom data |
| **CF ModStorage** | Auto | Low | CF mods |
| **Database** | Manual | High | Cross-server |

### Save Frequency

| Data Type | Frequency | Trigger |
|-----------|-----------|---------|
| **Player Position** | Every 60s | Timer |
| **Inventory** | On change | Event |
| **Base Storage** | Every 300s | Timer |
| **Economy** | Every transaction | Event |

---

## ⚠️ Common Mistakes

### ❌ Mistake 1: Saving Too Often

```c
// WRONG: Saves every frame
override void EOnFrame(IEntity other, float timeSlice)
{
    SaveData();
}

// CORRECT: Save periodically
override void EOnFrame(IEntity other, float timeSlice)
{
    if (GetGame().GetTickTime() - m_LastSave > 60)
        SaveData();
}
```

### ❌ Mistake 2: No Validation

```c
// WRONG: Load without validation
PlayerData data = LoadData();
m_Player.SetHealth(data.Health);  // Might be invalid!

// CORRECT: Validate first
PlayerData data = LoadData();
if (ValidatePlayerData(data))
    m_Player.SetHealth(data.Health);
```

---

## 🎓 Next Steps

**After learning storage:**
1. **Read:** [Persistent Storage](PERSISTENT_STORAGE.md) - Cross-server storage
2. **Read:** [CF ModStorage](CF_MODSTORAGE.md) - CF storage
3. **Example:** [examples/storage/](../examples/storage/) - Working storage system

---

## 💡 Key Takeaways

1. **Storage Types:**
   - Vanilla = Auto-save (limited)
   - JSON = Manual save (flexible)
   - CF = Simplified (easy)
   - Database = Cross-server (advanced)

2. **Best Practices:**
   - Don't save every frame
   - Validate loaded data
   - Use checksums
   - Batch saves

3. **Performance:**
   - Save periodically (60s+)
   - Batch multiple saves
   - Use async I/O

---

**Last Updated:** 2025-01-18
**Status:** ✅ Complete
