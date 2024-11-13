#!/bin/bash  

# Запуск сhmod +x script1.sh ; ./script1.sh 2 2

#Указывает, что данный файл нужно интерпретировать с помощью bash

# echo   "Кису любишь!"  # Закомментированная строка, которая выводила бы текст "Кису любишь!"

KISA=Киса  # Переменной KISA присваивается значение "Киса"

echo $KISA  # Выводит значение переменной KISA

echo $RANDOM  # Выводит случайное число, генерируемое встроенной переменной RANDOM

# Создание и переход в каталог "test"
mkdir test  # Создает директорию "test"
cd test     # Переходит в директорию "test"

n=0  # Инициализация переменной n для подсчета файлов

# Внешний цикл создает заданное количество директорий "level1"
for ((i = 1; i <= $1; i++))  # Цикл выполняется $1 раз (первый параметр)
do
    mkdir level1.$i  # Создает директорию "level1.i" для каждого значения i
    
    # Вложенный цикл создает заданное количество поддиректорий "level2" в каждой "level1"
    for ((j = 1; j <= $2; j++))  # Цикл выполняется $2 раз (второй параметр)
    do
        mkdir level1.$i/level2.$j  # Создает поддиректорию "level2.j" в текущей "level1.i"
        
        n=$((n+1))  # Увеличивает счетчик n

        # Создает текстовый файл внутри текущей поддиректории "level2" и записывает в него строку
        touch level1.$i/level2.$j/TextFile$n  # Создает файл "TextFilen" в текущей поддиректории
        echo "This is file $n" > level1.$i/level2.$j/TextFile$n  # Записывает текст в файл
        echo $n  # Выводит значение n
    done
done

# Обратный вывод содержимого каждого файла
for f in `find . -type f`  # Ищет все файлы в текущей и вложенных директориях
do
  rev $f > $f  # Инвертирует содержимое каждого файла, перезаписывая его
done

