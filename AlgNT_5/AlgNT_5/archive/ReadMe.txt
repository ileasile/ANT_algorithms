﻿AlgNT_5.cpp - тестирующая система; код, запускающий необходимые функции библиотеки NTL. 
Измерения времени записываются в папку tests

test/make_plots.gp - скрипт gnuplot для построения графиков
Также в папке test находятся csv-файлы отчётов тестов на время (проверка на простоту и генерация простого числа) и графики
_get_cpu_time.cpp - функция, кроссплатформенно возвращающая более или менее точное время

Замечания. Пути заголовочных файлов могут быть некорректными, при компиляции их следует поменять
           На графиках на оси абцисс отложены целочисленные двоичные логарифмы от генерируемых/тестируемых чисел, 
           на оси ординат - время в секундах.