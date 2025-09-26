# Сhecking triangle intersection

## 🛠 Сборка проекта

Требуется:  
- CMake (>= 3.30)  
- clang с поддержкой C++20
- Python3
- Ninja

Сборка:
1. Склонируйте репозиторий:
    ```bash
    git clone https://github.com/isshlyapin/MIPT-Cpp-Course.git
    ```
2. Перейдите в директорию проекта:
    ```bash
    cd MIPT-Cpp-Course/hw2_Triangles
    ```
3. Соберите проект
    ```bash
    cmake -S . -B build -G Ninja -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_BUILD_TYPE=Release
    ```
    ```bash
    cmake --build build
    ```

## 🚀 Использование
1. Запуск программы
    ```bash
    ./build/triangle_intersection
    ```
2. Запуск тестов
    ```bash
    ctest --test-dir build/tests
    ```

### Входные данные:
1. Кол-во треугольников
2. Координаты треугольников через пробел
3. Пример
   ```
    2

    2 0 0 
    0 1 0 
    0 0 1

    1 0 0
    0 2 0
    0 0 2
   ```
### Выходные данные
    Индексы треугольников имеющих пересечения (каждый с новой строки)
    Пример:
   ```
    0
    1
   ```