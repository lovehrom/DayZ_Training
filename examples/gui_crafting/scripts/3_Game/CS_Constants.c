// CS_Constants.c
// RPC IDs, sound paths, notification icons

class CS_Constants
{
    // RPC IDs (must match client/server)
    static const int RPC_SEND_RECIPES = 556677;  // Server → Client: send recipes
    static const int RPC_REQUEST_CRAFT = 556678; // Client → Server: craft request
    static const int RPC_CRAFT_RESULT = 556679;  // Server → Client: craft result

    // Notification system
    static const string NOTIFICATION_ICON = "CraftSystem\\Scripts\\GUI\\icon.edds";
    static const string NOTIFICATION_TITLE = "Craft System";

    // Sound effect
    static const string SOUND_CRAFT = "CS_Craft_SoundSet";
}
