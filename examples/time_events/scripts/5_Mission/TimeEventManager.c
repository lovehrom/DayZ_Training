// TimeEventManager.c
// Time-based event system

class TimeEventManager
{
    protected float m_LastCheckTime;
    protected bool m_IsNight;

    void TimeEventManager()
    {
        m_LastCheckTime = 0;
        m_IsNight = false;
    }

    // Check time every minute
    void Update()
    {
        float gameTime = GetGame().GetWorld().GetWorldTime();

        // Only check once per minute
        if (Math.AbsFloat(gameTime - m_LastCheckTime) < 0.016)
        {
            return;
        }

        m_LastCheckTime = gameTime;

        // Night (20:00 - 06:00)
        if (gameTime >= 20.0 || gameTime < 6.0)
        {
            if (!m_IsNight)
            {
                OnNightStart();
                m_IsNight = true;
            }
        }
        else
        {
            if (m_IsNight)
            {
                OnDayStart();
                m_IsNight = false;
            }
        }

        // Dawn at 06:00
        if (gameTime >= 6.0 && gameTime < 6.1)
        {
            OnDawn();
        }

        // Dusk at 18:00
        if (gameTime >= 18.0 && gameTime < 18.1)
        {
            OnDusk();
        }
    }

    void OnNightStart()
    {
        Print("[TimeEvent] Night started - Spawning night mobs");
        // Spawn night-specific enemies
    }

    void OnDayStart()
    {
        Print("[TimeEvent] Day started - Despawning night mobs");
        // Despawn night enemies
    }

    void OnDawn()
    {
        Print("[TimeEvent] Dawn!");
        NotificationSystem.ServerMessage("Dawn has broken...");
    }

    void OnDusk()
    {
        Print("[TimeEvent] Dusk!");
        NotificationSystem.ServerMessage("Night is falling...");
    }

    // Check season
    int GetMonth()
    {
        int year, month, day, hour, minute;
        GetGame().GetWorld().GetDate(year, month, day, hour, minute);
        return month;
    }

    bool IsWinter()
    {
        int month = GetMonth();
        return (month == 12 || month == 1 || month == 2);
    }
}

// Global instance
ref TimeEventManager g_TimeEventManager;
