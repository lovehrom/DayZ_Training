# Weapon Example - Пример оружия

## Что это

Полный пример создания кастомного оружия для DayZ. Включает саму винтовку и магазин к ней.

## Как использовать

1. **Скопировать мод:**
   ```
   @WeaponExample/ → K:\SteamLibrary\steamapps\common\DayZ\@WeaponExample\
   ```

2. **Запустить игру:**
   ```
   -mission=.\Missions\DayZCommunityOfflineMode.ChernarusPlus -mod=@WeaponExample
   ```

3. **Проверить в игре:**
   ```
   _test = "CustomRifle"_spawn
   _test = "CustomRifle_Mag"_spawn
   ```

## Файлы

- `config.cpp` - Конфигурация оружия и магазина
- `scripts/3_Game/Weapons/CustomRifle.c` - Код оружия

## Структура

```
@WeaponExample/
├── config.cpp
└── scripts/
    └── 3_Game/
        └── Weapons/
            └── CustomRifle.c
```

## Как это работает

### config.cpp

**Оружие:**
```cpp
class CustomRifle: Rifle_Base
{
    scope = 2;
    displayName = "Custom Rifle";
    model = "\DZ\weapons\rifles\M4\M4.p3d";  // Заглушка M4

    // Слоты для attachments
    class Attachments
    {
        class CustomRifle_Magazine { ... };
        class WeaponOptics { ... };
        class WeaponFlashlight { ... };
    };
}
```

**Магазин:**
```cpp
class CustomRifle_Mag: Magazine_Base
{
    ammoCount = 30;  // 30 патронов

    class Ammo
    {
        type = "Bullet_556x45";  // Стандартные патроны 5.56mm
        count = 30;
    };
}
```

### CustomRifle.c

```c
class CustomRifle extends Rifle_Base
{
    override void EEFired(int muzzleType, int mode, string ammo)
    {
        super.EEFired(muzzleType, mode, ammo);
        // Логика при выстреле
    }
}
```

## Модификация

### Изменить урон

**В config.cpp:**
```cpp
class CustomRifle: Rifle_Base
{
    damage = 100;  // Повышенный урон
}
```

### Изменить магазин

**В config.cpp:**
```cpp
class CustomRifle_Mag: Magazine_Base
{
    ammoCount = 60;  // 60 патронов вместо 30

    class Ammo
    {
        type = "Bullet_762x39";  // Другой калибр
        count = 60;
    };
}
```

### Добавить прицел

**В Attachments:**
```cpp
class WeaponOptics
{
    slotName = "weaponOptics";
    slotType = "weaponOptics_ACOG";  // Конкретный тип прицела
};
```

### Добавить звук

**В CustomRifle.c:**
```c
override void EEFired(int muzzleType, int mode, string ammo)
{
    super.EEFired(muzzleType, mode, ammo);

    // Проиграть кастомный звук
    PlaySound("custom_rifle_shot");
}
```

## Связанные темы

→ [../../topics/01_items.md](../../topics/01_items.md) - Создание предметов (оружие это тоже Item)
→ [../../topics/02_config.md](../../topics/02_config.md) - Конфигурация
→ [../minimal_item/](../minimal_item/) - Простой пример

## Известные ограничения

- Использует заглушку модели M4
- Не имеет уникальных звуков
- Не имеет уникальных анимаций
- Использует стандартные патроны 5.56mm

## Следующие шаги

После изучения этого примера:
1. Добавьте собственную модель
2. Создайте кастомные звуки
3. Добавьте уникальные патроны
4. Создайте кастомные attachments

---
*Последнее обновление: 2025-01-16*
