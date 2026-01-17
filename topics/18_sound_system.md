# Sound System

**Status:** ⚠️ Advanced
**Related:** [Particle System](../reference/PARTICLE_SYSTEM.md) | [Effects](../reference/SOUND_SYSTEM.md)

---

## ⚡ 30-Second Summary

Проигрывание звуков: SEffectManager.PlaySound(), PlaySoundSet(), PlaySoundOnObject() для позиционных и объектных звуков.

---

## 🎯 5-Minute Solution

### Позиционный звук

```c
EffectSound sound = SEffectManager.PlaySound("SoundSet", position);
sound.SetAutodestroy(true); // Автоудаление
```

### Звук на объекте

```c
EffectSound sound = SEffectManager.PlaySoundOnObject("SoundSet", this);
```

---

## 💡 Examples

### Пример 1: Звук при действии

```c
override void EEHitBy(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos)
{
    super.EEKilled(damageResult, damageType, source, component, dmgZone, ammo, modelPos);
    SEffectManager.PlaySound("hit_SoundSet", GetPosition());
}
```

### Пример 2: Ambient звук

```c
class AmbientSound
{
    protected EffectSound m_Sound;

    void PlayAmbient()
    {
        m_Sound = SEffectManager.PlaySound("ambient_SoundSet", GetPosition());
    }

    void StopAmbient()
    {
        if (m_Sound) m_Sound.SoundStop();
    }
}
```

---

## 🔗 Related Topics

→ [Particle System](../reference/PARTICLE_SYSTEM.md) - Визуальные эффекты
→ [Reference: Sound System](../reference/SOUND_SYSTEM.md) - Полная документация

---

*Последнее обновление: 2025-01-17*
