# Deployment Guide

**Status:** ✅ Complete Guide
**Target:** Mod developers
**Goal:** Package and publish DayZ mods

---

## ⚡ 30-Second Summary

Complete guide to packaging, testing, and publishing DayZ mods. Covers PBO building, Workshop upload, server setup, and version management.

---

## 📦 PBO Building

### What is a PBO?

PBO (Packed Binary Object) is DayZ's mod format. All scripts must be compiled into PBO files.

---

### Option 1: ArmaToolKit (Recommended)

**Download:** [ArmaToolKit](https://developer.bi.bi/p对公司/support/downloads/tools)

#### Installation

1. Download and install ArmaToolKit
2. Open "AddonBuilder"
3. Configure settings

#### Configuration

```
Source folder: K:\Работа\Dayz_WorckPlace\MyMod\
Destination: K:\Работа\Dayz_WorckPlace\MyMod\@MyMod\
Addons folder: K:\Работа\Dayz_WorckPlace\MyMod\addons\
```

#### Build Process

1. Click "Pack"
2. Wait for compilation
3. PBO created in `@MyMod/` folder

---

### Option 2: DayZ Tools (Official)

**Download:** [DayZ Tools](https://store.steampowered.com/app/2261200/DayZ_Tools/)

#### Using Workbench

1. Open Workbench
2. File → Open Project → Select your mod
3. Build → Build Addon (Ctrl+B)
4. PBO created in `output/` folder

---

### Option 3: Command Line (Automation)

#### AddonBuilder CLI

```batch
@echo off
set SOURCE=K:\Работа\Dayz_WorckPlace\MyMod
set DEST=K:\Работа\Dayz_WorckPlace\MyMod\@MyMod

"C:\Program Files (x86)\Bohemia Interactive\Tools\AddonBuilder\AddonBuilder.exe" %SOURCE% %DEST% -packonly -clear

echo Build complete!
pause
```

#### HEMTT (Advanced)

[HEMTT](https://github.com/BrettMayson/HEMTT) is a build tool for DayZ/Arma mods.

**Install:**
```batch
pip install hemtt
```

**Build:**
```batch
hemtt build
```

**hemtt.toml:**
```toml
name = "MyMod"
prefix = "MyMod"

[version]
major = 1
minor = 0
patch = 0

[files]
include = [
    "mod.cpp",
    "logo.paa",
    "meta.cpp",
]
```

---

## 🧪 Testing

### Local Testing

#### Step 1: Create Test Server

1. Copy `@MyMod/` to DayZ server root
2. Edit `serverDZ.cfg`:
```cfg
// Add mod to load order
launcherMod=";
@MyMod;
"
```

#### Step 2: Start Server

```batch
cd "C:\Program Files (x86)\Steam\steamapps\common\DayZServer"
start DayZServer.exe -config=serverDZ.cfg -port=2302 -profiles=instance_2302 "-mod=@MyMod"
```

#### Step 3: Connect with Client

1. Launch DayZ
2. Play → Change Parameters → Mods
3. Load `@MyMod`
4. Connect to `127.0.0.1:2302`

---

### Common Testing Issues

#### Issue: Mod doesn't load

**Solution:**
1. Check `mod.cpp` exists in PBO root
2. Verify PBO structure: `@MyMod/addons/MyMod.pbo`
3. Check server log for errors

#### Issue: Script errors

**Solution:**
1. Check `script.log` in server profile
2. Verify script.compile log
3. Check for missing includes

#### Issue: Missing dependencies

**Solution:**
1. Check `requiredAddons` in config.cpp
2. Verify dependencies are loaded
3. Check load order

---

## 📤 Steam Workshop Upload

### Option 1: SteamCMD (Automated)

#### Install SteamCMD

```batch
# Download SteamCMD
curl -o steamcmd.zip https://steamcdn-a.akamaihd.net/client/installer/steamcmd.zip
unzip steamcmd.zip -d steamcmd

# Run once to update
steamcmd.exe +quit
```

#### Upload Script

```batch
@echo off
set VDF=upload_workshop.vdf

steamcmd.exe +login YOUR_USERNAME +workshop_build_item 123456789 %VDF% +quit
```

#### upload_workshop.vdf

```vdf
"workshopitem"
{
    "appid" "221100"
    "contentfolder" "K:\Работа\Dayz_WorckPlace\MyMod\@MyMod"
    "changenote" "Update 1.0 - Initial release"
    "previewfile" "K:\Работа\Dayz_WorckPlace\MyMod\preview.jpg"
    "visibility" "public"
}
```

---

### Option 2: Steam Workshop Website

#### Step 1: Create Workshop Item

1. Go to [DayZ Workshop](https://steamcommunity.com/app/221100/workshop/)
2. Click "Upload Item"
3. Select "DayZ" as game

#### Step 2: Upload Mod

1. Upload `@MyMod.zip` (containing PBOs)
2. Add preview image (512x512 minimum)
3. Add description
4. Set visibility (Public/Friends/Private)
5. Click "Submit"

---

### Workshop Metadata

#### mod.cpp

```cpp
name = "My Mod";
logo = "mod.paa"; // 256x256 image
logoOver = "mod_over.paa"; // 512x512 image (hover)
logoSmall = "mod_small.paa"; // 128x128 image (tooltip)
tooltip = "My awesome mod description";
overview = "Full description of what the mod does...";
action = "https://discord.gg/myserver";
author = "YourName";
```

#### meta.cpp

```cpp
version = "1.0.0";
```

---

## 🖥️ Server Setup

### Server Configuration

#### serverDZ.cfg

```cfg
// General settings
hostname = "My DayZ Server";
password = "";
passwordAdmin = "adminpassword";
maxPlayers = 60;
verifySignatures = 2;
guaranteeUpdates = 1;
disableCrosshair = 0;

// Time
serverTime = "SystemTime";
serverTimeAcceleration = 1;
serverNightTimeAcceleration = 1;

// Mods
launcherMod="
@MyMod;
@AnotherMod;
"
```

---

### Mod Loading Order

**IMPORTANT:** Order matters!

```
1. Dependencies (DayZ, Expansion)
2. Frameworks (Community Framework)
3. Core mods (Your mod)
4. Addons (Your addons)
5. Overrides (Server overrides)
```

**Example:**
```
launcherMod="
@DayZ;
@CommunityFramework;
@MyMod_Core;
@MyMod_Items;
@MyMod_Server;
"
```

---

### Server Startup Script

**start_server.bat:**
```batch
@echo off
set DAYZ_SA="C:\Program Files (x86)\Steam\steamapps\common\DayZServer"
set PORT=2302
set CONFIG=serverDZ.cfg
set PROFILES=instance_%PORT%

start "" "%DAYZ_SA%\DayZServer.exe" -config=%CONFIG% -port=%PORT% -profiles=%PROFILES% "-mod=@MyMod" -dologs -adminlog -netlog

echo Server started on port %PORT%
pause
```

---

## 🔐 PBO Signing

### Why Sign PBOs?

Prevents players from cheating by modifying your mod.

### Create Keys

#### Using DayZ Tools

1. Open "DZLights"
2. Tools → Create Key
3. Select key name: `MyMod.bikey`
4. Save key

#### Copy Key to Server

```
Server Keys/
├── MyMod.bikey
└── AnotherMod.bikey
```

Location: `DayZServer/keys/`

---

### Sign PBOs

#### Using DZSign

```batch
DZSign.exe MyMod.pbo MyMod.bikey
```

#### Verify Signature

```batch
DZSign.exe -verify MyMod.pbo MyMod.bikey
```

---

## 📝 Version Management

### Semantic Versioning

Format: `MAJOR.MINOR.PATCH`

- **MAJOR:** Breaking changes (1.0.0 → 2.0.0)
- **MINOR:** New features (1.0.0 → 1.1.0)
- **PATCH:** Bug fixes (1.0.0 → 1.0.1)

**Examples:**
```
1.0.0 - Initial release
1.1.0 - Added crafting system
1.1.1 - Fixed crafting bug
2.0.0 - Complete refactor (breaking)
```

---

### Changelog Format

```markdown
# Changelog

## [1.1.0] - 2025-01-18

### Added
- Crafting system with 50 recipes
- GUI recipe menu
- Hot-reload for JSON configs

### Changed
- Improved XP system performance
- Updated faction balance

### Fixed
- Fixed storage corruption bug
- Fixed teleport cooldown issue

## [1.0.0] - 2025-01-10

### Added
- Initial release
- XP system
- Faction system
- Storage system
```

---

### Update Notifications

#### In-Game Update Check

```c
class UpdateChecker
{
    static void CheckForUpdates(PlayerBase player)
    {
        string currentVersion = "1.0.0";
        string latestVersion = GetLatestVersionFromServer();

        if (currentVersion != latestVersion)
        {
            NotificationSystem.Notify(player, "Update available: " + latestVersion);
            NotificationSystem.Notify(player, "Current: " + currentVersion);
        }
    }

    static string GetLatestVersionFromServer()
    {
        // HTTP request to your server
        // Or read from Steam Workshop metadata

        return "1.1.0"; // Example
    }
}
```

---

## 🌐 Community Publishing

### Where to Publish

1. **Steam Workshop** (Primary)
   - URL: https://steamcommunity.com/app/221100/workshop/
   - Reach: All DayZ players
   - Updates: Automatic

2. **GitHub** (Source code)
   - URL: https://github.com/
   - Purpose: Open source mods
   - Collaboration: Issues, pull requests

3. **Discord** (Community)
   - Purpose: Support, feedback
   - Features: Announcements, bug reports

4. **DayZ Forums** (Discussions)
   - URL: https://forums.dayz.com/
   - Purpose: Release announcements

---

### Promotion Tips

1. **Screenshots/Videos**
   - Show mod features
   - Demonstrate gameplay
   - Tutorial videos

2. **Documentation**
   - README with installation
   - Feature list
   - Known issues

3. **Community Engagement**
   - Respond to comments
   - Address bugs quickly
   - Listen to feedback

4. **Regular Updates**
   - Fix bugs promptly
   - Add requested features
   - Communicate changes

---

## 🔍 Troubleshooting

### Issue: Workshop Upload Fails

**Solutions:**
1. Check Steam login
2. Verify VDF file format
3. Check file permissions
4. Reduce file size (<100 MB recommended)

### Issue: Server Won't Start

**Solutions:**
1. Check mod load order
2. Verify all dependencies loaded
3. Check server log for errors
4. Verify PBO signatures

### Issue: Players Can't Connect

**Solutions:**
1. Verify mod is on Workshop
2. Check client has same mod version
3. Verify server mod signature
4. Check server port forwarding

---

## 📚 Additional Resources

### Tools

- [ArmaToolKit](https://developer.bi.bi/p对公司/support/downloads/tools)
- [DayZ Tools](https://store.steampowered.com/app/2261200/DayZ_Tools/)
- [HEMTT](https://github.com/BrettMayson/HEMTT)
- [PboManager](http://winse.byte.nl/files/pboManager/)

### Documentation

- [DayZ Modding Discord](https://discord.gg/dayz)
- [DayZ Wiki](https://dayz.fandom.com/wiki/DayZ_Wiki)
- [Bohemia Forums](https://forums.bohemia.net/forums/forum/154-dayz/)

### Communities

- [r/dayz](https://reddit.com/r/dayz)
- [DayZ Modding Group](https://steamcommunity.com/groups/dayzmodding)

---

## 🔗 Related Topics

→ [Config](../topics/02_config.md) - Mod configuration
→ [Troubleshooting](../troubleshooting/) - Common issues
→ [Performance](../reference/PERFORMANCE_GUIDE.md) - Optimization

---

*Last updated: 2025-01-18*
*Target: Mod developers*
