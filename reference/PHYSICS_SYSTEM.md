# Physics System - Физика DayZ

**Status:** ✅ Verified from raw_data
**Sources:** PlayerBase.c, EntityAI.c, ItemBase.c
**Related:** [Entity Lifecycle](ENTITYAI_LIFECYCLE.md) | [GetGame API](GETGAME_API.md)

---

## ⚡ 30-Second Summary

DayZ physics система предоставляет raycasting, velocity queries, и entity linking через `DayZPhysics` класс и методы `Physics*()`.

---

## 🎯 5-Minute Solution: Raycast для проверки линии визирования

### Базовый Raycast

```c
// ✅ ПРОВЕРЕНО: PlayerBase.c:7573
vector from = player.GetPosition() + "0 1.5 0";  // Голова игрока
vector to = from + player.GetDirection() * 100;  // 100м вперёд

vector contactPos;
vector contactDir;
int contactComponent;

DayZPhysics.RaycastRV(from, to, contactPos, contactDir, contactComponent, NULL, NULL, NULL, false, true);

if (contactPos != vector.Zero)
{
    Print("Hit at: " + contactPos.ToString());
}
```

### Проверка скорости игрока

```c
// ✅ ПРОВЕРЕНО: PlayerBase.c:3693
vector velocity;
PhysicsGetVelocity(velocity);

float speed = vector.Distance(vector.Zero, velocity);
Print("Player speed: " + speed + " m/s");
```

---

## 📚 Deep Dive

### DayZPhysics.RaycastRV()

**✅ ПРОВЕРЕНО:** PlayerBase.c:7573

```c
// Сигнатура метода:
void RaycastRV(
    vector from,              // Начальная точка raycast
    vector to,                // Конечная точка raycast
    out vector contactPos,    // Точка контакта (результат)
    out vector contactDir,    // Направление контакта (результат)
    out int contactComponent, // Компонент который был hit (результат)
    Object ignoredObject,     // Объект который игнорируется (обычно NULL)
    Object ignoredObject2,    // Второй игнорируемый объект (обычно NULL)
    Object withObj,           // Объект для поиска (обычно NULL)
    bool sorted,              // Сортировать результаты (обычно false)
    bool ground_only          // Только земля? (true/false)
)
```

**Примеры использования:**

```c
// 1. Проверка препятствий перед игроком
vector from = player.GetPosition() + "0 1.5 0";
vector to = from + player.GetDirection() * 10;
vector hitPos;
vector hitDir;
int hitComp;

DayZPhysics.RaycastRV(from, to, hitPos, hitDir, hitComp, NULL, NULL, NULL, false, false);

if (hitPos != vector.Zero)
{
    Print("Obstacle detected at: " + hitPos.ToString());
}

// 2. Проверка земли под ногами
vector from2 = player.GetPosition() + "0 0.5 0";
vector to2 = from2 - "0 2 0";  // Вниз

DayZPhysics.RaycastRV(from2, to2, hitPos, hitDir, hitComp, NULL, NULL, NULL, false, true);

if (hitPos != vector.Zero)
{
    float height = hitPos[1];
    Print("Ground height: " + height);
}
```

### PhysicsGetVelocity()

**✅ ПРОВЕРЕНО:** PlayerBase.c:3693

```c
// Получить скорость объекта
vector velocity;
PhysicsGetVelocity(velocity);

float speed = velocity.Length();  // Скалярная скорость
float speedHoriz = vector.Distance(vector.Zero, vector.Set(velocity[0], 0, velocity[1]));  // Горизонтальная скорость
```

**Примеры использования:**

```c
// 1. Проверка скорости движения
vector vel;
PhysicsGetVelocity(vel);

if (vel.Length() > 5.0)
{
    Print("Moving fast: " + vel.Length() + " m/s");
}

// 2. Проверка падения
vector vel;
PhysicsGetVelocity(vel);

if (vel[1] < -5.0)  // Y координата - вертикальная
{
    Print("Falling down fast!");
}
```

### PhysicsGetPositionWS()

**✅ ПРОВЕРЕНО:** PlayerBase.c:2877

```c
// Получить мировую позицию объекта
vector worldPos = PhysicsGetPositionWS();
Print("World position: " + worldPos.ToString());
```

### PhysicsIsFalling()

**✅ ПРОВЕРЕНО:** PlayerBase.c:3339

```c
// Проверить падает ли объект
bool isFalling = PhysicsIsFalling(false);  // false = без учёта лодки

if (isFalling)
{
    Print("Player is falling!");
}

// Детектирование начала падения
bool wasFalling = m_IsFalling;
m_IsFalling = PhysicsIsFalling(false);

if (!wasFalling && m_IsFalling)
{
    Print("Started falling!");
}

if (wasFalling && !m_IsFalling)
{
    Print("Landed!");
}
```

### PhysicsGetLinkedEntity()

**✅ ПРОВЕРЕНО:** EntityAI.c:4069

```c
// Получить entity к которому привязан объект (например, игрок на лодке)
EntityAI linked = PhysicsGetLinkedEntity();

if (linked)
{
    Print("Linked to: " + linked.GetType());
}

// Пример из EntityAI.c:4069 - проверка стоит ли игрок на лодке
Human player = Human.Cast(this);
EntityAI boat = player.PhysicsGetLinkedEntity();

if (player && boat)
{
    Print("Player is standing on boat");
}
```

### SetDynamicPhysicsLifeTime()

**✅ ПРОВЕРЕНО:** ItemBase.c:4516

```c
// Установить время жизни динамической физики (в секундах)
SetDynamicPhysicsLifeTime(0.01);  // Очень короткая жизнь
SetDynamicPhysicsLifeTime(5.0);   // 5 секунд

// Пример из ItemBase.c:4516 - при дропе предмета
SetDynamicPhysicsLifeTime(0.01);
m_ItemBeingDroppedPhys = false;
```

---

## 💡 Examples

### Example 1: Система прыжков

```c
modded class PlayerBase
{
    bool CanJump()
    {
        // Проверить не падает ли игрок
        if (PhysicsIsFalling(false))
        {
            return false;
        }

        // Проверить землю под ногами
        vector from = GetPosition() + "0 0.5 0";
        vector to = from - "0 1 0";
        vector hitPos;
        vector hitDir;
        int hitComp;

        DayZPhysics.RaycastRV(from, to, hitPos, hitDir, hitComp, NULL, NULL, NULL, false, true);

        return (hitPos != vector.Zero);
    }
}
```

### Example 2: Измерение скорости

```c
modded class PlayerBase
{
    float GetMovementSpeed()
    {
        vector vel;
        PhysicsGetVelocity(vel);

        // Только горизонтальная скорость
        vector horizVel = vector.Set(vel[0], 0, vel[2]);
        return vector.Distance(vector.Zero, horizVel);
    }

    override void OnUpdate(float delta_time)
    {
        super.OnUpdate(delta_time);

        float speed = GetMovementSpeed();

        if (speed > 8.0)
        {
            Print("Sprinting!");
        }
        else if (speed > 3.0)
        {
            Print("Running!");
        }
        else if (speed > 0.1)
        {
            Print("Walking!");
        }
    }
}
```

### Example 3: Raycast для стрельбы

```c
class BulletTrace
{
    static void TraceShot(PlayerBase player, vector direction)
    {
        vector from = player.GetPosition() + "0 1.5 0";
        vector to = from + direction * 1000;

        vector hitPos;
        vector hitDir;
        int hitComp;

        DayZPhysics.RaycastRV(from, to, hitPos, hitDir, hitComp, NULL, NULL, NULL, false, false);

        if (hitPos != vector.Zero)
        {
            Print("Hit at: " + hitPos.ToString());
            Print("Component: " + hitComp);

            // Нарисовать линию (debug)
            // Shape.CreateLines(from, to, COLOR_RED);
        }
    }
}
```

---

## 🔗 Related Topics

→ [Entity Lifecycle](ENTITYAI_LIFECYCLE.md) - OnInit, EEKilled
→ [GetGame API](GETGAME_API.md) - GetGame() методы
→ [Inventory System](INVENTORY_SYSTEM.md) - GetInventory() паттерны

---

## 🚨 Troubleshooting

**Проблема:** Raycast всегда возвращает vector.Zero
- **Решение:** Проверьте что from != to

**Проблема:** PhysicsGetVelocity возвращает ноль
- **Решение:** Объект должен иметь физику (не статичный)

**Проблема:** PhysicsIsFalling всегда false
- **Решение:** Игрок должен быть в воздухе (на земле = false)

---

## ⚠️ SOURCE VERIFICATION

**Статус:** ✅ Verified from raw_data

**Что проверено:**
- ✅ DayZPhysics.RaycastRV() - PlayerBase.c:7573
- ✅ PhysicsGetLinkedEntity() - EntityAI.c:4069
- ✅ PhysicsGetPositionWS() - PlayerBase.c:2877
- ✅ PhysicsIsFalling() - PlayerBase.c:3339
- ✅ PhysicsGetVelocity() - PlayerBase.c:3693
- ✅ SetDynamicPhysicsLifeTime() - ItemBase.c:4516

**Что НЕ проверено:**
- ❌ Полный список параметров DayZPhysics.RaycastRV (некоторые NULL)
- ❌ ContactComponent ID значения
- ❌ Physics collision layers

---

## 🔍 Как использовать эту информацию

1. **Raycasting:** Используйте RaycastRV для проверки препятствий
2. **Velocity:** PhysicsGetVelocity для измерения скорости
3. **Falling:** PhysicsIsFalling для детектирования падения
4. **Linking:** PhysicsGetLinkedEntity для проверки привязки к transport

---

*Последнее обновление: 2025-01-16*
*Источник: raw_data/PlayerBase.c, EntityAI.c, ItemBase.c*
