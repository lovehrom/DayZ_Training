class StoredItem
{
    string ClassName;
    float Health;
    float Quantity;

    // Для вложенных предметов (attachments, cargo)
    ref array<ref StoredItem> Attachments = new array<ref StoredItem>;
    ref array<ref StoredItem> Cargo = new array<ref StoredItem>;
}
