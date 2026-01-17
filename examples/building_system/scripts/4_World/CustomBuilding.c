// CustomBuilding.c
// Custom building with codelock support

class CustomBuilding extends House
{
    override void OnPlacementComplete(Man player)
    {
        super.OnPlacementComplete(player);

        // Set health
        SetHealth("GlobalHealth", "Health", 500);

        Print("[Building] Built at " + GetPosition().ToString());
    }

    // Codelock support
    override bool CanUseConstruction()
    {
        if (HasCodelock())
        {
            return HasCodelockAccess();
        }
        return true;
    }

    // Check if player has access
    bool HasCodelockAccess()
    {
        // Implement codelock check
        return true;
    }
}
