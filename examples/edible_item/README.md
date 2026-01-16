# Edible Item Example

## Что это

Пример создания Edible предмета (еда) для DayZ.

## Как использовать

1. **Скопировать мод:**
   - Создайте `@EdibleItem/` рядом с `@DayZServer/`
   - Скопируйте файлы мода в `@EdibleItem/`

2. **Запустить сервер:**
   - Добавьте `-mod=@EdibleItem`
   - Запустите сервер

3. **Проверить в игре:**
   - Зайдите на сервер
   - Используйте `_test = "ApplePie"_spawn`
   - Съешьте пирог → проверьте здоровье

## Файлы

- `config.cpp` - Конфигурация с nutritional values
- `scripts/3_Game/Items/ApplePie.c` - Класс предмета

## Ключевые моменты

### Nutritional values (в config.cpp)
- `energy` - Калории
- `water` - Вода
- `nutritionalValue` - Пищевая ценность
- `toxicity` - Токсичность

### Temperature
- `temperatureThreshold` - При какой температуре замерзает
- `temperatureEnabled` - Включить температуру

### OnConsume()
- Вызывается когда игрок съедает предмет
- Можно добавить кастомную логику

## Связанные темы

→ [../../topics/01_items.md](../../topics/01_items.md) - Создание предметов
→ [../../topics/05_economy.md](../../topics/05_economy.md) - Настройка спавна

## Следующие шаги

- Измените nutritional values
- Добавите custom модель
- Добавьте custom иконку
