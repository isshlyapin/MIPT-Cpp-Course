# Threaded Binary Tree (Прошитое AVL дерево)

## Описание

Реализация самобалансирующегося AVL-дерева с нитями (threads) для эффективного обхода в порядке inorder.

## Основная задача

Программа читает команды со стандартного ввода:

- `k X` - добавить ключ X в дерево
- `q L R` - запрос: сколько элементов в диапазоне [L, R]
  - L **включается** (Не меньше L)
  - R **включается** (Не меньше R)
  - Если R <= L, ответ 0

## Benchmarks 
```
------------------------------------------------------------------------------------
Benchmark                          Time             CPU   Iterations UserCounters...
------------------------------------------------------------------------------------
BM_MapInsert/10000           3168894 ns      3161857 ns          220 items_per_second=3.1627M/s
BM_TreeInsert/10000          5157638 ns      5152856 ns          136 items_per_second=1.94067M/s

BM_MapInsert/100000         48510721 ns     48380330 ns           14 items_per_second=2.06696M/s
BM_TreeInsert/100000        71536013 ns     71441905 ns            9 items_per_second=1.39974M/s

BM_MapInsert/1000000      1337412458 ns   1332627925 ns            1 items_per_second=750.397k/s
BM_TreeInsert/1000000     1520107040 ns   1517209127 ns            1 items_per_second=659.105k/s

BM_MapRangeQuery/10000       4328042 ns      4323197 ns          154 items_per_second=23.131k/s
BM_TreeRangeQuery/10000      2180980 ns      2178962 ns          317 items_per_second=45.8934k/s

BM_MapRangeQuery/100000    125255523 ns    125015286 ns            5 items_per_second=799.902/s
BM_TreeRangeQuery/100000    57429584 ns     57336722 ns           12 items_per_second=1.74408k/s

BM_MapRangeQuery/1000000  3610088656 ns   3604799974 ns            1 items_per_second=27.7408/s
BM_TreeRangeQuery/1000000 4019019204 ns   4013541875 ns            1 items_per_second=24.9156/s
```

Заметно преимущество std::map на вставке, т.к. в стандартной библиотеке используется черно-красное дерево и не надо следить за корректностью нитей

Но на **Range queries** задаче Threaded Binary Tree выигрывает за счет более быстрого inorder обхода по нитям

### Пример

**Входные данные:**
```
k 10 k 20 q 8 31 q 6 9 k 30 k 40 q 15 40
```

**Выходные данные:**
```
2 0 3
```

## Сборка
```bash
# только основная программа
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

```bash
# с тестами
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_TEST=ON
cmake --build build
```

```bash
# с benchmark
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_BENCHMARK=ON
cmake --build build
```

# Запуск
```bash
./build/rq
```

```bash
#Запуск тестов
ctest --test-dir build/test
```

```bash
#Запуск benchmark
./build/benchmark/benchmark 
```

## Автор

**Шляпин Илья**

- 📫[Email](mailto:shlyapin2005@gmail.com)
- 🌐[Telegram](https://t.me/isshlyapin)