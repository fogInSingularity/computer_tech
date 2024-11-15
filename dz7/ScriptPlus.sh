#!/bin/bash  
# Указывает, что данный файл нужно интерпретировать с помощью bash

# Инициализация переменной tl со значением '|' для формирования отступов
tl='|'

# Цикл создает линию с отступом из символов "-".
for ((i=$2; i > 0; i--))  # Выполняется $2 раз, где $2 — это второй аргумент, задающий уровень вложенности
do
    tl="$tl-"  # Добавляет символ "-" к переменной tl
done

# Сохраняем список файлов и папок в переменной line
line=$(ls "$1")  # Выводит содержимое директории, указанной первым аргументом $1, и сохраняет в переменную line

# Цикл для обработки каждого элемента в переменной line
for word in $line  # Перебирает каждый элемент (файл или папку) в списке line
do		
    # Убираем запятую в конце (если есть)
    word=${word%,}
    
    # Выводим текущий элемент с отступом tl
    echo "$tl$word"
    
    # Проверяем, является ли текущий элемент директорией и если уровень вложенности $2 больше 0
    if [ "$2" -gt 0 ] && [ -d "$1/$word" ]; then 
        nextlev=$(($2 - 1))  # Уменьшаем уровень вложенности
        # Рекурсивно вызываем скрипт для вложенной директории с уменьшенным уровнем вложенности
        bash "$0" "$1/$word" "$nextlev"
    fi
done

