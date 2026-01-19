# COLLECTIONS AND TYPEDEFS - Массивы, Карты и Наборы

**Назначение:** Описание структур данных Enforce Script и стандартных псевдонимов типов (Typedefs).
**Источник:** Dayz Explorer (Enforce script essentials)

---

## 📦 Основные структуры данных

### array<Class T>
Динамический массив.
- **Пример:** `ref array<string> names = new array<string>;`
- **Методы:** `Insert()`, `Remove()`, `Get()`, `Count()`, `Clear()`.

### map<Class TKey, Class TValue>
Ассоциативный массив (словарь).
- **Пример:** `ref map<int, string> idToName = new map<int, string>;`
- **Методы:** `Insert()`, `Remove()`, `Contains()`, `Get()`, `Set()`.

### set<Class T>
Набор уникальных значений.
- **Пример:** `ref set<float> uniqueValues = new set<float>;`

---

## 🏷️ Стандартные Typedefs (Псевдонимы)

В коде DayZ принято использовать короткие имена для коллекций. Это делает код чище.

### Массивы (Arrays)
| Псевдоним | Оригинальный тип |
|-----------|------------------|
| `TStringArray` | `array<string>` |
| `TIntArray` | `array<int>` |
| `TFloatArray` | `array<float>` |
| `TBoolArray` | `array<bool>` |
| `TVectorArray` | `array<vector>` |
| `TClassArray` | `array<Class>` |
| `TManagedArray` | `array<Managed>` |
| `TManagedRefArray`| `array<ref Managed>` |

### Карты (Maps)
| Псевдоним | Ключ | Значение |
|-----------|------|----------|
| `TStringFloatMap` | `string` | `float` |
| `TStringIntMap` | `string` | `int` |
| `TStringStringMap`| `string` | `string` |
| `TIntStringMap` | `int` | `string` |
| `TIntFloatMap` | `int` | `float` |
| `TClassManagedMap`| `Class` | `Managed` |

---

## 🛠️ Функции для работы с массивами

### Sort(void param_array[], int num)
Сортирует статический массив (целые числа, числа с плавающей запятой, строки).
- **Пример:**
```c
string names[3] = {"Zebra", "Apple", "Car"};
Sort(names, 3); // Результат: Apple, Car, Zebra
```

### reversearray(void param_array)
Разворачивает массив в обратном порядке.

### copyarray(void destArray, void srcArray)
Копирует содержимое одного массива в другой.

---

## 🧩 Работа с итераторами (MapIterator)
Для обхода `map` используется `MapIterator` (фактически это `int`).

```c
MapIterator it = myMap.Begin();
while (it != myMap.End()) {
    string key = myMap.GetKey(it);
    float value = myMap.GetElement(it);
    // ...
    it = myMap.Next(it);
}
```

---

## 🔍 Советы
- Всегда инициализируйте коллекции через `new`, иначе вы получите ошибку доступа к памяти.
- Для массивов объектов используйте `ref array<ref MyClass>`, чтобы массив "владел" объектами и они не удалялись.
- Используйте `TStringArray` вместо `array<string>` — это стандарт индустрии DayZ моддинга.
