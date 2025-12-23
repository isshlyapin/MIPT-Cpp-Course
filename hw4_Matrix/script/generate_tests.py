#!/usr/bin/env python3
"""
Скрипт для генерации тестового набора для программы det.

Использует matrix_gen.py для генерации матриц с известным определителем.
Новые тесты добавляются к существующим (автоматическая нумерация).

Примеры:
    # Генерация 10 тестов с матрицами случайных размеров
    uv run script/generate_tests.py --count 10

    # Генерация тестов для конкретных размеров
    uv run script/generate_tests.py --sizes 2 3 5 10 50 100

    # Генерация с заданным диапазоном определителей
    uv run script/generate_tests.py --count 5 --det-range -1000 1000

    # Очистить существующие тесты и сгенерировать новые
    uv run script/generate_tests.py --count 10 --clean
"""

import sys
import random
import argparse
from pathlib import Path

# Добавляем директорию скриптов в путь для импорта matrix_gen
SCRIPT_ROOT = Path(__file__).parent
sys.path.insert(0, str(SCRIPT_ROOT))

from matrix_gen import generate_matrix_adaptive, matrix_to_plain

# Директория для тестовых данных
DATA_DIR = Path(__file__).parent.parent / "test" / "data"


def generate_test(
    test_num: int,
    n: int,
    det: int,
    max_abs: float = 100.0,
    seed: int | None = None
) -> bool:
    """
    Генерирует один тестовый файл.
    
    Args:
        test_num: номер теста (для имени файла)
        n: размер матрицы
        det: требуемый определитель
        max_abs: максимальный модуль элементов
        seed: зерно для генератора
    
    Returns:
        True если тест успешно создан
    """
    DATA_DIR.mkdir(parents=True, exist_ok=True)
    
    # Генерируем матрицу
    matrix = generate_matrix_adaptive(n, det, max_abs, seed)
    
    if matrix is None:
        print(f"Warning: Could not generate matrix for test {test_num}")
        return False
    
    # Формируем входные данные
    input_data = matrix_to_plain(matrix)
    
    # Записываем файлы
    input_file = DATA_DIR / f"{test_num}.in"
    output_file = DATA_DIR / f"{test_num}.out"
    
    input_file.write_text(input_data + "\n")
    output_file.write_text(str(det) + "\n")
    
    print(f"Generated test {test_num}: {n}x{n} matrix, det = {det}")
    return True


def generate_test_suite(
    count: int = 10,
    sizes: list[int] | None = None,
    det_range: tuple[int, int] = (-10000, 10000),
    max_abs: float = 100.0,
    seed: int | None = None,
    start_num: int = 1
) -> int:
    """
    Генерирует набор тестов.
    
    Args:
        count: количество тестов
        sizes: список размеров матриц (если None, используется случайный)
        det_range: диапазон определителей (min, max)
        max_abs: максимальный модуль элементов
        seed: начальное зерно для генератора
        start_num: начальный номер теста
    
    Returns:
        Количество успешно созданных тестов
    """
    if seed is not None:
        random.seed(seed)
    
    generated = 0
    
    for i in range(count):
        test_num = start_num + i
        
        # Определяем размер матрицы
        if sizes:
            n = sizes[i % len(sizes)]
        else:
            # Случайный размер с предпочтением небольших матриц
            n = random.choices(
                population=[2, 3, 4, 5, 10, 20, 50, 100],
                weights=[20, 20, 15, 15, 10, 10, 5, 5],
                k=1
            )[0]
        
        # Случайный определитель
        det = random.randint(det_range[0], det_range[1])
        
        # Генерируем тест
        test_seed = seed + i if seed is not None else None
        if generate_test(test_num, n, det, max_abs, test_seed):
            generated += 1
    
    return generated


def clean_tests():
    """Удаляет все существующие тесты."""
    if not DATA_DIR.exists():
        return
    
    for f in DATA_DIR.glob("*.in"):
        f.unlink()
    for f in DATA_DIR.glob("*.out"):
        f.unlink()
    
    print("Cleaned existing tests")


def get_next_test_num() -> int:
    """Получает следующий свободный номер теста."""
    if not DATA_DIR.exists():
        return 1
    
    existing = []
    for f in DATA_DIR.glob("*.in"):
        try:
            existing.append(int(f.stem))
        except ValueError:
            pass
    
    return max(existing, default=0) + 1


def main():
    parser = argparse.ArgumentParser(
        description="Генератор тестового набора для программы det"
    )
    parser.add_argument(
        "--count", type=int, default=10,
        help="Количество тестов для генерации (по умолчанию: 10)"
    )
    parser.add_argument(
        "--sizes", type=int, nargs="+",
        help="Список размеров матриц (например: 2 3 5 10 50)"
    )
    parser.add_argument(
        "--det-range", type=int, nargs=2, default=[-1000, 1000],
        metavar=("MIN", "MAX"),
        help="Диапазон определителей (по умолчанию: -10000 10000)"
    )
    parser.add_argument(
        "--max-abs", type=float, default=10000.0,
        help="Максимальный модуль элементов матрицы (по умолчанию: 100)"
    )
    parser.add_argument(
        "--seed", type=int, default=42,
        help="Зерно для генератора случайных чисел"
    )
    parser.add_argument(
        "--clean", action="store_true",
        help="Очистить существующие тесты перед генерацией"
    )
    
    args = parser.parse_args()
    
    if args.clean:
        clean_tests()
    
    # По умолчанию добавляем к существующим тестам
    start_num = get_next_test_num()
    
    generated = generate_test_suite(
        count=args.count,
        sizes=args.sizes,
        det_range=tuple(args.det_range),
        max_abs=args.max_abs,
        seed=args.seed,
        start_num=start_num
    )
    
    print(f"\nGenerated {generated}/{args.count} tests in {DATA_DIR}")


if __name__ == "__main__":
    main()
