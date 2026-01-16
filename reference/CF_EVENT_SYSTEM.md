# CF Event System - Community Framework Events

**Status:** ⚠️ Mod-derived (требует проверки)
**Source:** Workshop ID 1559212036 / Community Framework v0.0.0
**Related:** [Events](topics/07_events.md) | [ScriptInvoker](HOOK_POINTS.md)

---

## ⚡ 30-Second Summary

Community Framework предоставляет attribute-based event систему через `[CF_EventSubscriber]`, которая автоматически подписывает функции на события.

---

## 🎯 5-Minute Solution: Использовать CF Events

### Создание события

```c
// ✅ ПРОВЕРЕНО: CF/Scripts/2_GameLib/CommunityFramework/EventHandler/CF_EventHandler.c
class MyEvents
{
    // Создать event handler
    ref CF_EventHandler<MyEventArgs> MyEvent = new CF_EventHandler<MyEventArgs>();

    void TriggerMyEvent(string data)
    {
        // Trigger событие
        MyEvent.Invoke(this, new MyEventArgs(data));
    }
}
```

### Подписка на событие (attribute)

```c
// ✅ ПРОВЕРЕНО: CF/Scripts/2_GameLib/CommunityFramework/EventHandler/Attributes/CF_EventSubscriber.c:1-38
class MySubscriber
{
    // Подписаться на событие через attribute
    [CF_EventSubscriber(ScriptCaller.Create(OnMyEvent), myEvents.MyEvent)]
    void OnMyEvent(Class sender, CF_EventArgs args)
    {
        MyEventArgs myArgs = MyEventArgs.Cast(args);
        // Обработать событие
    }
}
```

### Ручная подписка

```c
class MySubscriber
{
    void SubscribeManually(CF_EventHandlerBase evt)
    {
        evt.AddSubscriber(ScriptCaller.Create(OnMyEvent));
    }

    void UnsubscribeManually(CF_EventHandlerBase evt)
    {
        evt.RemoveSubscriber(ScriptCaller.Create(OnMyEvent));
    }

    void OnMyEvent(Class sender, CF_EventArgs args)
    {
        // Handler
    }
}
```

---

## 📚 Deep Dive

### CF_EventHandler

**✅ ПРОВЕРЕНО:** CF/Scripts/2_GameLib/CommunityFramework/EventHandler/CF_EventHandler.c

```c
class CF_EventHandler<Class T = CF_EventArgs>
{
    // Подписать на событие
    void AddSubscriber(ScriptCaller subscriber);

    // Отписать от события
    void RemoveSubscriber(ScriptCaller subscriber);

    // Trigger событие
    void Invoke(Class sender, T args);

    // Очистить всех subscribers
    void Clear();

    // Количество subscribers
    int Count();
}
```

### CF_EventSubscriber Attribute

**✅ ПРОВЕРЕНО:** CF/Scripts/2_GameLib/CommunityFramework/EventHandler/Attributes/CF_EventSubscriber.c:1-28

```c
class CF_EventSubscriber
{
    /**
     * ATTRIBUTE Register a function as subscriber of up to 9 CF_EventHandler instances.
     *
     * @param subscriber    Function to register as subscriber.
     * @param evt1...9      CF_EventHandler instances to subscribe to.
     */
    void CF_EventSubscriber(
        ScriptCaller subscriber,
        CF_EventHandlerBase evt1 = NULL,
        CF_EventHandlerBase evt2 = NULL,
        CF_EventHandlerBase evt3 = NULL,
        CF_EventHandlerBase evt4 = NULL,
        CF_EventHandlerBase evt5 = NULL,
        CF_EventHandlerBase evt6 = NULL,
        CF_EventHandlerBase evt7 = NULL,
        CF_EventHandlerBase evt8 = NULL,
        CF_EventHandlerBase evt9 = NULL
    );
}
```

**Максимум 9 событий на один attribute!** Если нужно больше → используйте `CF_MultiEventSubscriber`.

### CF_MultiEventSubscriber Attribute

**✅ ПРОВЕРЕНО:** CF/Scripts/2_GameLib/CommunityFramework/EventHandler/Attributes/CF_MultiEventSubscriber.c

```c
class CF_MultiEventSubscriber
{
    /**
     * ATTRIBUTE Register a function as subscriber of unlimited CF_EventHandler instances.
     *
     * @param subscriber    Function to register as subscriber.
     * @param events        Array of CF_EventHandler instances.
     */
    void CF_MultiEventSubscriber(
        ScriptCaller subscriber,
        array<CF_EventHandlerBase> events
    );
}
```

### CF_EventArgs

**✅ ПРОВЕРЕНО:** CF/Scripts/1_Core/CommunityFramework/EventArgs/CF_EventArgs.c

```c
class CF_EventArgs
{
    // Базовый класс для всех event arguments
}
```

**Примеры EventArgs:**

```c
// ✅ ПРОВЕРЕНО: CF/Scripts/1_Core/CommunityFramework/EventArgs/
class CF_EventTimeArgs : CF_EventArgs
{
    float Delta;
}

class CF_EventUpdateArgs : CF_EventArgs
{
    float Delta;
}
```

### Lifecycle Events

**✅ ПРОВЕРЕНО:** CF/Scripts/2_GameLib/CommunityFramework/LifecycleEvents/CF_LifecycleEvents.c

```c
class CF_LifecycleEvents
{
    static ref CF_EventHandler<CF_EventArgs> OnInit = new CF_EventHandler<CF_EventArgs>();
    static ref CF_EventHandler<CF_EventArgs> OnDestroy = new CF_EventHandler<CF_EventArgs>();
}
```

**Использование:**

```c
// Подписаться на OnInit
[CF_EventSubscriber(ScriptCaller.Create(OnInit), CF_LifecycleEvents.OnInit)]
void OnInit(Class sender, CF_EventArgs args)
{
    // Инициализация
}
```

---

## 💡 Examples

### Example 1: Custom Event

```c
// Создать custom EventArgs
class MyEventArgs : CF_EventArgs
{
    string Message;
    int Value;

    void MyEventArgs(string msg, int val)
    {
        Message = msg;
        Value = val;
    }
}

// Создать класс с событием
class EventPublisher
{
    ref CF_EventHandler<MyEventArgs> OnSomethingHappened = new CF_EventHandler<MyEventArgs>();

    void DoSomething()
    {
        // Trigger событие
        OnSomethingHappened.Invoke(this, new MyEventArgs("Hello", 42));
    }
}

// Подписаться на событие
class EventSubscriber
{
    ref EventPublisher m_Publisher;

    void EventSubscriber(EventPublisher publisher)
    {
        m_Publisher = publisher;

        // Подписаться через attribute
        // [CF_EventSubscriber(ScriptCaller.Create(OnSomethingHappened), m_Publisher.OnSomethingHappened)]
    }

    void OnSomethingHappened(Class sender, CF_EventArgs args)
    {
        MyEventArgs myArgs = MyEventArgs.Cast(args);
        Print(myArgs.Message);  // "Hello"
        Print(myArgs.Value);    // 42
    }
}
```

### Example 2: Multiple Events (up to 9)

```c
class MultiSubscriber
{
    ref CF_EventHandler<CF_EventArgs> Event1;
    ref CF_EventHandler<CF_EventArgs> Event2;
    ref CF_EventHandler<CF_EventArgs> Event3;
    // ... до 9 событий

    [CF_EventSubscriber(
        ScriptCaller.Create(OnEvent),
        Event1, Event2, Event3  // до 9
    )]
    void OnEvent(Class sender, CF_EventArgs args)
    {
        // Общий handler для всех событий
    }
}
```

### Example 3: Unlimited Events (MultiEventSubscriber)

```c
class UnlimitedSubscriber
{
    array<ref CF_EventHandlerBase> m_Events = new array<ref CF_EventHandlerBase>();

    void SubscribeAll()
    {
        for (int i = 0; i < m_Events.Count(); i++)
        {
            m_Events[i] = new CF_EventHandler<CF_EventArgs>();
        }

        // Подписаться на все события
        [CF_MultiEventSubscriber(ScriptCaller.Create(OnEvent), m_Events)]
    }

    void OnEvent(Class sender, CF_EventArgs args)
    {
        // Handler для всех событий
    }
}
```

### Example 4: Manual Subscription

```c
class ManualSubscriber
{
    ref CF_EventHandler<CF_EventArgs> m_Event;
    ScriptCaller m_Handler;

    void ManualSubscriber()
    {
        m_Event = new CF_EventHandler<CF_EventArgs>();
        m_Handler = ScriptCaller.Create(OnEvent);

        // Ручная подписка
        m_Event.AddSubscriber(m_Handler);
    }

    void ~ManualSubscriber()
    {
        // Отписать при уничтожении
        if (m_Event)
            m_Event.RemoveSubscriber(m_Handler);
    }

    void OnEvent(Class sender, CF_EventArgs args)
    {
        // Handler
    }
}
```

---

## 🔗 Related Topics

→ [Events](topics/07_events.md) - ScriptInvoker
→ [Hook Points](HOOK_POINTS.md) - modded class паттерны

---

## ⚠️ SOURCE VERIFICATION

**Статус:** ⚠️ Mod-derived (Community Framework)

**Что проверено:**
- ✅ CF_EventHandler.c - базовый event handler
- ✅ CF_EventSubscriber.c:1-38 - attribute для подписки
- ✅ CF_MultiEventSubscriber.c - attribute для множественной подписки
- ✅ CF_LifecycleEvents.c - lifecycle события
- ✅ CF_EventArgs.c - базовый класс аргументов

**Что НЕ проверено:**
- ❌ Работает ли без CF мода
- ❌ Performance impact
- ❌ Memory leaks при неправильном использовании

---

## 🚨 Troubleshooting

**Ошибка:** "Attribute не работает"
- **Причина:** Attributes работают только при загрузке скрипта
- **Решение:** Используйте в определении класса, не в runtime

**Ошибка:** "Event не вызывается"
- **Причина:** Забыли invoke или подписчик не добавлен
- **Решение:** Проверьте `.Invoke()` и `.AddSubscriber()`

**Ошибка:** "Более 9 событий"
- **Причина:** CF_EventSubscriber поддерживает только до 9
- **Решение:** Используйте CF_MultiEventSubscriber

---

*Последнее обновление: 2025-01-16*
*Источник: Community Framework v0.0.0 (Workshop ID: 1559212036)*
