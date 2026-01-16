# SliverWeapons - Custom RPC for Global Sounds

**Status:** ⚠️ Mod-derived (требует проверки)
**Source:** Workshop ID 3644228096 / SliverWeapons
**Related:** [RPC Networking](RPC_NETWORKING.md) | [GetGame API](GETGAME_API.md)

---

## ⚡ 30-Second Summary

Отправка глобальных звуков всем игрокам на сервере через custom RPC с уникальным ID.

---

## 🎯 5-Minute Solution: Отправить звук всем игрокам

### Custom RPC ID

```c
// ✅ ПРОВЕРЕНО: AdminHammer_KillSound.c
static const int RPC_ADMINHAMMER_KILL_SOUND = 77801;  // Уникальный ID > 10000
```

### Broadcast функция

```c
// ✅ ПРОВЕРЕНО: SliverWeapons pattern
void BroadcastAdminHammerKillSound()
{
    // Получить всех игроков на сервере
    array<Man> players = new array<Man>;
    GetGame().GetPlayers(players);

    // Отправить RPC каждому игроку
    foreach (Man man : players)
    {
        PlayerBase pb;
        if (Class.CastTo(pb, man))
        {
            GetGame().RPCSingleParam(pb, RPC_ADMINHAMMER_KILL_SOUND, null, true, pb.GetIdentity());
        }
    }
}
```

### Получение RPC на клиенте

```c
modded class PlayerBase
{
    override void OnRPC(PlayerIdentity sender, Object target, int rpc_type, ParamsReadContext ctx)
    {
        super.OnRPC(sender, target, rpc_type, ctx);

        if (rpc_type == RPC_ADMINHAMMER_KILL_SOUND)
        {
            // Проиграть звук на клиенте
            PlayKillSound();
        }
    }

    void PlayKillSound()
    {
        // Custom sound playback
        SEffectManager.PlaySound("AdminHammer_Kill_SoundSet", GetPosition());
    }
}
```

---

## 📚 Deep Dive

### GetGame().GetPlayers() для broadcast

**✅ ПРОВЕРЕНО:** AdminHammer_KillSound.c pattern

```c
// Получить всех игроков на сервере
array<Man> players = new array<Man>;
GetGame().GetPlayers(players);

foreach (Man man : players)
{
    PlayerBase pb = PlayerBase.Cast(man);
    if (pb && pb.GetIdentity())
    {
        // Отправить RPC конкретному игроку
        GetGame().RPCSingleParam(pb, RPC_ID, params, true, pb.GetIdentity());
    }
}
```

**Уникальность:**
- **null** параметр для RPC (если не нужно передавать данные)
- **true** для guaranteed delivery
- **pb.GetIdentity()** для идентификации получателя

### Custom RPC ID выбор

**✅ ПРОВЕРЕНО:** SliverWeapons pattern

```c
// Используйте ID > 10000 для избежания конфликтов с vanilla RPC
static const int RPC_ADMINHAMMER_KILL_SOUND = 77801;
static const int RPC_CUSTOM_SOUND_2 = 77802;
static const int RPC_CUSTOM_SOUND_3 = 77803;
```

**Почему > 10000:**
- Vanilla RPC IDs обычно < 10000
- Избежание конфликтов с будущими обновлениями

---

## 💡 Examples

### Example 1: Kill sound broadcast

```c
// ✅ ПРОВЕРЕНО: SliverWeapons AdminHammer pattern
class AdminHammer_KillSound
{
    static const int RPC_ADMINHAMMER_KILL_SOUND = 77801;

    static void BroadcastKillSound()
    {
        array<Man> players = new array<Man>;
        GetGame().GetPlayers(players);

        foreach (Man man : players)
        {
            PlayerBase pb;
            if (Class.CastTo(pb, man))
            {
                GetGame().RPCSingleParam(pb, RPC_ADMINHAMMER_KILL_SOUND, null, true, pb.GetIdentity());
            }
        }
    }
}
```

### Example 2: Multiple sound broadcasts

```c
class SoundManager
{
    static const int RPC_SOUND_EFFECT_1 = 77801;
    static const int RPC_SOUND_EFFECT_2 = 77802;
    static const int RPC_SOUND_EFFECT_3 = 77803;

    static void BroadcastSound(int rpc_type)
    {
        array<Man> players = new array<Man>;
        GetGame().GetPlayers(players);

        foreach (Man man : players)
        {
            PlayerBase pb = PlayerBase.Cast(man);
            if (pb && pb.GetIdentity())
            {
                GetGame().RPCSingleParam(pb, rpc_type, null, true, pb.GetIdentity());
            }
        }
    }
}
```

---

## 🔗 Related Topics

→ [RPC Networking](RPC_NETWORKING.md) - Vanilla RPC система
→ [GetGame API](GETGAME_API.md) - GetGame() методы
→ [Notification System](NOTIFICATION_SYSTEM.md) - Player notifications via RPC

---

## ⚠️ SOURCE VERIFICATION

**Статус:** ⚠️ Mod-derived (SliverWeapons)

**Что проверено:**
- ✅ RPC_ADMINHAMMER_KILL_SOUND = 77801 custom ID
- ✅ GetGame().GetPlayers() broadcast pattern
- ✅ GetGame().RPCSingleParam() с null параметром

**Что НЕ проверено:**
- ❌ Работает ли без SliverWeapons мода
- ❌ Полный список vanilla RPC IDs (чтобы убедиться в отсутствии конфликтов)
- ❌ Как клиент обрабатывает этот RPC (полная реализация OnRPC)

---

## 🚨 Troubleshooting

**Ошибка:** Звук не воспроизводится на клиентах
- **Причина:** Нет обработчика RPC на клиенте
- **Решение:** Реализуйте `OnRPC()` handler в PlayerBase

**Ошибка:** Конфликт RPC ID
- **Причина:** ID совпадает с vanilla или другим модом
- **Решение:** Используйте уникальный ID > 10000

**Ошибка:** RPC не доходит до некоторых игроков
- **Причина:** GetIdentity() возвращает NULL
- **Решение:** Проверяйте `if (pb.GetIdentity())` перед отправкой

---

## 💡 Tips

### Best Practices

1. **Уникальные RPC ID:** Используйте ID > 10000 и задокументируйте их
2. **Guaranteed delivery:** Для критичных звуков используйте `true`
3. **Проверка игрока:** Всегда проверяйте `GetIdentity()` перед отправкой
4. **Null параметры:** Если не нужно передавать данные, используйте `null`

### Performance

- Не создавайте новые `array<Man>` каждый кадр (кэшируйте если частый вызов)
- Используйте sound sets вместо прямых путей к файлам
- Лимитируйте частоту broadcast (не спамьте звуками)

---

*Последнее обновление: 2025-01-16*
*Источник: SliverWeapons (Workshop ID: 3644228096)*
