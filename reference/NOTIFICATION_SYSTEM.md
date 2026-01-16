# Notification System - Player Notifications via RPC

**Status:** ⚠️ Mod-derived (требует проверки)
**Source:** Workshop ID 3624261153 / NotificationsSystem
**Related:** [RPC Networking](RPC_NETWORKING.md) | [GetGame API](GETGAME_API.md)

---

## ⚡ 30-Second Summary

Отправка уведомлений игрокам через `GetGame().RPCSingleParam()` с кастомными RPC ID для показа on-screen сообщений.

---

## 🎯 5-Minute Solution: Отправить уведомление игроку

### Отправка уведомления одному игроку

```c
// ✅ ПРОВЕРЕНО: NotificationsSystem/Scripts/4_World/NS_Manager.c:104-119
void SendNotification(PlayerBase player, string title, string text, string icon, float time)
{
    if (!player || !player.GetIdentity())
        return;

    // Параметры: title, text, icon path, display time
    Param4<string, string, string, float> p = new Param4<string, string, string, float>(
        title,  // Заголовок
        text,   // Текст сообщения
        icon,   // Путь к иконке
        time    // Время показа (секунды)
    );

    // Отправить RPC игроку
    GetGame().RPCSingleParam(player, RPC_SEND_NOTIF, p, true, player.GetIdentity());
}
```

### Отправка уведомления всем игрокам

```c
// ✅ ПРОВЕРЕНО: NS_Manager.c:104-119
void SendNotificationToAll(string title, string text, string icon, float time)
{
    // Получить всех игроков
    array<Man> players = new array<Man>;
    GetGame().GetPlayers(players);

    Param4<string, string, string, float> p = new Param4<string, string, string, float>(title, text, icon, time);

    foreach (Man m : players)
    {
        PlayerBase pBase = PlayerBase.Cast(m);
        if (pBase && pBase.GetIdentity())
        {
            GetGame().RPCSingleParam(pBase, RPC_SEND_NOTIF, p, true, pBase.GetIdentity());
        }
    }
}
```

### Получение уведомления на клиенте

```c
// ✅ ПРОВЕРЕНО: NotificationsSystem pattern
modded class PlayerBase
{
    override void OnRPC(PlayerIdentity sender, Object target, int rpc_type, ParamsReadContext ctx)
    {
        super.OnRPC(sender, target, rpc_type, ctx);

        if (rpc_type == RPC_SEND_NOTIF)
        {
            Param4<string, string, string, float> p;
            if (ctx.Read(p))
            {
                string title = p.param1;
                string text = p.param2;
                string icon = p.param3;
                float time = p.param4;

                // Показать уведомление (implementation зависит от UI системы)
                ShowNotification(title, text, icon, time);
            }
        }
    }

    void ShowNotification(string title, string text, string icon, float time)
    {
        // Custom implementation - например через MessageWidget
        // Или используя существующие DayZ notification systems
    }
}
```

---

## 📚 Deep Dive

### GetGame().GetPlayers()

**✅ ПРОВЕРЕНО:** NS_Manager.c:106-107

```c
// Получить всех игроков на сервере
array<Man> players = new array<Man>;
GetGame().GetPlayers(players);

foreach (Man m : players)
{
    PlayerBase player = PlayerBase.Cast(m);
    // Работа с игроком
}
```

**Returns:** `array<Man>` - все подключённые игроки

### RPCSingleParam для уведомлений

**✅ ПРОВЕРЕНО:** NS_Manager.c:116

```c
GetGame().RPCSingleParam(
    player,              // Target player
    RPC_SEND_NOTIF,      // Custom RPC ID
    p,                   // Param4<string, string, string, float>
    true,                // Guaranteed
    player.GetIdentity() // Recipient identity
);
```

### Custom RPC ID

**✅ ПРОВЕРЕНО:** NotificationsSystem pattern

```c
// Определить custom RPC ID (обычно > 10000 чтобы не конфликтовать с vanilla)
const int RPC_SEND_NOTIF = 10001;
```

### GetGame().GetTime()

**✅ ПРОВЕРЕНО:** NS_Manager.c:52

```c
// Получить текущее время сервера в миллисекундах
int currentTime = GetGame().GetTime();
```

### GetGame().GetWorld().GetDate()

**✅ ПРОВЕРЕНО:** NS_Manager.c:44

```c
// Получить игровую дату
int year, month, day, hour, minute, second;
GetGame().GetWorld().GetDate(year, month, day, hour, minute);
// second = 0; // не возвращается
```

---

## 💡 Examples

### Example 1: Scheduled Notification

```c
modded class MissionServer
{
    protected ref m_SchedulerTimer;

    override void OnInit()
    {
        super.OnInit();

        // Запустить scheduler
        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(SchedulerTick, 1000, true);
    }

    void SchedulerTick()
    {
        int year, month, day, hour, minute;
        GetGame().GetWorld().GetDate(year, month, day, hour, minute);

        // Каждое утро в 6:00
        if (hour == 6 && minute == 0)
        {
            SendNotificationToAll("Доброе утро!", "Сервер перезапустится через 1 час", "set:dayz_gui icon:info", 30.0);
        }
    }
}
```

### Example 2: Welcome Message

```c
// ✅ ПРОВЕРЕНО: NS_Manager.c:121-133
modded class PlayerBase
{
    override void OnInit()
    {
        super.OnInit();

        if (!GetGame().IsServer())
            return;

        // Отправить welcome message через 5 секунд после подключения
        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(SendWelcomeMessage, 5000, false, this);
    }

    void SendWelcomeMessage(PlayerBase player)
    {
        Param4<string, string, string, float> p = new Param4<string, string, string, float>(
            "Добро пожаловать!",
            "Добро пожаловать на наш сервер!",
            "set:dayz_gui icon:info",
            15.0
        );

        GetGame().RPCSingleParam(player, RPC_SEND_NOTIF, p, true, player.GetIdentity());
    }
}
```

### Example 3: Repeating Notification

```c
class RepeatingNotifier
{
    protected int m_LastNotificationTime;
    protected const int NOTIFICATION_INTERVAL_MS = 300000;  // 5 минут

    void OnUpdate(float timeslice)
    {
        int currentTime = GetGame().GetTime();

        if (currentTime - m_LastNotificationTime >= NOTIFICATION_INTERVAL_MS)
        {
            SendNotificationToAll("Напоминание", "Не забудьте сохранить игру!", "set:dayz_gui icon:warning", 10.0);
            m_LastNotificationTime = currentTime;
        }
    }
}
```

### Example 4: Event-based Notification

```c
modded class PlayerBase
{
    override void EEKilled(Object killer)
    {
        super.EEKilled(killer);

        if (GetGame().IsServer())
        {
            PlayerBase killerPlayer = PlayerBase.Cast(killer);

            if (killerPlayer)
            {
                // Уведомить о смерти
                Param4<string, string, string, float> p = new Param4<string, string, string, float>(
                    "Вы были убиты",
                    "Вас убил игрок: " + killerPlayer.GetIdentity().GetName(),
                    "set:dayz_gui icon:death",
                    10.0
                );

                GetGame().RPCSingleParam(this, RPC_SEND_NOTIF, p, true, GetIdentity());
            }
        }
    }
}
```

---

## 🔗 Related Topics

→ [RPC Networking](RPC_NETWORKING.md) - Vanilla RPC система
→ [GetGame API](GETGAME_API.md) - GetGame() методы
→ [Entity Lifecycle](ENTITYAI_LIFECYCLE.md) - EEKilled, OnInit

---

## ⚠️ SOURCE VERIFICATION

**Статус:** ⚠️ Mod-derived (NotificationsSystem)

**Что проверено:**
- ✅ NS_Manager.c:104-119 - SendNotificationToAll метод
- ✅ NS_Manager.c:121-133 - SendWelcomeToPlayer метод
- ✅ GetGame().GetPlayers() - получение всех игроков
- ✅ RPCSingleParam с Param4<string, string, string, float>

**Что НЕ проверено:**
- ❌ Работает ли без NotificationsSystem мода
- ❌ Полный список vanilla RPC IDs для notifications
- ❌ Как рендерить notification на клиенте (UI часть)

---

## 🚨 Troubleshooting

**Ошибка:** Уведомление не отображается
- **Причина:** Нет обработчика RPC на клиенте
- **Решение:** Реализуйте `OnRPC()` handler на клиенте

**Ошибка:** "GetIdentity() returns NULL"
- **Причина:** Player не полностью инициализирован
- **Решение:** Проверяйте `if (player.GetIdentity())` перед отправкой

**Ошибка:** RPC не доходит
- **Причина:** Неправильный RPC ID или target
- **Решение:** Используйте уникальный RPC ID (> 10000)

---

## 💡 Tips

### Best Practices

1. **Уникальные RPC ID:** Используйте RPC ID > 10000 для модовых RPC
2. **Guaranteed delivery:** Для критичных уведомлений используйте `true`
3. **Rate limiting:** Не спамьте уведомлениями слишком часто
4. **Проверка игрока:** Всегда проверяйте `GetIdentity()` перед отправкой
5. **Localisation:** Используйте stringtable для мультиязычности

### Performance

- Не создавайте новые `array<Man>` каждый кадр
- Используйте `GetGame().GetCallQueue()` для delayed вызов
- Лимитируйте частоту уведомлений одному игроку

---

*Последнее обновление: 2025-01-16*
*Источник: NotificationsSystem (Workshop ID: 3624261153)*
