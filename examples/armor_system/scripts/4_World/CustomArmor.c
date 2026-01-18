// CustomArmor.c
// Armor with damage reduction

class CustomArmor extends Clothing
{
    override float GetDamageProtection(int damageType = -1)
    {
        return 0.5; // 50% damage reduction
    }

    override bool CanReceiveDamage(int damageType, EntityAI source)
    {
        // Check if armor has condition
        if (GetHealth01() > 0)
        {
            // Reduce damage to armor instead
            AddHealth(-("" + GetDamageProtection() * 10));
            return false;
        }
        return true;
    }

    override void EEHitBy(TotalDamageResult damageResult, int damageType, EntityAI source)
    {
        super.EEHitBy(damageResult, damageType, source);

        float damage = damageResult.GetDamage(damageType);

        // Reduce damage by armor
        float reducedDamage = damage * (1.0 - GetDamageProtection());

        // Apply reduced damage to player
        PlayerBase player = PlayerBase.Cast(this);
        if (player)
        {
            player.AddHealth("", "Health", -reducedDamage);
        }
    }
}
