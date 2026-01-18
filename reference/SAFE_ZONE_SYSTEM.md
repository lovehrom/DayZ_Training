# Safe Zone System - Complete Reference

**Source:** TraderPlus Safe Zone (Mod ID: 2458896948)
**Complexity:** Intermediate

---

## ⚡ Summary

Safe Zones prevent PvP damage and combat in designated areas. Automatically disables weapons, removes zombies/animals, and protects players.

---

## 🎯 Key Features

- ✅ **No PvP** - Weapons disabled in zone
- ✅ **No Zombies** - Automatically cleaned up
- ✅ **No Animals** - Aggressive animals removed
- ✅ **Infection Protection** - Influenza paused
- ✅ **Restraint Removal** - Handcuffs removed
- ✅ **Visual Notification** - UI indicator when in zone

---

## 🔧 Implementation

### Safe Zone Location

```c
class SafeZoneLocation
{
    float X;        // Position X
    float Y;        // Position Z (Y in game coords)
    float Radius;   // Zone radius in meters
    string SafeZoneStatut;  // Zone name
}

// Example: Trader zone at 5000,0,5000 with 100m radius
SafeZoneLocation zone;
zone.X = 5000;
zone.Y = 5000;
zone.Radius = 100;
zone.SafeZoneStatut = "Trader Zone";
```

### Check Player in Zone

```c
void CheckIfInsideSafeZone(PlayerBase player)
{
    vector pos = player.GetPosition();

    foreach (SafeZoneLocation zone : m_SafeZones)
    {
        float distance = vector.Distance(pos, Vector(zone.X, 0, zone.Y));

        if (distance <= zone.Radius)
        {
            // Player entered zone
            if (!player.GetSafeZoneStatus())
            {
                player.SetSafeZoneStatus(true);
                player.SetSafeZoneName(zone.SafeZoneStatut);
                player.GetInputController().OverrideRaise(true, false);  // Lower weapon
                player.SetAllowDamage(false);  // God mode
                NotificationSystem.Notify(player, "Entered: " + zone.SafeZoneStatut);
            }
        }
        else
        {
            // Player left zone
            if (player.GetSafeZoneStatus())
            {
                player.SetSafeZoneStatus(false);
                player.SetSafeZoneName("");
                player.GetInputController().OverrideRaise(false, false);
                player.SetAllowDamage(true);
                NotificationSystem.Notify(player, "Left safe zone");
            }
        }
    }
}
```

### Cleanup Entities

```c
void EntitiesCleanUp(vector pos)
{
    array<Object> objs = new array<Object>;
    GetGame().GetObjectsAtPosition3D(pos, 20, objs, NULL);

    foreach (Object obj : objs)
    {
        // Delete zombies and animals (not whitelisted)
        if (!IsWhitelisted(obj) && (obj.IsKindOf("ZombieBase") || obj.IsKindOf("AnimalBase")))
        {
            GetGame().ObjectDelete(obj);
        }
    }
}
```

---

## 📝 Configuration

```json
{
    "SafeAreaLocation": [
        {
            "X": 5000,
            "Y": 5000,
            "Radius": 100,
            "SafeZoneStatut": "Trader Zone"
        }
    ],
    "ObjectsToDelete": [
        "ZombieBase",
        "AnimalBase"
    ],
    "WhitelistEntities": [
        "SurvivorBase",
        "CarScript"
    ],
    "CleanUpTimer": 60
}
```

---

**Last Updated:** 2025-01-18
**Status:** ✅ Complete
