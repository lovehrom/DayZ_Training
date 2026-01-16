# Sound System - Звуковая система

**✅ ПРОВЕРЕНО:** raw_data/EntityAI.c, PlayerBase.c
**Источник:** Распакованный PBO (игровые файлы)

---

## SEffectManager.PlaySound()

Проиграть звук в определённой позиции.

```c
// ✅ ПРОВЕРЕНО: EntityAI.c:4012,4018,4024,4030
EffectSound sound = SEffectManager.PlaySound("SoundSetName", position);

// Примеры из EntityAI.c:
EffectSound sound = SEffectManager.PlaySound("hardTreeFall_SoundSet", GetPosition());  // 4012
EffectSound sound = SEffectManager.PlaySound("softTreeFall_SoundSet", GetPosition());  // 4018
EffectSound sound = SEffectManager.PlaySound("hardBushFall_SoundSet", GetPosition());   // 4024

// Автоуничтожение после проигрывания:
sound.SetAutodestroy(true);
```

---

## SEffectManager.PlaySoundOnObject()

Проиграть звук привязанный к объекту.

```c
// ✅ ПРОВЕРЕНО: PlayerBase.c:1217,7293
EffectSound sound = SEffectManager.PlaySoundOnObject("SoundSetName", this);
```

---

## PlaySoundSet()

Проиграть SoundSet (на объекте).

```c
// ✅ ПРОВЕРЕНО: PlayerBase.c:3636,5656
PlaySoundSet(m_SoundVariable, "SoundSetName", volume, spatialOffset);

// Пример из PlayerBase.c:3636:
PlaySoundSet(m_BrokenLegSound, SOUND_BREAK_LEG, 0.1, 0.1);
```

**Параметры:**
- `m_SoundVariable` - переменная для хранения Sound (ref EffectSound)
- `"SoundSetName"` - имя SoundSet из config
- `volume` - громкость (0.0 - 1.0)
- `spatialOffset` - пространственное смещение

---

## PlaySoundSetLoop()

Проиграть зацикленный звук.

```c
// ✅ ПРОВЕРЕНО: PlayerBase.c:9050,9067
PlaySoundSetLoop(m_SoundVariable, "SoundSetName", volume, spatialOffset);
```

Используется для долгозвучащих эффектов (мхиты, окружение и т.д.)

---

## StopSoundSet()

Остановить проигрывание SoundSet.

```c
// ✅ ПРОВЕРЕНО: PlayerBase.c:1225,2631,5678,9074
StopSoundSet(m_SoundVariable);

// Пример из PlayerBase.c:1225:
SEffectManager.DestroyEffect(m_FliesEff);
StopSoundSet(m_SoundFliesEffect);
```

**Важно:** Вызывайте `SEffectManager.DestroyEffect()` перед остановкой звука.

---

## Template

### Проиграть разовый звук

```c
class MyItem extends ItemBase
{
    void PlayFallingSound()
    {
        // Звук в позиции объекта (автоуничтожение)
        EffectSound sound = SEffectManager.PlaySound("hardTreeFall_SoundSet", GetPosition());
        sound.SetAutodestroy(true);
    }
}
```

### Проиграть звук на игроке

```c
class PlayerBase extends Man
{
    ref EffectSound m_MySound;

    void PlayCustomSound()
    {
        PlaySoundSet(m_MySound, "SOUND_MY_CUSTOM", 0.5, 0.1);
    }

    void StopCustomSound()
    {
        StopSoundSet(m_MySound);
    }
}
```

### Зацикленный звук

```c
class MyItem extends ItemBase
{
    ref EffectSound m_LoopSound;

    void StartLoopSound()
    {
        PlaySoundSetLoop(m_LoopSound, "ambient_loop_SoundSet", 1.0, 0.0);
    }

    void StopLoopSound()
    {
        if (m_LoopSound)
        {
            StopSoundSet(m_LoopSound);
        }
    }
}
```

---

## SoundSet имена

⚠️ **ВНИМАНИЕ:** SoundSet имена берутся из config игры.

**Как найти SoundSet:**
1. Поищите в raw_data/config.cpp
2. Используйте DZ Tools для просмотра SoundSets
3. Примеры: `SOUND_BREAK_LEG`, `hardTreeFall_SoundSet`

---

## Источники

- **raw_data/EntityAI.c** (строки 4012, 4018, 4024, 4030)
- **raw_data/PlayerBase.c** (строки 1217, 1225, 2631, 3636, 5678, 5656, 7293, 9050, 9067, 9074)

---

## ⚠️ Что НЕ найдено

- Полный список всех SoundSet имён
- Официальная документация по SoundSet системе
- Как создать кастомный SoundSet

---

*Последнее обновление: 2025-01-16*
*Статус: Проверено на raw_data*
