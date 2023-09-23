# Прогноз погоды.

## Источник данных

- [Open-Meteo](https://open-meteo.com/en/docs#latitude=59.94&longitude=30.31&hourly=temperature_2m&forecast_days=16) для прогноза
- [Api-Ninjas](https://api-ninjas.com/api/city) для определения координат по названию города

## Функциональные возможности

 - Отображать прогноз погоды на несколько дней вперед (значение по умолчанию задается конфигом)
 - Обновлять с некоторой частотой (задается конфигом)
 - Переключаться между городами с помощью клавиш "n", "p"
 - Заканчивать работу программы по Esc
 - Увеличивать\уменьшать количество дней прогноза по нажатие клавиш "+", "-"

Список городов, частота обновления, количество дней прогноза должны быть определены в *config.json*

Структура:
``` 
    {
        "cities": [[city_name, two_letter_country_name], ...],
        "forecast_days": int, // default = 7
        "update_frequency": int (in minutes) // default = 10
    }
```

## Реализация
Библиотека для запросов: [C++ Requests](https://github.com/libcpr/cpr)
Библиотека для парсинга json: [JSON for Modern C++](https://github.com/nlohmann/json)

