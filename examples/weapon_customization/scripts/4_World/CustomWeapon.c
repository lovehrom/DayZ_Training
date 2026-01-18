// CustomWeapon.c
// Weapon with custom stats

class CustomAK extends Rifle_Base
{
    override float GetDamageSystem()
    {
        return 150; // Higher damage than vanilla (45)
    }

    override float GetReloadTimeModifier()
    {
        return 0.5; // 50% faster reload
    }

    override int GetReactionTimeout()
    {
        return 100; // Faster handling
    }

    override void OnFire(float bulletDamage, int damageType)
    {
        super.OnFire(bulletDamage, damageType);

        // Custom fire effect
        Print("[CustomWeapon] Fired with damage: " + bulletDamage);
    }
}
