// ============================================================
// ATM ENTITY - Extended ATM object class
// ============================================================
// This class extends the basic ATM building with additional functionality
// Currently serves as a placeholder for future ATM features

class ATMEntity: BuildingBase
{
    // ============================================================
    // ATM STATE
    // ============================================================
    protected bool m_IsOnline;  // Is ATM operational
    protected float m_CashReserve;  // Physical cash reserve (optional feature)

    // ============================================================
    // CONSTRUCTOR - Initialize ATM
    // ============================================================
    void ATMEntity()
    {
        m_IsOnline = true;
        m_CashReserve = 100000;  // 100k cash reserve

        // Register ATM with BankingManager (optional)
        RegisterATM();
    }

    // ============================================================
    // DESTRUCTOR - Cleanup
    // ============================================================
    void ~ATMEntity()
    {
        // Unregister ATM (optional)
        UnregisterATM();
    }

    // ============================================================
    // REGISTER ATM - Register with banking system
    // ============================================================
    void RegisterATM()
    {
        // Placeholder for future features
        // Could be used for:
        // - ATM network tracking
        // - Location-based services
        // - Cash reserve management

        Print("[ATM System] ATM registered at position: " + GetPosition().ToString());
    }

    // ============================================================
    // UNREGISTER ATM - Unregister from banking system
    // ============================================================
    void UnregisterATM()
    {
        Print("[ATM System] ATM unregistered");
    }

    // ============================================================
    // IS ONLINE - Check if ATM is operational
    // ============================================================
    bool IsOnline()
    {
        return m_IsOnline;
    }

    // ============================================================
    // SET ONLINE - Set ATM operational status
    // ============================================================
    void SetOnline(bool online)
    {
        m_IsOnline = online;

        if (!online)
        {
            // Notify nearby players
            NotifyNearbyPlayers("ATM is currently offline. Please try another ATM.");
        }
    }

    // ============================================================
    // GET CASH RESERVE - Get available physical cash
    // ============================================================
    float GetCashReserve()
    {
        return m_CashReserve;
    }

    // ============================================================
    // SET CASH RESERVE - Set available physical cash
    // ============================================================
    void SetCashReserve(float reserve)
    {
        m_CashReserve = reserve;
    }

    // ============================================================
    // NOTIFY NEARBY PLAYERS - Send message to nearby players
    // ============================================================
    void NotifyNearbyPlayers(string message)
    {
        array<Man> players = new array<Man>;
        GetGame().GetPlayers(players);

        foreach (Man man : players)
        {
            PlayerBase player = PlayerBase.Cast(man);

            if (player)
            {
                float distance = vector.Distance(player.GetPosition(), GetPosition());

                if (distance <= 10)  // 10-meter radius
                {
                    NotificationSystem.Notify(player, message);
                }
            }
        }
    }

    // ============================================================
    // ON DAMAGE - Handle ATM damage
    // ============================================================
    override void EEHitBy(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
    {
        super.EEHitBy(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);

        // ATM cannot be damaged (defined in config.cpp)
        // This is just a safety check
        Print("[ATM System] ATM hit by damage source: " + source.ToString());
    }

    // ============================================================
    // ON UPDATE - Called every frame
    // ============================================================
    override void EOnFrame(IEntity other, float timeSlice)
    {
        super.EOnFrame(other, timeSlice);

        // Placeholder for future features:
        // - Animation (screen on/off)
        // - Sound effects
        // - Status indicators
    }
}
