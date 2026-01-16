# Анализ паттернов из кода игры (Raw Data)

**Этот документ содержит реальный паттерны кода, извлечённые из PBO файлов игры.**

---

## 📊 Анализируемые файлы

- [EntityAI.c](EntityAI.c) - Базовый класс для всех объектов (130KB)
- [ItemBase.c](ItemBase.c) - Базовый класс для предметов (138KB)
- [PlayerBase.c](PlayerBase.c) - Класс игрока (265KB)
- [Man.c](Man.c) - Человек (родитель PlayerBase) (36KB)

---

## 🎯 Ключевые находки

### 1. Lifecycle методы (EntityAI)

**Порядок вызова при создании объекта:**

```c
// 1. EEInit() - Инициализация
override void EEInit()
{
    super.EEInit();
    // Ваш код инициализации
}

// 2. EEParentedTo() - Когда объект добавлен к родителю
override void EEParentedTo(EntityAI parent)
{
    super.EEParentedTo(parent);
    // Объект теперь имеет родителя
}

// 3. EEInventoryIn() - Когда объект попадает в инвентарь
override void EEInventoryIn(Man newParentMan, EntityAI diz, EntityAI newParent)
{
    super.EEInventoryIn(newParentMan, diz, newParent);
    // Объект в инвентаре игрока
}
```

**Порядок при уничтожении:**

```c
// EEHealthLevelChanged() - При изменении здоровья
override void EEHealthLevelChanged(int oldLevel, int newLevel, string zone)
{
    super.EEHealthLevelChanged(oldLevel, newLevel, zone);

    if (newLevel == GameConstants.STATE_RUINED)
    {
        // Объект разрушен
        OnDamageDestroyed(oldLevel);
    }
}

// EEKilled() - При смерти объекта
override void EEKilled(Object killer)
{
    super.EEKilled(killer);
    // Объект убит
}
```

---

### 2. Работа с инвентарём (EntityAI)

**Добавление предмета в инвентарь:**

```c
// Способ 1: Создать прямо в инвентаре
EntityAI item = GetInventory().CreateInInventory("Apple");

// Способ 2: Взять существующий предмет
bool taken = GetInventory().TakeEntityToInventory(InventoryMode.SERVER, flags, item);

// Способ 3: Добавить в cargo (слоты хранения)
bool added = GetInventory().TakeEntityToCargo(InventoryMode.SERVER, item);

// Способ 4: Прикрепить к слоту
bool attached = GetInventory().TakeEntityAsAttachment(InventoryMode.SERVER, item);
```

**Поиск предметов:**

```c
// Найти attachment по слоту
EntityAI gloves = GetInventory().FindAttachment(InventorySlots.GLOVES);
EntityAI shoes = GetInventory().FindAttachment(InventorySlots.FEET);

// Перечислить все предметы
array<EntityAI> itemsArray = new array<EntityAI>;
GetInventory().EnumerateInventory(InventoryTraversalType.LEVELORDER, itemsArray);

// Подсчёт предметов
int count = itemsArray.Count();
```

---

### 3. Система здоровья (EntityAI)

**Получение здоровья:**

```c
// Общее здоровье
float health = GetHealth("", "");

// Здоровье части тела
float legHealth = GetHealth("RightLeg", "Health");

// Кровь и шок
float blood = GetHealth("", "Blood");
float shock = GetHealth("", "Shock");
```

**Установка здоровья:**

```c
// Установить здоровье
SetHealth("", "", 100);  // Полное здоровье

// Нанести урон
SetHealth("", "", 50);  // Установить 50 HP

// Убить часть тела
SetHealth("RightLeg", "", 0);  // Отломать ногу
```

**Проверка состояния:**

```c
// Проверка разрушен ли объект
bool isRuined = IsDamageDestroyed();

// Проверка жив ли игрок
bool isAlive = IsAlive();

// Получить уровень здоровья (0-3)
int level = GetHealthLevel("");
// 0 = Healthy, 1 = Worn, 2 = Damaged, 3 = Ruined
```

---

### 4. RPC и синхронизация (EntityAI)

**Регистрация переменных:**

```c
void MyEntity()
{
    // Регистрация bool переменной
    RegisterNetSyncVariableBool("m_MyBool");

    // Регистрация int с диапазоном
    RegisterNetSyncVariableInt("m_MyInt", 0, 100);

    // Регистрация float с диапазоном и точностью
    RegisterNetSyncVariableFloat("m_MyFloat", 0.0, 100.0, 2);

    // Регистрация объекта
    RegisterNetSyncVariableObject("m_MyObject");
}
```

**Отправка RPC:**

```c
// Отправить RPC на клиент
Param1<bool> params = new Param1<bool>(true);
GetGame().RPC(this, ERPCs.RPC_MY_FUNCTION, params, true, identity);
```

**Обработка входящих RPC:**

```c
override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
{
    super.OnRPC(sender, rpc_type, ctx);

    switch (rpc_type)
    {
        case ERPCs.RPC_MY_FUNCTION:
            // Обработка
            break;
    }
}
```

**Маркировка "грязной" синхронизации:**

```c
// После изменения переменной вызвать:
SetSynchDirty();
```

---

### 5. Методы PlayerBase

**Инициализация действий:**

```c
override void SetActions(out TInputActionMap InputActionMap)
{
    super.SetActions(InputActionMap);

    // Добавить кастомное действие
    AddAction(ActionMyCustomAction, InputActionMap);
}
```

**Работа со здоровьем игрока:**

```c
// Получить статы
PlayerStats stats = GetPlayerStats();

// Добавить здоровье
stats.GetHealth().Add(10.0);

// Добавить кровь
AddHealth("", "Blood", 1000);

// Проверить жив ли
bool alive = IsAlive();
```

**Поиск предметов в инвентаре:**

```c
// Подсчёт типа предметов
int CountItemType(string item_type)
{
    // Метод для подсчёта предметов определённого типа
    // Реализация в PlayerBase.c
}

// Найти предмет по типу
EntityAI FindItemByType(string item_type)
{
    array<EntityAI> items = new array<EntityAI>;
    GetInventory().EnumerateInventory(InventoryTraversalType.LEVELORDER, items);

    for (int i = 0; i < items.Count(); i++)
    {
        if (items.Get(i).GetType() == item_type)
            return items.Get(i);
    }
    return null;
}
```

**Отправка сообщений игроку:**

```c
// Действие (зелёный текст)
MessageAction("Вы использовали предмет");

// Важное сообщение (красный текст)
MessageImportant("Внимание!");

// Дружественное сообщение (жёлтый текст)
MessageFriendly("Другой игрок");

// Debug вывод
Print("Debug message");
Debug.Log("Log message", "category");
```

---

### 6. ItemBase паттерны

**Свойства из config.cpp:**

```cpp
class MyItem: ItemBase
{
    // Вес и размер
    weight = 100;           // Вес в граммах
    itemSize[] = {1, 2};    // Размер слота 1x2

    // Количество
    varQuantityInit = 1;    // Начальное количество
    varQuantityMin = 0;     // Минимальное
    varQuantityMax = 10;    // Максимальное
    varStackMax = 10.0;     // Максимальный стек

    // Температура
    temperaturePerQuantityWeight = 1.0;

    // Чистота
    varCleannessInit = 100;  // Начальная чистота
    varCleannessMin = 0;
    varCleannessMax = 100;
};
```

**OnConsume паттерн:**

```c
override void OnConsume(PlayerBase player)
{
    super.OnConsume(player);

    if (!GetGame().IsServer())
        return;

    // Применить эффекты
    player.GetStatFood().Add(100);
    player.GetStatWater().Add(50);

    // Уменьшить количество
    AddQuantity(-1);

    // Удалить если закончился
    if (GetQuantity() <= 0)
    {
        player.GetInventory().RemoveItem(this);
    }
}
```

**OnAttach/OnDetach паттерн:**

```c
override void OnAttach()
{
    super.OnAttach();

    // Предмет присоединён
    m_IsAttached = true;

    // Применить эффекты
    UpdateWeight();
    SetSynchDirty();
}

override void OnDetach()
{
    super.OnDetach();

    // Предмет отсоединён
    m_IsAttached = false;

    // Удалить эффекты
    SetSynchDirty();
}
```

**OnUpdate с таймером (без утечки памяти):**

```c
private float m_Timer = 0;

override void OnUpdate(float delta_time)
{
    super.OnUpdate(delta_time);

    m_Timer += delta_time;

    // Выполнять каждую секунду (а не каждый кадр)
    if (m_Timer >= 1.0)
    {
        m_Timer = 0;

        // Выполнить логику
        UpdateEffects();
    }
}
```

**Event handlers:**

```c
// При изменении расположения
override void OnItemLocationChanged(EntityAI old_owner, EntityAI new_owner)
{
    super.OnItemLocationChanged(old_owner, new_owner);

    // Предмет перемещён
}

// При входе в инвентарь
override void OnInventoryEnter(Man player)
{
    super.OnInventoryEnter(player);

    // Предмет в инвентаре
}

// При выходе из инвентаря
override void OnInventoryExit(Man player)
{
    super.OnInventoryExit(player);

    // Предмет покинул инвентарь
}

// При изменении здоровья
override void EEHealthLevelChanged(int oldLevel, int newLevel, string zone)
{
    super.EEHealthLevelChanged(oldLevel, newLevel, zone);

    if (newLevel == GameConstants.STATE_RUINED)
    {
        // Предмет разрушен
    }
}
```

---

## 📝 Рекомендации по использованию

### Для создания предметов:

1. **Всегда** вызывайте `super()` в override методах
2. **Всегда** проверяйте `GetGame().IsServer()` перед сетевыми операциями
3. **Используйте** таймеры вместо постоянных проверок в OnUpdate
4. **Вызывайте** `SetSynchDirty()` после изменения синхронизируемых переменных
5. **Очищайте** ресурсы (таймеры) в деструкторе (~ItemBase)

### для модификации игрока:

1. **Используйте** `modded class` для расширения PlayerBase
2. **Вызывайте** `super.SetActions()` перед добавлением своих действий
3. **Используйте** `MessageAction()` для отправки сообщений игроку
4. **Проверяйте** `IsControlledPlayer()` для локального игрока

### Для работы с инвентарём:

1. **Используйте** правильный InventoryMode:
   - `InventoryMode.SERVER` - на сервере
   - `InventoryMode.LOCAL` - локально
   - `InventoryMode.PREDICTIVE` - с предсказанием
2. **Проверяйте** null указатели перед использованием
3. **Используйте** `EnumerateInventory()` для поиска предметов

---

*Дата анализа: 2025-01-16*
*Версия DayZ: 1.26 (estimated)*
