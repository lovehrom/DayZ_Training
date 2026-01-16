# JSON Parsing - JsonFileLoader

**✅ ПРОВЕРЕНО:** DayZCommunityOfflineMode samples
**⚠️ ВНИМАНИЕ:** В raw_data не найдено, примеры из community samples

---

## Синтаксис

### Сохранить в JSON файл

```c
// ✅ ПРОВЕРЕНО: samples/DayZCommunityOfflineMode/Scripts/4_World/Entities/ManBase/PlayerBase/CharacterSave.c:62
JsonFileLoader<CharacterData>.JsonSaveFile(path, data);

// Пример:
JsonFileLoader<CharacterData>.JsonSaveFile(BASE_PLAYER_SAVE_DIR + "\\" + character + "\\" + save + ".json", oData);
```

### Загрузить из JSON файла

```c
// ✅ ПРОВЕРЕНО: samples/DayZCommunityOfflineMode/Scripts/4_World/Entities/ManBase/PlayerBase/CharacterLoad.c
JsonFileLoader<CharacterData>.JsonLoadFile(path, data);

// Пример:
JsonFileLoader<CharacterData>.JsonLoadFile(file, oData);
JsonFileLoader<SceneSaveST>.JsonLoadFile(BASE_SCENE_DIR + "\\" + "latest.json", scene);
```

---

## Template

```c
// Класс данных для сериализации
class PlayerData
{
    string playerName;
    vector position;
    float health;
}

// Сохранение
void SavePlayerData(string path, PlayerData data)
{
    JsonFileLoader<PlayerData>.JsonSaveFile(path, data);
}

// Загрузка
void LoadPlayerData(string path, out PlayerData data)
{
    if (FileExist(path))
    {
        JsonFileLoader<PlayerData>.JsonLoadFile(path, data);
    }
}
```

---

## Источники

- **samples/DayZCommunityOfflineMode/Scripts/4_World/Entities/ManBase/PlayerBase/CharacterSave.c** (строка 62)
- **samples/DayZCommunityOfflineMode/Scripts/4_World/Entities/ManBase/PlayerBase/CharacterLoad.c**

---

## ⚠️ Что НЕ найдено

- JsonFileLoader в **raw_data/** (игровых файлах) - не найдено
- Официальная документация по JsonFileLoader - не найдена
- Все примеры из community samples (требуется проверка на практике)

---

*Последнее обновление: 2025-01-16*
*Статус: Требует проверки на практике*
