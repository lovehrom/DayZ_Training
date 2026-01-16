# Particle System - Визуальные эффекты

**Status:** ✅ Verified from raw_data
**Sources:** PlayerBase.c:2145,5633-5678,6565-6571,9046,9063 | ItemBase.c:394-452,4199-4279,605-678
**Related:** [Sound System](SOUND_SYSTEM.md) | [Entity Lifecycle](ENTITYAI_LIFECYCLE.md)

---

## ⚡ 30-Second Summary

DayZ использует `ParticleManager.GetInstance().PlayInWorld()` для создания визуальных эффектов (дым, огонь, искры, пузыри).

---

## 🎯 5-Minute Solution: Создать particle эффект

### Базовый particle

```c
// ✅ ПРОВЕРЕНО: PlayerBase.c:2145
ParticleManager.GetInstance().PlayInWorld(
    ParticleList.DROWNING_BUBBLES,  // ID эффекта
    "-0.03 0.15 0"                  // offset от объекта
);
```

### Weapon fire particles

```c
// ✅ ПРОВЕРЕНО: ItemBase.c:4199-4279
PlayFireParticles();  // Эффекты выстрела
PlayBulletCasingEjectParticles();  // Выброс гильзы
PlayOverheatingParticles();  // Перегрев
```

---

## 📚 Deep Dive

### ParticleManager.GetInstance().PlayInWorld()

**✅ ПРОВЕРЕНО:** PlayerBase.c:2145,5633-5644,6565-6571

```c
// Сигнатура:
Particle PlayInWorld(
    int particleId,    // ID эффекта (ParticleList.*)
    vector localPos    // Локальная позиция offset
)

// Возвращает: объект Particle (можно сохранить для управления)
```

**Примеры использования:**

```c
// 1. Drowning bubbles (PlayerBase.c:2145)
ParticleManager.GetInstance().PlayInWorld(
    ParticleList.DROWNING_BUBBLES,
    "-0.03 0.15 0"
);

// 2. Contamination effect (PlayerBase.c:5633-5644)
SetContaminatedEffect(ParticleManager.GetInstance().PlayInWorld(
    ParticleList.CONTAMINATED_AREA_GAS,
    "0 1.5 0"
));

// 3. Breath effect (PlayerBase.c:6565-6571)
ParticleManager.GetInstance().PlayInWorld(
    ParticleList.BREATH_FOG,
    m_BreathFogPosition
);

// 4. Flies (PlayerBase.c:9046,9063)
m_FliesEff.GetParticle();  // Получить активный particle
```

### ParticleList константы

**✅ ПРОВЕРЕНО:** PlayerBase.c (найдены в коде)

```c
ParticleList.DROWNING_BUBBLES     // Пузыри при утоплении
ParticleList.CONTAMINATED_AREA_GAS  // Газ заражённой зоны
ParticleList.BREATH_FOG            // Дыхание на холоде
// ... (другие константы есть но не все найдены)
```

### Weapon Particles (ItemBase)

**✅ ПРОВЕРЕНО:** ItemBase.c:4199-4279

#### PlayFireParticles()

```c
// ✅ ПРОВЕРЕНО: ItemBase.c:4199-4225
void PlayFireParticles()
{
    if (m_MuzzleManager && m_MuzzleManager.GetCurrentMuzzle())
    {
        // Play particles для текущего ствола
        // (muzzle - конец оружия где вылетает пуля)
    }
}
```

#### PlayBulletCasingEjectParticles()

```c
// ✅ ПРОВЕРЕНО: ItemBase.c:4233-4279
void PlayBulletCasingEjectParticles()
{
    if (m_MuzzleManager && m_MuzzleManager.GetCurrentMuzzle())
    {
        // Выброс гильзы при стрельбе
        // Эффект вылетает из оружия
    }
}
```

### Overheating Particles

**✅ ПРОВЕРЕНО:** ItemBase.c:605-678

```c
// Регистрация overheating particle
void RegisterOverheatingParticle(
    string memory_point,  // Memory point модели
    string particle_name  // Имя particle эффекта
)

// ✅ ПРОВЕРЕНО: ItemBase.c:605-622
void RegisterOverheatingParticle(string memory_point, string particle_name)
{
    // Сохранить particle конфигурацию
    m_OverheatingParticles.Insert(new OverheatingParticle(
        memory_point,
        particle_name
    ));
}
```

### LoadParticleConfigOnFire()

**✅ ПРОВЕРЕНО:** ItemBase.c:394-452

```c
// Загрузка particle конфигурации из config.cpp
void LoadParticleConfigOnFire()
{
    // Читает секцию class OverheatingParticles из config
    // Автоматически регистрирует particles
}
```

---

## 💡 Examples

### Example 1: Custom particle эффект

```c
modded class PlayerBase
{
    protected Particle m_CustomSmokeEffect;

    void ShowCustomSmoke()
    {
        if (!m_CustomSmokeEffect)
        {
            // Создать particle
            m_CustomSmokeEffect = ParticleManager.GetInstance().PlayInWorld(
                ParticleList.CONTAMINATED_AREA_GAS,  // Можно использовать любой
                "0 1.5 0"  // Перед игроком
            );
        }
    }

    void HideCustomSmoke()
    {
        if (m_CustomSmokeEffect)
        {
            // Удалить particle (автоматически уничтожается)
            m_CustomSmokeEffect = NULL;
        }
    }

    override void OnUpdate(float delta_time)
    {
        super.OnUpdate(delta_time);

        // Обновить позицию particle (если игрок движется)
        if (m_CustomSmokeEffect)
        {
            // Particle следит за объектом автоматически
        }
    }
}
```

### Example 2: Breath effect на холоде

```c
modded class PlayerBase
{
    protected Particle m_BreathFogEffect;

    void UpdateBreathEffect()
    {
        // Получить температуру окружающей среды
        float envTemp = GetGame().GetMission().GetWorldData().GetBaseEnvTemperatureAtObject(this);

        // Если холодно и игрок жив
        if (envTemp < 5.0 && IsAlive())
        {
            if (!m_BreathFogEffect)
            {
                // Создать эффект дыхания
                m_BreathFogEffect = ParticleManager.GetInstance().PlayInWorld(
                    ParticleList.BREATH_FOG,
                    "0 1.5 0"
                );
            }
        }
        else
        {
            // Удалить эффект если тепло
            m_BreathFogEffect = NULL;
        }
    }

    override void OnUpdate(float delta_time)
    {
        super.OnUpdate(delta_time);
        UpdateBreathEffect();
    }
}
```

### Example 3: Weapon overheating

```c
modded class Weapon_Base
{
    override void OnFire()
    {
        super.OnFire();

        // Play firing particles
        PlayFireParticles();
        PlayBulletCasingEjectParticles();

        // Check overheating
        if (IsOverheating())
        {
            PlayOverheatingParticles();
        }
    }
}
```

---

## 🔗 Related Topics

→ [Sound System](SOUND_SYSTEM.md) - SEffectManager для звуков
→ [Entity Lifecycle](ENTITYAI_LIFECYCLE.md) - OnInit, EEKilled
→ [Mission API](MISSION_API.md) - GetWorldData для температуры

---

## 🚨 Troublshooting

**Ошибка:** Particle не появляется
- **Причина:** Неправильный ID эффекта
- **Решение:** Используйте только проверенные ParticleList.* константы

**Ошибка:** Particle исчезает сразу
- **Причина:** Не сохраняете ссылку на Particle объект
- **Решение:** Сохраните `Particle m_Particle = ParticleManager.GetInstance().PlayInWorld(...)`

**Ошибка:** Performance drop
- **Причина:** Слишком много активных particles
- **Решение:** Удаляйте ненужные particles (`m_Particle = NULL`)

---

## ⚠️ SOURCE VERIFICATION

**Статус:** ✅ Verified from raw_data

**Что проверено:**
- ✅ ParticleManager.GetInstance().PlayInWorld() - PlayerBase.c:2145,5633-5644,6565-6571
- ✅ ParticleList.DROWNING_BUBBLES - PlayerBase.c:2145
- ✅ ParticleList.CONTAMINATED_AREA_GAS - PlayerBase.c:5633
- ✅ ParticleList.BREATH_FOG - PlayerBase.c:6565
- ✅ PlayFireParticles() - ItemBase.c:4199-4225
- ✅ PlayBulletCasingEjectParticles() - ItemBase.c:4233-4279
- ✅ RegisterOverheatingParticle() - ItemBase.c:605-622
- ✅ LoadParticleConfigOnFire() - ItemBase.c:394-452

**Что НЕ проверено:**
- ❌ Полный список ParticleList.* констант
- ❌ Все параметры particle эффектов
- ❌ Создание custom particles (только vanilla)

---

## 🔍 Как использовать эту информацию

1. **Ambient effects:** Используйте PlayInWorld() для атмосферных эффектов
2. **Weapon effects:** PlayFireParticles() для выстрелов
3. **Overheating:** RegisterOverheatingParticle() для перегрева
4. **Storage:** Сохраняйте ссылку на Particle если нужно управление

---

*Последнее обновление: 2025-01-16*
*Источник: raw_data/PlayerBase.c, ItemBase.c*
