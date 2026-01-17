# Custom Zombie/NPC Example

**Статус:** ✅ Рабочий пример
**Сложность:** Средняя
**Время создания:** 25 минут

---

## 📋 Что это делает

Custom NPC на базе зомби:
- NPC extends InfectedBanditBase
- JSON конфигурация
- Custom лут при смерти
- Агрессивное поведение

---

## 🎯 Как использовать

### Step 1: Добавьте в config.cpp

```cpp
class CfgPatches
{
    class CustomNPC
    {
        units[] = {"CustomBandit"};
        weapons[] = {};
        requiredVersion = 0.1;
        requiredAddons[] = {"DZ_Data"};
    };
};

class CfgVehicles
{
    class InfectedBanditBase;
    class CustomBandit: InfectedBanditBase
    {
        scope = 2;
        displayName = "Bandit";

        // Лут при смерти
        lootTable[] = {
            {"AKM", 0.3},
            {"Magazine_AKM_30Rnd", 0.5},
            {"CannedBeans", 0.8}
        };
    };
};
```

### Step 2: Скопируйте скрипты

```
examples/custom_zombie/
└── scripts/
    └── 4_World/
        └── Classes/
            └── CustomBandit.c
```

### Step 3: Создайте JSON конфигурацию

**$profile:bandit_config.json:**
```json
{
    "Health": 200,
    "Damage": 50,
    "AgroRange": 100,
    "Loot": [
        "AKM",
        "Magazine_AKM_30Rnd",
        "CannedBeans"
    ]
}
```

### Step 4: Спавн через Script

Вставьте в ваш mission:
```c
EntityAI SpawnBandit(vector position)
{
    return GetGame().CreateObject("CustomBandit", position);
}
```

---

## 📁 Структура файлов

```
custom_zombie/
├── README.md                        ← Этот файл
├── config.cpp                       ← Конфигурация
├── bandit_config.json               ← Пример конфигурации
└── scripts/
    └── 4_World/
        └── Classes/
            ├── AIBanditConfig.c     ← Класс конфигурации
            └── CustomBandit.c       ← Класс NPC
```

---

## 🔧 Как это работает

### 1. AIBanditConfig.c

Класс конфигурации NPC:
- Health
- Damage
- AgroRange
- Loot table

### 2. CustomBandit.c

Класс NPC который:
- Расширяет InfectedBanditBase
- Загружает конфигурацию из JSON
- Спавнит лут при смерти

---

## 💡 Кастомизация

### Изменить health:

**В bandit_config.json:**
```json
{
    "Health": 500  // Больше HP
}
```

### Добавить оружие:

**В CustomBandit.c:**
```c
override void EEInit()
{
    super.EEInit();

    // Даем оружие
    ItemBase rifle = ItemBase.Cast(GetInventory().CreateInInventory("AKM"));
    if (rifle)
    {
        // Добавляем магазин
        ItemBase mag = ItemBase.Cast(GetInventory().CreateInInventory("Magazine_AKM_30Rnd"));
        if (mag)
        {
            magazine.SetAmmoCount(30);
        }
    }
}
```

### Создать tier систему:

**В config.cpp:**
```cpp
class CustomBandit_Rookie: CustomBandit
{
    health = 100;
};

class CustomBandit_Veteran: CustomBandit
{
    health = 200;
};

class CustomBandit_Elite: CustomBandit
{
    health = 500;
};
```

---

## ⚠️ Совместимость

- ✅ Работает с vanilla DayZ
- ⚠️ Требует тестирации с другими AI модами
- ⚠️ Может конфликтовать с infected модами

---

## 🐛 Troubleshooting

### NPC не спавнится

1. Проверьте config.cpp
2. Проверьте что класс зарегистрирован в CfgPatches
3. Проверьте script log

### NPC не атакует

1. Убедитесь что это InfectedBanditBase (не AnimalBase!)
2. Проверьте что AI включен
3. Проверьте distance для agro

---

## 📊 Производительность

- **CPU:** Минимальное влияние
- **Memory:** ~10 KB per NPC
- **Network:** Не влияет

---

## 🚀 Дальнейшее развитие

### Ideas:
1. **Waypoints** - патрулирование
2. **Factions** - группировка NPC
3. **AI behavior** - кастомное поведение
4. **Dialog system** - разговоры с NPC

**Смотрите:** [topics/15_zombie_ai.md](../../topics/15_zombie_ai.md)

---

## ⚠️ Важные замечания

### Zombie vs Animal

**Используйте InfectedBanditBase для NPC:**
- ✅ Правильные анимации
- ✅ Правильное AI
- ✅ Совместимость с vanilla

**НЕ используйте AnimalBase:**
- ❌ Неправильные анимации
- ❌ Неправильное поведение

---

*Последнее обновление: 2025-01-17*
*Пример создан для обучения*
