# Cache Simulator

Проект для исследования и сравнения алгоритмов кэширования (в том числе **LIRS** и **Belady**)
Реализация выполнена на C++ с использованием CMake и Conan для сборки и GoogleTest для тестирования.

## 📌 Описание

В данном проекте реализованы два алгоритма:

- **LIRS (Low Inter-reference Recency Set)** — современная политика замещения, эффективнее классического LRU  
- **Belady (Optimal / MIN)** — теоретически оптимальная политика, использующая знание будущих запросов. Применяется только для анализа, так как в реальности будущее неизвестно

## 🛠 Сборка проекта

Требуется:
- podman (для создания docker образа и запуска в нем проекта)

Сборка:
1. Склонируйте репозиторий:
    ```bash
    git clone https://github.com/isshlyapin/MIPT-Cpp-Course.git
    ```
2. Перейдите в директорию проекта:
    ```bash
    cd MIPT-Cpp-Course/hw1_Cache
    ```
3. Соберите образ с необходимым набором инструментов

    ```bash
    podman build -f Dockerfile -t <image_name>
    ```
4. Перейдите в созданый образ
    ```bash
    podman run -it --rm --mount type=bind,source=./,target=/usr/src/myapp <image_name> bash
    ```
5. Настройте conan
    ```bash
    uv run conan profile detect
    ```
6. Соберите проект
    ```bash
    uv run conan build . --build=missing -s build_type=Release
    ```

## 🚀 Использование
1. Запуск программы
    ```bash
    ./build/Release/cache
    ```
2. Запуск тестов
    ```bash
    ctest --test-dir build/Release/tests
    ```
### Входные данные:
1. Размер кэша
2. Кол-во запросов
3. Все запросы через пробел (целые числа)
4. Пример
   ```
   2 6 1 2 1 2 1 2
   ```