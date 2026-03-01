# Bitonic Sort (OpenCL)

Сортировка битоническим алгоритмом на GPU через OpenCL.
Реализация содержит:

- `OCLBitonicSorter<T>`: сортировка массива размера $2^k$ (контейнер с contiguous-итераторами).
- Два ядра: локальная стадия (использует local memory) и глобальная стадия.
- CLI-утилиты:
    - `bitonic_verify` — читает числа из stdin, сортирует и печатает отсортированный массив.
    - `bitonic_benchmark_random` — бенчмарк на случайном входе (печатает тайминги).
    - `bitonic_benchmark_stdin` — бенчмарк на входе из stdin (печатает тайминги).
- Набор unit-тестов на GoogleTest (опционально) + скрипт на Python для запуска e2e тестов.

## Требования

- CMake `>= 3.20`
- Компилятор C++20
- OpenCL SDK/headers + ICD loader (в CMake используется `find_package(OpenCL REQUIRED)`)
- python (для e2e-тестов)
- Для тестов: GoogleTest (`find_package(GTest REQUIRED)`)

Важно: среда `OCLSimpleBitonicEnv` выбирает первую OpenCL-платформу с GPU-девайсами. Если на машине нет доступного GPU OpenCL (или он недоступен через драйвер), запуск `bitonic_...` и тесты могут падать на инициализации/сборке программы.

## Сборка

Минимальная сборка (Release):

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

После этого будут собраны бинарники:

- `./build/bitonic_verify`
- `./build/bitonic_benchmark_random`
- `./build/bitonic_benchmark_stdin`

Можно собирать конкретный таргет:

```bash
cmake --build build -j --target bitonic_verify
```

## Опции CMake

Все опции задаются при конфигурации CMake:

- `-DANALYZE=ON|OFF` (по умолчанию `OFF`) — включает отладочный вывод.
- `-DCOMPARE_CPU=ON|OFF` (по умолчанию `OFF`) — дополнительно печатает время CPU-сортировки (для сравнения/бенчмарка).
- `-DTYPE=int|float|double` (по умолчанию `int`) — тип элементов, для которого собирается `bitonic`.
- `-DBUILD_TEST=ON|OFF` (по умолчанию `OFF`) — собирать тесты из `test/`.

## Запуск программы

CLI-аргументы (см. `Config::read`):

- `--size N` — число элементов (по умолчанию `1024`). Должно быть степенью двойки, иначе будет `RUNTIME ERROR: Size must be a power of 2`.
- `--lsize L` — параметр local size (по умолчанию `256`). Передаётся в `OCLBitonicSorter` и используется при сборке OpenCL-программы как `-DLSZ=L`.

Ограничения на `--lsize`:

- `L > 1`
- `L * sizeof(TYPE) <= local_mem_size` устройства

Примеры:

```bash
./build/bitonic_verify --size 8 --lsize 8 < test/data/3.in
./build/bitonic_verify --size 32 --lsize 256 < test/data/5.in
```

Вывод `bitonic_verify`: отсортированные числа в одну строку.

Вывод бенчмарков (`bitonic_benchmark_random` / `bitonic_benchmark_stdin`):

- `GPU wall time measured: ... ns` — время «снаружи» (запуск + ожидание).
- `GPU pure time measured: ... ns` — время между OpenCL events (профилирование очереди).
- При `-DCOMPARE_CPU=ON`: `CPU time measured: ... ns`.

## Тесты

### E2E (корректность сортировки)

Собрать `bitonic_verify` и прогнать тесты из `test/data/*.in`:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j --target bitonic_verify
python3 test/test_e2e.py
```

### Unit (GoogleTest)

Сборка тестов:

```bash
cmake -S . -B build -DBUILD_TEST=ON -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

Прогон:

```bash
ctest --test-dir build/test --output-on-failure
```


## Benchmark report

Ниже — сохранённый отчёт с результатами на конкретных платформах.

### Platform: NVIDIA CUDA

- **Platform:** NVIDIA CUDA (OpenCL 3.0, CUDA 12.2.149)
- **Device:** NVIDIA GeForce RTX 4090
    - Local mem: 48 KiB
    - Global mem: 24 GiB
    - Compute units: 128

---

#### Performance Results (LSize = 256)

| Array Size | CPU Time  | GPU Time *(copy+kernel)*  | GPU Speedup |
| ---------- | --------- | ------------------------- | ----------- |
| 32  KB     | 161.47 µs | 315.49 µs                 | 0.51×       |
| 64  KB     | 247.52 µs | 493.28 µs                 | 0.50×       |
| 128 KB     | 519.06 µs | 455.62 µs                 | 1.14×       |
| 256 KB     | 1149   µs | 538.82 µs                 | 2.13×       |
| 512 KB     | 2417   µs | 894.18 µs                 | 2.70×       |
| 1   MB     | 5.157  ms | 1.542  ms                 | 3.34×       |
| 2   MB     | 11.06  ms | 2.934  ms                 | 3.77×       |
| 4   MB     | 22.80  ms | 4.540  ms                 | **5.02×**   |
| 8   MB     | 48.49  ms | 12.49  ms                 | 3.88×       |
| 16  MB     | 101.42 ms | 24.62  ms                 | 4.12×       |
| 32  MB     | 216.02 ms | 69.68  ms                 | 3.10×       |

---

#### Observations

* До **128 KB** накладные расходы GPU (kernel launch + memory transfer) превышают выигрыш от параллелизма
* Начиная с **256 KB** GPU стабильно быстрее CPU

---

### Platform: rusticl (Mesa/X.org)

- **Platform:** rusticl (Mesa/X.org) (OpenCL 3.0)
- **Device:** Mesa Intel(R) Graphics (ADL GT2)
    - Local Memory: 64 KiB
    - Global Memory: 8 GiB
    - Compute Units: 4

---

#### Performance Results (LSize = 256)

| Array Size | CPU Time (ms) | GPU Time *(copy+kernel)* (ms) | GPU Speedup |
| ---------- | ------------- | ------------- | ----------- |
| 32 KB      | 0.348         | 11.949        | 0.03×       |
| 64 KB      | 0.929         | 20.624        | 0.05×       |
| 128 KB     | 1.217         | 33.338        | 0.04×       |
| 256 KB     | 3.807         | 44.716        | 0.09×       |
| 512 KB     | 3.327         | 74.763        | 0.04×       |
| 1 MB       | 6.958         | 89.780        | 0.08×       |
| 2 MB       | 23.100        | 125.960       | 0.18×       |
| 4 MB       | 29.490        | 237.390       | 0.12×       |
| 8 MB       | 59.930        | 514.850       | 0.12×       |
| 16 MB      | 153.470       | 1353.990      | 0.11×       |
| 32 MB      | 315.200       | 2925.160      | 0.11×       |

---

#### Краткий вывод

* GPU стабильно уступает CPU на всех размерах.
* При 2–32 MB GPU медленнее примерно в 9–10×.
