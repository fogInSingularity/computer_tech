#!/bin/bash  
# Указывает, что данный файл нужно интерпретировать с помощью bash

# Проверка количества аргументов, переданных скрипту
if [ $# -ge 0 ];  # Проверяет, передан ли хотя бы один аргумент ($# - число аргументов)
then
  disk="sda"  # Если аргументов нет, переменной disk присваивается значение "sda"
else
  disk=$1  # Если аргумент передан, то он используется в качестве имени диска
fi

# Выполнение команды iostat для сбора статистики по диску
String=$(iostat -dhx $disk 1 2)  # Команда iostat собирает расширенную статистику по диску $disk с задержкой 1 секунда, выполняется 2 раза

# Вывод последних 6 символов строки, содержащей результат команды iostat
echo ${String: -6}  # Обрезает и выводит последние 6 символов переменной String
