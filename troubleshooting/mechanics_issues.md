# Troubleshooting - Mechanics Issues

**Purpose:** Solutions for common problems with specific DayZ modding mechanics
**For:** AI to quickly find solutions when something doesn't work
**Last Updated:** 2025-01-18 (Phase 7)

---

## ⚡ 30-Second Summary

Most DayZ modding problems fall into these categories:
- **RPC not working** - Wrong execution type or not registered
- **Data not saving** - Wrong file path or no permissions
- **Items not spawning** - Wrong position or invalid type
- **Performance issues** - Updating too frequently or not cleaning up

---

## 🔌 RPC ISSUES

### Problem: RPC Function Not Called

**Symptoms:**
- RPC function never executes
- No error message
- Nothing happens when called

**Solutions:**

1. **Check registration:**
```c
// ❌ WRONG: Not registered
void MyRPC() { }

// ✅ CORRECT: Registered in Init()
void InitRPC()
{
    GetRPCManager().AddRPC("MyMod", "MyRPC", this, SingleplayerExecutionType.Server);
}

void MyRPC(PlayerIdentity sender, Object target, ParamsReadContext ctx)
{
    Print("RPC executed!");
}
```

2. **Check execution type:**
```c
// ❌ WRONG: Server RPC on client
GetRPCManager().AddRPC("MyMod", "MyRPC", this, SingleplayerExecutionType.Server);
// Called on client - won't execute!

// ✅ CORRECT: Match execution type
// If on server: SingleplayerExecutionType.Server
// If on client: SingleplayerExecutionType.Client
```

3. **Check function signature:**
```c
// ❌ WRONG: Wrong signature
void MyRPC(string data) { }

// ✅ CORRECT: Must have these exact parameters
void MyRPC(PlayerIdentity sender, Object target, ParamsReadContext ctx)
{
    string data;
    if (!ctx.Read(data)) return;
}
```

---

### Problem: RPC Parameters Not Received

**Symptoms:**
- RPC executes but parameters are empty/wrong
- `ctx.Read()` returns false

**Solutions:**

1. **Check parameter count:**
```c
// ❌ WRONG: Sending 2 params, reading 3
GetRPCManager().SendRPC("MyMod", "MyFunc", new Param2<int, string>(100, "text"), true);
void MyFunc(PlayerIdentity sender, Object target, ParamsReadContext ctx)
{
    int val1;
    string val2;
    float val3;  // ❌ Too many reads!

    if (!ctx.Read(val1)) return;
    if (!ctx.Read(val2)) return;
    if (!ctx.Read(val3)) return;  // Will fail here
}

// ✅ CORRECT: Match parameter count
void MyFunc(PlayerIdentity sender, Object target, ParamsReadContext ctx)
{
    int val1;
    string val2;

    if (!ctx.Read(val1)) return;
    if (!ctx.Read(val2)) return;
}
```

2. **Check parameter types:**
```c
// ❌ WRONG: Type mismatch
GetRPCManager().SendRPC("MyMod", "MyFunc", new Param1<int>(100), true);
void MyFunc(PlayerIdentity sender, Object target, ParamsReadContext ctx)
{
    string value;  // ❌ Expecting string!
    if (!ctx.Read(value)) return;
}

// ✅ CORRECT: Match types
void MyFunc(PlayerIdentity sender, Object target, ParamsReadContext ctx)
{
    int value;
    if (!ctx.Read(value)) return;
}
```

---

### Problem: Client RPC Not Receiving

**Symptoms:**
- Server sends RPC but client doesn't receive
- Works on server but not on clients

**Solutions:**

1. **Check target parameter:**
```c
// ❌ WRONG: No target (broadcast only)
GetRPCManager().SendRPC("MyMod", "ClientFunc", params, true);

// ✅ CORRECT: Specify player identity
GetRPCManager().SendRPC("MyMod", "ClientFunc", params, true, player.GetIdentity());
```

2. **Check if client has function:**
```c
// Client must have registered the same RPC
void InitRPC()
{
    GetRPCManager().AddRPC("MyMod", "ClientFunc", this, SingleplayerExecutionType.Client);
}
```

---

## 💾 STORAGE ISSUES

### Problem: Data Not Saving

**Symptoms:**
- JSON file not created
- Data lost after restart

**Solutions:**

1. **Check file path:**
```c
// ❌ WRONG: Wrong path format
SaveData("MyData.json");  // Saves in wrong location

// ✅ CORRECT: Use proper path prefix
SaveData("$profile:MyMod/MyData.json");
```

2. **Check directory exists:**
```c
void SaveData(string path)
{
    // Extract directory
    string directory = "$profile:MyMod/";

    // Create if doesn't exist
    if (!FileExist(directory))
    {
        if (!MakeDirectory(directory))
        {
            Print("[Error] Failed to create directory: " + directory);
            return;
        }
    }

    // Now save
    File file = OpenFile(path, FileMode.WRITE);
    // ... save code
}
```

3. **Check file close:**
```c
// ❌ WRONG: Forgot to close
File file = OpenFile(path, FileMode.WRITE);
file.WriteString(data);
// File not closed! Data lost!

// ✅ CORRECT: Always close
File file = OpenFile(path, FileMode.WRITE);
if (file)
{
    file.WriteString(data);
    file.Close();  // Always close!
}
```

---

### Problem: JSON Parse Error

**Symptoms:**
- `JsonDeserialize()` fails
- Data corrupted after load

**Solutions:**

1. **Validate before parsing:**
```c
void LoadData(string path)
{
    if (!FileExist(path))
    {
        Print("[Error] File not found: " + path);
        return;
    }

    File file = OpenFile(path, FileMode.READ);
    string json;

    if (file)
    {
        file.ReadString(json);
        file.Close();
    }

    // Check if empty
    if (json.IsEmpty() || json == "")
    {
        Print("[Error] File is empty: " + path);
        return;
    }

    // Now parse
    map<string, int> data;
    if (json.Deserialize(data))
    {
        Print("[Error] Failed to parse JSON: " + path);
        return;
    }
}
```

2. **Check type matching:**
```c
// ❌ WRONG: Type mismatch
map<string, int> data = (map<string, int>)JsonDeserialize(json, map<string, int>);

// ✅ CORRECT: Match original type
// When saving:
map<string, int> originalData;
string json = JsonSerializeToString(originalData);

// When loading:
map<string, int> loadedData = (map<string, int>)JsonDeserialize(json, map<string, int>);
```

---

## 🎒 ITEM SPAWNING ISSUES

### Problem: Item Not Spawning

**Symptoms:**
- `CreateObject()` returns NULL
- Item doesn't appear

**Solutions:**

1. **Check item type name:**
```c
// ❌ WRONG: Wrong class name
EntityAI item = GetGame().CreateObject("AK74", pos);  // Wrong!

// ✅ CORRECT: Use full class path
EntityAI item = GetGame().CreateObject("AK74", pos);

// ✅ CORRECT: Check config.cpp for correct name
// Check: class AK74 extends RifleBolt {} in config
```

2. **Check position validity:**
```c
// ❌ WRONG: Invalid position (underground)
vector pos = Vector(5000, -100, 5000);  // Underground!

// ✅ CORRECT: Use surface height
float x = 5000;
float z = 5000;
float y = GetGame().SurfaceY(x, z);
vector pos = Vector(x, y, z);
```

3. **Check if spawning on server:**
```c
// ❌ WRONG: Spawning on client
EntityAI item = GetGame().CreateObject("AK74", pos);  // Client spawn!

// ✅ CORRECT: Spawn on server only
if (GetGame().IsServer())
{
    EntityAI item = GetGame().CreateObject("AK74", pos);
}
```

---

### Problem: Item Disappears Immediately

**Symptoms:**
- Item spawns then disappears
- No error message

**Solutions:**

1. **Check if in valid storage:**
```c
// ❌ WRONG: Spawned in invalid location
EntityAI item = GetGame().CreateObject("AK74", pos);
// Item falls through world!

// ✅ CORRECT: Spawn on surface
vector spawnPos = GetGame().SurfaceY(x,);
EntityAI item = GetGame().CreateObject("AK74", spawnPos);
```

2. **Check if despawn timer:**
```c
// ❌ WRONG: Auto-despawn enabled
EntityAI item = GetGame().CreateObject("AK74", pos);
item.SetLifetime(60);  // Deleted after 60 seconds!

// ✅ CORRECT: Set proper lifetime
EntityAI item = GetGame().CreateObject("AK74", pos);
item.SetLifetime(7200);  // 2 hours (or use 0 for permanent)
```

---

## ⏱️ PERFORMANCE ISSUES

### Problem: Server Lagging

**Symptoms:**
- Low FPS
- High CPU usage
- Players complain about lag

**Solutions:**

1. **Don't update every frame:**
```c
// ❌ WRONG: Updates every frame
override void EOnFrame(IEntity other, float timeSlice)
{
    SaveData();  // Every frame! LAG!
}

// ✅ CORRECT: Update periodically
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

2. **Limit entity count:**
```c
// ❌ WRONG: Unlimited spawns
for (int i = 0; i < 999999; i++)
{
    SpawnZombie();  // Too many!
}

// ✅ CORRECT: Limit spawns
const int MAX_ZOMBIES = 100;
int currentZombies = GetZombieCount();

if (currentZombies < MAX_ZOMBIES)
{
    SpawnZombie();
}
```

3. **Batch RPC calls:**
```c
// ❌ WRONG: 1000 RPC calls
for (int i = 0; i < 1000; i++)
{
    GetRPCManager().SendRPC("MyMod", "Update", new Param1<int>(i), true);
}

// ✅ CORRECT: Single batch RPC
array<int> data = new array<int>;
for (int i = 0; i < 1000; i++)
{
    data.Insert(i);
}
GetRPCManager().SendRPC("MyMod", "UpdateBatch", new Param1<array<int>>(data), true);
```

---

### Problem: Memory Leak

**Symptoms:**
- Memory usage increases over time
- Eventually crashes server

**Solutions:**

1. **Clean up timers:**
```c
// ❌ WRONG: Timer never cleaned
class MyManager
{
    void StartTimer()
    {
        GetGame().GetCallQueue().CallLater(Update, 1000);  // Never stops!
    }

    void Update()
    {
        // Do something
        GetGame().GetCallQueue().CallLater(Update, 1000);  // Infinite!
    }
}

// ✅ CORRECT: Control timer lifecycle
class MyManager
{
    protected bool m_Running;

    void StartTimer()
    {
        m_Running = true;
        GetGame().GetCallQueue().CallLater(Update, 1000);
    }

    void Update()
    {
        if (!m_Running) return;

        // Do something

        if (m_Running)
            GetGame().GetCallQueue().CallLater(Update, 1000);
    }

    void StopTimer()
    {
        m_Running = false;
    }
}
```

2. **Clear arrays periodically:**
```c
// ❌ WRONG: Array never cleared
array<EntityAI> m_Entities;

override void EOnFrame(IEntity other, float timeSlice)
{
    EntityAI entity = GetGame().CreateObject("Zombie", pos);
    m_Entities.Insert(entity);  // Grows forever!
}

// ✅ CORRECT: Clean up periodically
array<EntityAI> m_Entities;
protected float m_LastCleanup;

override void EOnFrame(IEntity other, float timeSlice)
{
    // Add entities
    EntityAI entity = GetGame().CreateObject("Zombie", pos);
    m_Entities.Insert(entity);

    // Cleanup every 60 seconds
    float now = GetGame().GetTickTime();
    if (now - m_LastCleanup > 60)
    {
        // Remove dead entities
        for (int i = m_Entities.Count() - 1; i >= 0; i--)
        {
            if (!m_Entities[i] || !m_Entities[i].IsAlive())
            {
                m_Entities.Remove(i);
            }
        }

        m_LastCleanup = now;
    }
}
```

---

## 🎯 XP SYSTEM ISSUES

### Problem: XP Not Saving

**Symptoms:**
- XP resets after restart
- Progress lost

**Solutions:**

1. **Check file path:**
```c
// ❌ WRONG: No path prefix
SaveData("xp.json");  // Where does it save?

// ✅ CORRECT: Use profile path
SaveData("$profile:MyMod/xp.json");
```

2. **Check if server:**
```c
// ❌ WRONG: Save on client
void AddXP(int amount)
{
    m_XP += amount;
    SaveData();  // Client save!
}

// ✅ CORRECT: Save on server
void Server_AddXP(PlayerIdentity sender, int amount)
{
    if (!GetGame().IsServer()) return;

    m_XP[sender.GetId()] += amount;
    SaveData();  // Server save
}
```

3. **Check save timing:**
```c
// ❌ WRONG: Save every frame (slow!)
void AddXP(int amount)
{
    m_XP += amount;
    SaveData();  // Every frame = LAG!
}

// ✅ CORRECT: Save periodically
protected float m_LastSave;

void AddXP(int amount)
{
    m_XP += amount;

    float now = GetGame().GetTickTime();
    if (now - m_LastSave > 60)  // Every 60 seconds
    {
        SaveData();
        m_LastSave = now;
    }
}
```

---

### Problem: Level Up Not Working

**Symptoms:**
- XP increases but no level up
- Level up happens at wrong time

**Solutions:**

1. **Check level calculation:**
```c
// ❌ WRONG: Off-by-one error
int level = m_XP / 1000;  // Level 0 at 0-999 XP!

// ✅ CORRECT: Start at level 1
int level = (m_XP / 1000) + 1;
```

2. **Check threshold:**
```c
// ❌ WRONG: Wrong threshold
if (m_XP >= 100)  // Level 2 at 100 XP?

// ✅ CORRECT: Use proper XP curve
int GetLevelForXP(int xp)
{
    // Level 1: 0 XP
    // Level 2: 1000 XP
    // Level 3: 3000 XP
    // Level 4: 6000 XP
    // etc.

    if (xp < 1000) return 1;
    if (xp < 3000) return 2;
    if (xp < 6000) return 3;
    return 4;
}
```

---

## 🚨 TELEPORTATION ISSUES

### Problem: Player Gets Stuck

**Symptoms:**
- Teleported inside wall/object
- Can't move after teleport

**Solutions:**

1. **Check surface height:**
```c
// ❌ WRONG: Fixed Y coordinate
vector pos = Vector(5000, 0, 5000);  // Might be underground!

// ✅ CORRECT: Use surface
float x = 5000;
float z = 5000;
float y = GetGame().SurfaceY(x, z);
vector pos = Vector(x, y, z);
```

2. **Add safety margin:**
```c
// ✅ GOOD: Spawn slightly above surface
float x = 5000;
float z = 5000;
float y = GetGame().SurfaceY(x, z) + 0.5;  // 50cm above
vector pos = Vector(x, y, z);
```

3. **Check for collisions:**
```c
// ✅ BEST: Check position before teleport
vector pos = Vector(5000, 0, 5000);
float y = GetGame().SurfaceY(x, z) + 1.0;
pos[1] = y;

// Check if safe
if (IsSafePosition(pos))
{
    player.SetPosition(pos);
}
```

---

## 🧟 ZOMBIE/NPC ISSUES

### Problem: NPC Not Spawning

**Symptoms:**
- `CreateObject()` returns NULL
- No zombies appear

**Solutions:**

1. **Check spawn limits:**
```c
// ❌ WRONG: Exceeded limit
// Server has reached max entity count!
for (int i = 0; i < 1000; i++)
{
    SpawnZombie();  // Will fail after limit!
}

// ✅ CORRECT: Check before spawning
int currentZombies = GetZombieCount();
const int MAX_ZOMBIES = 500;

if (currentZombies < MAX_ZOMBIES)
{
    SpawnZombie();
}
else
{
    Print("[Spawn] Zombie limit reached!");
}
```

2. **Check zombie type:**
```c
// ❌ WRONG: Wrong class name
EntityAI zombie = GetGame().CreateObject("Zombie", pos);

// ✅ CORRECT: Use correct type
EntityAI zombie = GetGame().CreateObject("ZombieMale", pos);
// or "ZombieFemale", "ZombieBoater", etc.
```

---

## 🔧 UI ISSUES

### Problem: UI Not Showing

**Symptoms:**
- Menu doesn't appear
- No error message

**Solutions:**

1. **Check menu registration:**
```c
// ❌ WRONG: Not registered in config.cpp
// config.cpp:
// class MyMenu {}
//
// In script:
GetGame().GetUIManager().ShowScriptedMenu(m_MyMenu, NULL);

// ✅ CORRECT: Register in config.cpp
// config.cpp:
// class MyMenu: UIScriptedMenu { }
//
// In script:
GetGame().GetUIManager().ShowScriptedMenu(m_MyMenu, NULL);
```

2. **Check widget references:**
```c
// ❌ WRONG: Wrong widget name
TextWidget text = TextWidget.Cast(m_RootWidget.FindAnyWidget("Title"));  // Wrong name!

// ✅ CORRECT: Check layout file
// Layout file says: <Text name="TitleText" />
TextWidget text = TextWidget.Cast(m_RootWidget.FindAnyWidget("TitleText"));
```

3. **Check null checks:**
```c
// ❌ WRONG: No null check
TextWidget text = TextWidget.Cast(m_RootWidget.FindAnyWidget("TitleText"));
text.SetText("Hello");  // Might crash if NULL!

// ✅ CORRECT: Always check null
TextWidget text = TextWidget.Cast(m_RootWidget.FindAnyWidget("TitleText"));
if (text)
{
    text.SetText("Hello");
}
```

---

## 📊 QUICK DIAGNOSTICS

### Enable Script Logging
```c
// In config.cpp
debugMonitor = 1;  // Enable script debug

// In script
Print("[DEBUG] My message");  // Shows in .rpt file
```

### Check Server Logs
```
Location: C:\Servers\DayZServer\profile\script.log
Look for: [ERROR], [WARNING], or your custom tags
```

### Enable Verbose Logging
```c
// In config.cpp
scriptDebug = 1;  // Enable script debugging
```

---

**Last Updated:** 2025-01-18
**Status:** ✅ Phase 7 (Part 3/5)
**Next:** troubleshooting/debug_checklist.md
