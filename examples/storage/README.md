# Simple Storage System Example

**Статус:** ✅ Рабочий пример
**Сложность:** Высокая
**Время создания:** 40 минут

---

## 📋 Что это делает

Система персонального хранилища для DayZ:
- Cross-server storage (доступно на любых серверах с этим модом)
- Steam64 ID-based идентификация
- Recursive сохранение (weapon ammo, attachments)
- JSON persistence

---

## 🎯 Как использовать

### Step 1: Создайте предмет-контейнер

Добавьте в config.cpp новый контейнер:

```cpp
class CfgVehicles
{
    class Container_Base;
    class PersonalStorageBox: Container_Base
    {
        scope = 2;
        displayName = "Personal Storage Box";
        model = "\DZ\gear\camping\woodenbox.p3d";
        rotationFlags = 16;
        itemSize[] = {10, 10};
    };
};
```

### Step 2: Скопируйте файлы

```
examples/storage/
└── scripts/
    └── 4_World/
        └── Classes/
            ├── PersonalStorageBox.c
            └── StorageManager.c
```

### Step 3: Используйте

1. Поместите предмет в PersonalStorageBox
2. Закройте инвентарь
3. Предмет сохранится автоматически
4. Доступно на любом сервере с этим модом

---

## 📁 Структура файлов

```
storage/
├── README.md                        ← Этот файл
└── scripts/
    └── 4_World/
        └── Classes/
            ├── StoredItem.c         ← Класс хранимого предмета
            ├── StorageManager.c     ← Менеджер хранилища
            └── PersonalStorageBox.c ← Контейнер
```

---

## 🔧 Как это работает

### 1. StoredItem.c

Класс для представления предмета в хранилище:
- ClassName
- Health
- Quantity
- Attachments (recursive)
- Cargo (recursive)

### 2. StorageManager.c

Менеджер который:
- Сохраняет предметы в JSON
- Загружает из JSON
- Восстанавливает предметы

### 3. PersonalStorageBox.c

Контейнер который:
- Автосохранение при закрытии
- Автозагрузка при открытии
- Steam64 ID-based

---

## 💡 Кастомизация

### Ограничить количество предметов:

**В StorageManager.c:**
```c
protected const int MAX_ITEMS = 50; // Максимум 50 предметов

bool CanStoreItem()
{
    return m_StoredItems.Count() < MAX_ITEMS;
}
```

### Добавить cooldown:

**В PersonalStorageBox.c:**
```c
protected float m_LastAccessTime;
protected const float ACCESS_COOLDOWN = 60; // 1 минута

override bool CanOpenInventory(PlayerBase player)
{
    float currentTime = GetGame().GetTickTime();

    if (currentTime - m_LastAccessTime < ACCESS_COOLDOWN)
    {
        NotificationSystem.Notify(player, "Storage on cooldown!");
        return false;
    }

    return super.CanOpenInventory(player);
}
```

### Добавить cost:

**В PersonalStorageBox.c:**
```c
bool CanUse(PlayerBase player)
{
    // Проверяем валюту
    int balance = GetPlayerCurrency(player);

    if (balance < 10) // 10 монет
    {
        NotificationSystem.Notify(player, "Not enough coins!");
        return false;
    }

    return true;
}
```

---

## ⚠️ Совместимость

- ✅ Работает с vanilla DayZ
- ⚠️ Может конфликтовать с другими storage модами
- ⚠️ Требует тестирации с high-pop серверами

---

## 🐛 Troubleshooting

### Предметы не сохраняются

1. Проверьте права на запись `$profile:`
2. Проверьте script log
3. Убедитесь что сервер перезапустился

### Attachments теряются

1. Проверьте что recursive сохранение включено
2. Проверьте script log на ошибки
3. Убедитесь что предметы совместимы

---

## 📊 Производительность

- **Memory:** ~1-5 KB per item
- **Disk I/O:** При открытии/закрытии
- **Network:** Не влияет (server-side)

---

## 🚀 Дальнейшее развитие

### Ideas:
1. **GUI** - меню хранилища
2. **Categories** - категоризация предметов
3. **Search** - поиск по хранилищу
4. **Sharing** - совместное хранилище

**Смотрите:** [topics/14_storage.md](../../topics/14_storage.md)

---

## ⚠️ Важные замечания

### Recursive сохранение

Этот пример поддерживает recursive сохранение:
- Weapon + magazine + ammo
- Clothing + attachments
- Container + items

### Cross-server access

Хранилище основано на Steam64 ID, поэтому:
- Доступно на ЛЮБЫХ серверах с этим модом
- Привязано к Steam аккаунту
- Не зависит от сервера

---

*Последнее обновление: 2025-01-17*
*Пример создан для обучения*
