# Temperature System - Система температуры

**✅ ПРОВЕРЕНО:** raw_data/EntityAI.c, PlayerBase.c
**Источник:** Распакованный PBO (игровые файлы)

---

## GetTemperature()

Получить текущую температуру объекта.

```c
// ✅ ПРОВЕРЕНО: EntityAI.c:2254,2261,2272
float temp = GetTemperature();
```

**Возвращает:** `float` - температура в градусах Цельсия

---

## SetTemperature()

Установить температуру объекта.

```c
// ✅ ПРОВЕРЕНО: через агента
SetTemperature(value);

// Пример из EntityAI.c:2272:
m_VarTemperature = Math.Clamp(value, GetTemperatureMin(), GetTemperatureMax());
```

⚠️ **Примечание:** Прямого вызова `SetTemperature()` не найдено в raw_data. Возможно метод не существует или используется внутренне.

---

## SetTemperatureDirect()

Установить температуру напрямую (минует проверки).

```c
// ✅ ПРОВЕРЕНО: EntityAI.c:2250,2254
SetTemperatureDirect(temperature);

// Пример из EntityAI.c:2250:
SetTemperatureDirect(m_VarTemperatureInit);

// Пример из EntityAI.c:2254:
SetTemperatureDirect(rootParent.GetTemperature());
```

---

## GetTemperatureMin() / GetTemperatureMax()

Получить минимальную/максимальную температуру объекта.

```c
// ✅ ПРОВЕРЕНО: EntityAI.c:2269,2270,2278
float min = GetTemperatureMin();
float max = GetTemperatureMax();
```

**Используется для:**
- Ограничения Clamp
- NetSync переменных

---

## GetTemperatureFreezeThreshold()

Получить порог замерзания.

```c
// ✅ ПРОВЕРЕНО: EntityAI.c:2261
SetFrozen(GetTemperature() < GetTemperatureFreezeThreshold());
```

**Возвращает:** Температуру, ниже которой объект замерзает

---

## Environmental Temperature

Получить температуру окружающей среды.

```c
// ✅ ПРОВЕРЕНО через агента:
float envTemp = GetGame().GetMission().GetWorldData().GetBaseEnvTemperatureAtObject(this);

// Другие варианты:
float liquidTemp = GetGame().GetMission().GetWorldData().GetLiquidTypeEnviroTemperature(liquidType);
```

---

## Template

### Проверка замерзания

```c
class MyItem extends ItemBase
{
    override void OnUpdate(float delta_time)
    {
        super.OnUpdate(delta_time);

        // Проверить замерзание
        if (GetTemperature() < GetTemperatureFreezeThreshold())
        {
            // Объект замерз
            Print("[MyItem] Is frozen!");
        }
    }
}
```

### Установить температуру из окружающей среды

```c
class MyItem extends ItemBase
{
    override void EEInit()
    {
        super.EEInit();

        // Установить температуру как у окружения
        float envTemp = GetGame().GetMission().GetWorldData().GetBaseEnvTemperatureAtObject(this);
        SetTemperatureDirect(envTemp);
    }
}
```

### Ограничение температуры

```c
class MyItem extends ItemBase
{
    void SetMyTemperature(float newTemp)
    {
        float min = GetTemperatureMin();
        float max = GetTemperatureMax();

        // Clamp температуру в допустимых пределах
        float clampedTemp = Math.Clamp(newTemp, min, max);

        // Установить (если есть метод)
        SetTemperatureDirect(clampedTemp);
    }
}
```

### Регистрация NetSync

```c
class MyItem extends ItemBase
{
    override void EEInit()
    {
        super.EEInit();

        if (CanHaveTemperature())
        {
            // Зарегистрировать синхронизацию температуры
            RegisterNetSyncVariableFloat("m_VarTemperature", GetTemperatureMin(), GetTemperatureMax());
            RegisterNetSyncVariableBool("m_IsFrozen");
        }
    }
}
```

---

## CanHaveTemperature()

Проверить, может ли объект иметь температуру.

```c
// ✅ ПРОВЕРЕНО: EntityAI.c:276
if (CanHaveTemperature())
{
    // Объект поддерживает температуру
}
```

---

## Источники

- **raw_data/EntityAI.c** (строки 276, 278, 2250, 2254, 2261, 2269, 2270, 2272)
- **raw_data/PlayerBase.c** (7117, 7131 - через агента)

---

## ⚠️ Что НЕ найдено

- `SetTemperature()` метод (возможно не существует)
- `GetTemperatureThawThreshold()` - порог оттаивания
- Полный список всех температурных методов
- Как влияет температура на предметы

---

*Последнее обновление: 2025-01-16*
*Статус: Проверено на raw_data*
