// EnergyDrink.c
// Consumable that boosts player stats

class EnergyDrink extends Drink_Base
{
    override void OnConsume(float amount, PlayerBase player)
    {
        super.OnConsume(amount, player);

        // Boost stamina
        player.GetStatStamina().Add(100);

        // Boost energy
        player.GetStatEnergy().Add(500);

        // Boost hydration
        player.GetStatWater().Add(300);

        Print("[EnergyDrink] Consumed by " + player.GetIdentity().GetName());

        // Schedule removal of boost after 5 minutes
        GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(RemoveBoost, 300000, false, player);
    }

    void RemoveBoost(PlayerBase player)
    {
        if (!player) return;

        // Reduce stats back to normal
        player.GetStatStamina().Add(-50);

        Print("[EnergyDrink] Boost expired for " + player.GetIdentity().GetName());
    }
}
