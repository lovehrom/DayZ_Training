// CustomZombie.c
// Zombie with custom stats and loot

class CustomZombie extends ZombieBase
{
    override void Init()
    {
        super.Init();

        // Set custom health (vanilla: 100)
        SetHealth("GlobalHealth", "Health", 500);

        Print("[CustomZombie] Spawned with 500 HP");
    }

    override float GetDamage()
    {
        return 100; // High damage (vanilla: ~20)
    }

    override void EEKilled(Object killer)
    {
        super.EEKilled(killer);

        // Drop loot on death
        vector position = GetPosition();
        GetGame().CreateObject("AK74", position);
        GetGame().CreateObject("Bandage", position);

        Print("[CustomZombie] Killed - Dropped loot");
    }

    override bool CanAttack(EntityAI target)
    {
        // Always attack (even if normally wouldn't)
        return true;
    }
}
