# PlayerBase: Модификация игрока

**Aliases:** player | игрок | PlayerBase | modded class | модификация игрока
**Related:** [Actions](topics/03_actions.md) | [Events](topics/07_events.md)
**Examples:** [PlayerBase Mod](../examples/playerbase_mod/)

---

## ⚡ 30-Second Summary

PlayerBase - класс игрока в DayZ. Чтобы модифицировать игрока:
1. Создать `modded class PlayerBase` в `.c` файле
2. Переопределить методы (OnInit, OnUpdate, и т.д.)
3. Добавить новую логику (системы здоровья, эффекты, и т.д.)

---

## 🎯 5-Minute Solution: Добавить систему стамины

**Файл:** `scripts/4_World/Entities/Man/PlayerBase.c`

```c
modded class PlayerBase
{
    private float m_Stamina = 100.0;  // Переменная стамины

    override void OnInit()
    {
        super.OnInit();
        // Инициализация стамины
        m_Stamina = 100.0;
    }

    override void OnUpdate(float delta_time)
    {
        super.OnUpdate(delta_time);
        UpdateStamina(delta_time);
    }

    void UpdateStamina(float delta_time)
    {
        // Уменьшение стамины при беге
        if (IsRunning())
        {
            m_Stamina -= 5.0 * delta_time;  // 5 единиц в секунду
        }
        else
        {
            // Восстановление при отдыхе
            m_Stamina += 2.0 * delta_time;  // 2 единицы в секунду
        }

        // Ограничение 0-100
        m_Stamina = Math.Clamp(m_Stamina, 0.0, 100.0);
    }

    float GetStamina()
    {
        return m_Stamina;
    }
};
```

---

## 📚 Deep Dive

### modded class - что это?

**Обычный класс:** Создаёт новый класс
```c
class MyPlayer extends PlayerBase { }  // Новый класс
```

**Modded класс:** Модифицирует существующий
```c
modded class PlayerBase { }  // Модифицирует PlayerBase
```

**Когда использовать:**
- Нужно добавить логику к существующему классу
- Нужно переопределить метод
- Нельзя создать новый класс (например, для игрока)

---

### Жизненный цикл PlayerBase

| Метод | Когда вызывается | Для чего |
|-------|------------------|----------|
| `OnInit()` | При создании игрока | Инициализация |
| `OnUpdate(float delta_time)` | Каждый кадр | Обновление логики |
| `OnFrame()` | Каждый кадр (позже) | Визуальные эффекты |
| `OnConnect()` | При подключении к серверу | Настройка синхронизации |
| `OnDisconnect()` | При отключении | Очистка |

---

### Методы PlayerBase

#### OnInit() - Инициализация

```c
modded class PlayerBase
{
    override void OnInit()
    {
        super.OnInit();  // Всегда вызывать super!

        // Инициализация переменных
        m_MyVariable = 0;

        // Подписка на события
        // ...
    }
};
```

#### OnUpdate() - Обновление

```c
modded class PlayerBase
{
    override void OnUpdate(float delta_time)
    {
        super.OnUpdate();  // Всегда вызывать super!

        // Обновление логики каждый кадр
        UpdateMySystem(delta_time);
    }
};
```

**Важно:** `OnUpdate()` вызывается **каждый кадр** (60 раз в секунду). Будьте осторожны с производительностью!

---

### Система здоровья игрока

```c
modded class PlayerBase
{
    override void OnInit()
    {
        super.OnInit();

        // Получить систему здоровья
        PlayerStatsInterface stats = GetStat();

        // Статистика
        stats.GetHealth().Set(100);  // Здоровье
        stats.GetBlood().Set(5000);  // Кровь
        stats.GetEnergy().Set(1000);  // Энергия
        stats.GetWater().Set(2000);  // Вода
    }

    void HealPlayer(float amount)
    {
        GetStatHealth().Add(amount);
    }
};
```

---

## 💡 Examples

### Example 1: Система стамины
→ [examples/playerbase_mod/](../examples/playerbase_mod/)

### Example 2: Кастомное событие

```c
modded class PlayerBase
{
    static ref ScriptInvoker m_OnPlayerDeath = new ScriptInvoker();

    override void OnDeath(PlayerBase killer)
    {
        super.OnDeath(killer);
        m_OnPlayerDeath.Invoke(this, killer);  // Вызвать событие
    }
};
```

### Example 3: Добавить действие

```c
modded class PlayerBase
{
    override void SetActions()
    {
        super.SetActions();
        AddAction(ActionMyCustomAction);  // Добавить кастомное действие
    }
};
```

---

## 🔗 Related Topics

→ [Actions: Создание действий](topics/03_actions.md)
→ [Events: ScriptInvoker](topics/07_events.md)

---

## 🔍 Common Tasks

### Как добавить новую переменную в PlayerBase?

```c
modded class PlayerBase
{
    private float m_MyVariable = 0.0;  // Новая переменная

    float GetMyVariable()
    {
        return m_MyVariable;
    }

    void SetMyVariable(float value)
    {
        m_MyVariable = value;
    }
};
```

---

### Как вызвать код каждый N секунд?

```c
modded class PlayerBase
{
    private float m_Timer = 0.0;
    private const float TIMER_INTERVAL = 5.0;  // Каждые 5 секунд

    override void OnUpdate(float delta_time)
    {
        super.OnUpdate(delta_time);

        m_Timer += delta_time;
        if (m_Timer >= TIMER_INTERVAL)
        {
            m_Timer = 0.0;
            // Выполнить код
            DoSomething();
        }
    }
};
```

---

### Как добавить эффект при низкой стамине?

```c
modded class PlayerBase
{
    override void OnUpdate(float delta_time)
    {
        super.OnUpdate(delta_time);

        if (m_Stamina < 20.0)  // Если стамина < 20
        {
            // Добавить эффект размытия
            GetSymptomManager().QueueUpPrimarySymptom(SymptomIDs.SYMPTOM_BLURRY_VISION);
        }
    }
};
```

---

## 🚨 Troubleshooting

**Ошибка:** "Stack level too deep"
- **Причина:** Бесконечная рекурсия (забыли `super()`)
- **Решение:** Всегда вызывайте `super.MethodName()` в переопределённых методах

**Проблема:** Низкий FPS
- **Причина:** Слишком много логики в `OnUpdate()`
- **Решение:** Используйте таймеры вместо выполнения каждый кадр

---

## 📖 Additional Resources

- [raw_data/PlayerBase.c](../raw_data/PlayerBase.c) - Полный класс PlayerBase
- [raw_data/Man.c](../raw_data/Man.c) - Базовый класс Man

---

## 🎓 Best Practices

1. **Всегда** вызывайте `super()` в переопределённых методах
2. **Будьте осторожны** с производительностью в `OnUpdate()`
3. **Используйте** таймеры для выполнения кода с интервалом
4. **Не используйте** `OnUpdate()` для тяжёлых вычислений
5. **Используйте** `ScriptInvoker` для событий вместо direct calls
