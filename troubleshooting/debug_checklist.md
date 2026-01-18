# Debug Checklist - DayZ Modding

**Purpose:** Step-by-step debugging checklist for DayZ modding problems
**For:** AI to systematically diagnose issues
**Last Updated:** 2025-01-18 (Phase 7)

---

## ⚡ QUICK DIAGNOSTICS FLOW

```
1. Check Console Logs → 2. Check File Permissions → 3. Check Code Syntax → 4. Check Runtime → 5. Check Network
```

---

## 📋 GENERAL CHECKLIST

### Step 1: Verify Basic Setup ✅

- [ ] **Mod loaded in config.cpp?**
  ```c
  // config.cpp should have:
  class MyMod
  {
      // Your mod code
  }
  ```

- [ ] **.pbo built and packed?**
  - Check AddonBuilder/DZBuilder output
  - Check @MyMod folder contains .pbo

- [ ] **Server loads @MyMod?**
  - Check server startup parameters
  - Check for errors in server log

- [ ] **script.log exists?**
  - Check: `C:\Users\[Username]\Documents\DayZ\script.log`
  - Look for your mod's class name

---

## 🔌 RPC DEBUG CHECKLIST

### Problem: RPC Not Executing

#### Step 1: Check Registration ✅
- [ ] **RPC registered in Init()?**
  ```c
  void InitRPC()
  {
      GetRPCManager().AddRPC("MyMod", "MyRPC", this, SingleplayerExecutionType.Server);
  }
  ```

- [ ] **InitRPC() called somewhere?**
  ```c
  void Init()
  {
      InitRPC();  // Don't forget!
  }
  ```

#### Step 2: Check Function Signature ✅
- [ ] **Has correct parameters?**
  ```c
  // MUST have exactly these:
  void MyRPC(PlayerIdentity sender, Object target, ParamsReadContext ctx)
  ```

- [ ] **All parameters read with error checking?**
  ```c
  int value1;
  string value2;
  if (!ctx.Read(value1)) return;  // Always check!
  if (!ctx.Read(value2)) return;
  ```

#### Step 3: Check Execution Type ✅
- [ ] **Execution type matches side?**
  ```c
  // On server: SingleplayerExecutionType.Server
  // On client: SingleplayerExecutionType.Client
  ```

- [ ] **RPC called from correct side?**
  - Server RPC → Called from server
  - Client RPC → Called from server (to client)

#### Step 4: Check Target (Client RPC) ✅
- [ ] **Target specified?**
  ```c
  // For client RPC, MUST specify player identity:
  GetRPCManager().SendRPC("MyMod", "ClientFunc", params, true, player.GetIdentity());
  ```

#### Step 5: Check Mod Name ✅
- [ ] **Mod names match exactly?**
  ```c
  // Registration:
  GetRPCManager().AddRPC("MyMod", "MyRPC", ...);

  // Send:
  GetRPCManager().SendRPC("MyMod", "MyRPC", ...);
  // Case-sensitive!
  ```

#### Step 6: Add Debug Prints ✅
- [ ] **Debug prints added?**
  ```c
  void MyRPC(PlayerIdentity sender, Object target, ParamsReadContext ctx)
  {
      Print("[RPC] MyRPC called by " + sender.GetId());

      string data;
      if (!ctx.Read(data))
      {
          Print("[RPC] ERROR: Failed to read parameter!");
          return;
      }

      Print("[RPC] Data: " + data);
  }
  ```

---

## 💾 STORAGE DEBUG CHECKLIST

### Problem: Data Not Saving

#### Step 1: Check File Path ✅
- [ ] **Path uses correct prefix?**
  ```c
  // ✅ CORRECT:
  "$profile:MyMod/data.json"

  // ❌ WRONG:
  "data.json"           // No prefix
  "C:/data.json"      // Absolute path
  ```

- [ ] **Directory created?**
  ```c
  string dir = "$profile:MyMod/";
  if (!FileExist(dir))
  {
      MakeDirectory(dir);
  }
  ```

#### Step 2: Check File Operations ✅
- [ ] **File closed after writing?**
  ```c
  File file = OpenFile(path, FileMode.WRITE);
  if (file)
  {
      file.WriteString(data);
      file.Close();  // CRITICAL!
  }
  ```

- [ ] **Error checking present?**
  ```c
  File file = OpenFile(path, FileMode.WRITE);
  if (!file)
  {
      Print("[Storage] ERROR: Cannot open file: " + path);
      return;
  }
  ```

#### Step 3: Check JSON ✅
- [ ] **JSON valid before saving?**
  ```c
  string json = JsonSerializeToString(data);

  // Validate
  if (json.IsEmpty() || json == "")
  {
      Print("[Storage] ERROR: Empty JSON!");
      return;
  }
  ```

- [ ] **Types match save/load?**
  ```c
  // Saving:
  map<string, int> data;
  string json = JsonSerializeToString(data);

  // Loading:
  map<string, int> loaded = (map<string, int>)JsonDeserialize(json, map<string, int>);
  // Types must match exactly!
  ```

#### Step 4: Check Save Timing ✅
- [ ] **Save function called?**
  ```c
  void SaveData()
  {
      Print("[Storage] Saving...");

      // ... save code

      Print("[Storage] Saved successfully!");
  }
  ```

- [ ] **Save on correct side?**
  ```c
  void SaveData()
  {
      if (!GetGame().IsServer())
      {
          Print("[Storage] Not on server, skipping save");
          return;
      }
      // ... save code
  }
  ```

#### Step 5: Check File Permissions ✅
- [ ] **Server has write access?**
  - Check folder permissions
  - Check disk space
  - Check antivirus blocking

---

## 🎒 ITEM SPAWNING CHECKLIST

### Problem: Items Not Spawning

#### Step 1: Check Item Type ✅
- [ ] **Exact class name from config.cpp?**
  ```c
  // Check config.cpp:
  // class AK74 extends RifleBolt {}

  // Use exact name:
  EntityAI item = GetGame().CreateObject("AK74", pos);
  ```

- [ ] **Item inheritance correct?**
  ```c
  // config.cpp:
  // class MyItem: ItemBase {}

  // script.c:
  class MyItem extends ItemBase  // Use 'extends', not ':'
  {
      // ...
  }
  ```

#### Step 2: Check Position ✅
- [ ] **Position uses surface height?**
  ```c
  float x = 5000;
  float z = 5000;
  float y = GetGame().SurfaceY(x, z);
  vector pos = Vector(x, y, z);
  ```

- [ ] **Safety margin added?**
  ```c
  float surfaceY = GetGame().SurfaceY(x, z);
  float y = surfaceY + 0.5;  // 50cm above surface
  vector pos = Vector(x, y, z);
  ```

- [ ] **Position within map bounds?**
  ```c
  if (pos[0] < 0 || pos[0] > 15000) return;  // X out of bounds
  if (pos[2] < 0 || pos[2] > 15000) return;  // Z out of bounds
  ```

#### Step 3: Check Server Authority ✅
- [ ] **Spawning on server only?**
  ```c
  if (!GetGame().IsServer())
  {
      Print("[Spawn] Not on server, skipping spawn");
      return;
  }

  EntityAI item = GetGame().CreateObject("AK74", pos);
  ```

#### Step 4: Check Entity Limits ✅
- [ ] **Entity count checked?**
  ```c
  int currentCount = GetEntityCount();
  const int MAX_ENTITIES = 1000;

  if (currentCount >= MAX_ENTITIES)
  {
      Print("[Spawn] Entity limit reached!");
      return;
  }
  ```

#### Step 5: Add Debug Logging ✅
- [ ] **Spawn result logged?**
  ```c
  EntityAI item = GetGame().CreateObject("AK74", pos);
  if (!item)
  {
      Print("[Spawn] ERROR: Failed to spawn AK74 at " + pos);
  }
  else
  {
      Print("[Spawn] Successfully spawned AK74 at " + pos);
  }
  ```

---

## ⚡ PERFORMANCE DEBUG CHECKLIST

### Problem: Server Lagging

#### Step 1: Check Update Frequency ✅
- [ ] **Updates use timer?**
  ```c
  // ❌ WRONG: Updates every frame
  override void EOnFrame(IEntity other, float timeSlice)
  {
      DoExpensiveOperation();  // Every frame = LAG
  }

  // ✅ CORRECT: Use timer
  protected float m_LastUpdate;

  override void EOnFrame(IEntity other, float timeSlice)
  {
      float now = GetGame().GetTickTime();
      if (now - m_LastUpdate > 1.0)  // Once per second
      {
          DoExpensiveOperation();
          m_LastUpdate = now;
      }
  }
  ```

#### Step 2: Check Entity Count ✅
- [ ] **Entity count logged?**
  ```c
  array<Object> objects = new array<Object>;
  GetGame().GetObjects(objects);

  Print("[Perf] Entity count: " + objects.Count());
  // Should be < 1000 for good performance
  ```

- [ ] **Cleanup implemented?**
  ```c
  // Remove dead entities periodically
  for (int i = m_Entities.Count() - 1; i >= 0; i--)
  {
      if (!m_Entities[i] || !m_Entities[i].IsAlive())
      {
          m_Entities.Remove(i);
      }
  }
  ```

#### Step 3: Check RPC Traffic ✅
- [ ] **RPCs batched?**
  ```c
  // ✅ GOOD: Single batch RPC
  array<int> data;
  for (int i = 0; i < 1000; i++)
  {
      data.Insert(i);
  }
  GetRPCManager().SendRPC("MyMod", "UpdateBatch", new Param1<array<int>>(data), true);
  ```

#### Step 4: Check File I/O ✅
- [ ] **File I/O uses timer?**
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

## 🎮 UI DEBUG CHECKLIST

### Problem: UI Not Showing

#### Step 1: Check Menu Registration ✅
- [ ] **Menu class exists?**
  ```c
  // config.cpp:
  class MyMenu: UIScriptedMenu
  {
      // Menu code
  }
  ```

- [ ] **Menu opened correctly?**
  ```c
  GetGame().GetUIManager().ShowScriptedMenu(m_MyMenu, NULL);
  ```

#### Step 2: Check Widget References ✅
- [ ] **Widget names match layout file?**
  ```c
  // Layout file (.layout):
  // <Text name="TitleText" />

  // Code:
  TextWidget text = TextWidget.Cast(m_RootWidget.FindAnyWidget("TitleText"));
  ```

- [ ] **Widgets null-checked?**
  ```c
  TextWidget text = TextWidget.Cast(m_RootWidget.FindAnyWidget("TitleText"));
  if (text)
  {
      text.SetText("Hello");
  }
  ```

#### Step 3: Check Menu Lifecycle ✅
- [ ] **OnShow() called?**
  ```c
  override void OnShow()
  {
      super.OnShow();
      Print("[UI] Menu shown!");

      // Initialize widgets here
  }
  ```

- [ ] **Update() called?**
  ```c
  override void Update()
  {
      super.Update();
      // Update dynamic content
  }
  ```

---

## 🧪 GENERAL DEBUGGING TIPS

### Enable Debug Mode

**In config.cpp:**
```cpp
debugMonitor = 1;    // Enable script debug
scriptDebug = 1;    // Enable debug logging
```

### Use Print Statements

```c
// Add debug tags for easy searching
Print("[DEBUG] Variable value: " + value);
Print("[RPC] Function called");
Print("[ERROR] Something went wrong!");
```

### Check Log Files

**Server Logs:**
```
script.log  - Script errors
script.rpt   - Server RPT log
```

**Client Logs:**
```
script.log  - Script errors
script.rpt   - Client RPT log
```

### Common Error Patterns

**"Cannot expand macro"**
- Missing include file
- Wrong macro name

**"Class not found"**
- Class not in config.cpp
- Wrong inheritance

**"Script compile error"**
- Syntax error in .c file
- Missing semicolon
- Wrong inheritance

---

## ✅ FINAL VERIFICATION

### After Fixing Issues:

1. **Rebuild .pbo** - Repack mod after code changes
2. **Restart server** - Changes require server restart
3. **Clear cache** - Delete old .pbo and repack
4. **Test on clean install** - Verify from scratch
5. **Check logs again** - Ensure no new errors

---

## 🚀 QUICK REFERENCE

### Common File Paths
```
Server logs: C:\Servers\DayZServer\profile\script.log
Client logs: C:\Users\[Username]\Documents\DayZ\script.log
Server profile: C:\Servers\DayZServer\profile\
```

### Time Values
```
0.016 second  ≈ 1 frame (60 FPS)
1.0 second   = 60 frames
60.0 seconds = 1 minute
```

### Health Values
```
0.0 = Dead
1.0 = Full health
```

---

**Last Updated:** 2025-01-18
**Status:** ✅ Phase 7 (Part 4/5)
**Next:** Expand AI_FAQ.md
