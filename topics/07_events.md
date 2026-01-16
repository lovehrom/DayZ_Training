# Events: ScriptInvoker и события

**Aliases:** events | события | ScriptInvoker | OnPlayerSpawned | подписка на события
**Related:** [PlayerBase](topics/04_playerbase.md)

---

## ⚡ 30-Second Summary

ScriptInvoker - система событий в DayZ для подписки и оповещения.

Чтобы создать событие:
1. Создать `static ref ScriptInvoker` в классе
2. Вызвать `Invoke()` для оповещения подписчиков
3. Подписаться на событие через `Insert()`

---

## 🎯 5-Minute Solution: Создать событие смерти игрока

### Шаг 1: Создать ScriptInvoker в PlayerBase

```c
modded class PlayerBase
{
    // Создать ScriptInvoker (статический)
    static ref ScriptInvoker m_OnPlayerDeath = new ScriptInvoker();

    override void OnDeath(PlayerBase killer)
    {
        super.OnDeath(killer);

        // Вызвать событие
        m_OnPlayerDeath.Invoke(this, killer);
    }
};
```

### Шаг 2: Подписаться на событие

```c
class MyEventManager
{
    void MyEventManager()
    {
        // Подписаться на событие смерти
        PlayerBase.m_OnPlayerDeath.Insert(OnPlayerDeathHandler);
    }

    void OnPlayerDeathHandler(PlayerBase player, PlayerBase killer)
    {
        // Обработка события
        Print("Player " + player + " killed by " + killer);
    }
};
```

---

## 📚 Deep Dive

### ScriptInvoker - что это?

**ScriptInvoker** = список callback-функций

**Принцип работы:**
1. Создаёте ScriptInvoker
2. Подписываете callback-функции через `Insert()`
3. Вызываете `Invoke()` → все callbacks выполняются

---

### Создание события

```c
class MyClass
{
    // 1. Создать ScriptInvoker
    static ref ScriptInvoker m_OnMyEvent = new ScriptInvoker();

    void DoSomething()
    {
        // ... какая-то логика ...

        // 2. Вызвать событие
        m_OnMyEvent.Invoke(this, someData);
    }
};
```

---

### Подписка на событие

```c
class MyListener
{
    void MyListener()
    {
        // Подписаться
        MyClass.m_OnMyEvent.Insert(OnMyEventHandler);
    }

    void OnMyEventHandler(MyClass sender, MyData data)
    {
        // Обработка события
    }

    ~MyListener()
    {
        // Отписаться (деструктор)
        MyClass.m_OnMyEvent.Remove(OnMyEventHandler);
    }
};
```

---

### Стандартные события PlayerBase

DayZ уже имеет встроенные события:

| Событие | Когда вызывается |
|---------|------------------|
| `OnPlayerSpawned` | При спавне игрока |
| `OnDeath` | При смерти |
| `OnConnect` | При подключении к серверу |
| `OnDisconnect` | При отключении |

---

## 💡 Examples

### Example 1: Событие спавна игрока

```c
modded class PlayerBase
{
    static ref ScriptInvoker m_OnPlayerSpawned = new ScriptInvoker();

    override void OnConnect()
    {
        super.OnConnect();
        m_OnPlayerSpawned.Invoke(this);
    }
};
```

### Example 2: Система логирования

```c
modded class PlayerBase
{
    static ref ScriptInvoker m_OnItemConsumed = new ScriptInvoker();

    override void OnConsume(ItemBase item)
    {
        super.OnConsume(item);
        m_OnItemConsumed.Invoke(this, item);
    }
};

class Logger
{
    void Logger()
    {
        PlayerBase.m_OnItemConsumed.Insert(OnItemConsumedHandler);
    }

    void OnItemConsumedHandler(PlayerBase player, ItemBase item)
    {
        Print("Player consumed: " + item.GetType());
    }
};
```

---

## 🔗 Related Topics

→ [PlayerBase: Модификация игрока](topics/04_playerbase.md)

---

## 🔍 Common Tasks

### Как создать событие с параметрами?

```c
static ref ScriptInvoker m_OnMyEvent = new ScriptInvoker();

// Вызвать событие с параметрами
m_OnMyEvent.Invoke(param1, param2, param3);

// Подписаться
m_OnMyEvent.Insert(voidHandler(ClassType param1, ClassType param2, ClassType param3));
```

---

### Как отписаться от события?

```c
// Вариант 1: Удалить конкретный callback
m_OnMyEvent.Remove(MyCallback);

// Вариант 2: Очистить все
m_OnMyEvent.Clear();
```

---

### Как проверить что есть подписчики?

```c
if (m_OnMyEvent != null && m_OnMyEvent.Count() > 0)
{
    m_OnMyEvent.Invoke(...);
}
```

---

## 🚨 Troubleshooting

**Проблема:** Callback не вызывается
- **Решение:** Проверьте что вы подписались (`Insert()`)

**Проблема:** Ошибка "Null pointer"
- **Решение:** Проверьте что ScriptInvoker создан (`new ScriptInvoker()`)

---

## 📖 Additional Resources

- [raw_data/PlayerBase.c](../raw_data/PlayerBase.c) - Стандартные события

---

## 🎓 Best Practices

1. **Используйте** `static ref ScriptInvoker` для глобальных событий
2. **Отписывайтесь** в деструкторе (`~ClassName()`)
3. **Проверяйте** `null` перед вызовом
4. **Используйте** события для развязки компонентов
5. **Документируйте** параметры событий
