#!/usr/bin/env python3
"""
Генератор матриц NxN с заданным определителем.

Алгоритм:
1. Создаём диагональную матрицу с заданным определителем (произведение диагонали = det)
2. Применяем элементарные преобразования, сохраняющие определитель:
   - Прибавление к строке другой строки, умноженной на коэффициент
   - Перестановка строк (меняет знак определителя, компенсируем чётным числом)
3. Контролируем максимальный модуль элементов
"""

import numpy as np
import random
import argparse
from typing import Optional


def prime_factors(n: int) -> list[int]:
    """Возвращает список простых множителей числа."""
    factors = []
    d = 2
    while d * d <= n:
        while n % d == 0:
            factors.append(d)
            n //= d
        d += 1
    if n > 1:
        factors.append(n)
    return factors


def factorize_determinant(det: int, n: int) -> list[int]:
    """
    Разбивает определитель на n множителей для диагонали.
    Пытается сделать множители более равномерными.
    """
    if det == 0:
        # Если определитель 0, один из элементов должен быть 0
        factors = [1] * n
        factors[random.randint(0, n - 1)] = 0
        return factors

    sign = 1 if det > 0 else -1
    abs_det = abs(det)

    # Раскладываем на простые множители и распределяем по диагонали
    primes = prime_factors(abs_det)
    result = [1] * n
    
    # Распределяем простые множители равномерно
    for i, p in enumerate(primes):
        result[i % n] *= p
    
    # Применяем знак к первому элементу
    result[0] *= sign
    
    # Перемешиваем для разнообразия
    random.shuffle(result)
    
    return result


def generate_diagonal_matrix(det: int, n: int) -> np.ndarray:
    """Создаёт диагональную матрицу с заданным определителем."""
    factors = factorize_determinant(det, n)
    return np.diag(factors).astype(float)


def add_row_multiple(matrix: np.ndarray, src: int, dst: int, factor: float) -> None:
    """
    Прибавляет к строке dst строку src, умноженную на factor.
    Это преобразование НЕ меняет определитель.
    """
    matrix[dst] += factor * matrix[src]


def swap_rows(matrix: np.ndarray, i: int, j: int) -> None:
    """
    Меняет местами строки i и j.
    Это преобразование МЕНЯЕТ ЗНАК определителя.
    """
    matrix[[i, j]] = matrix[[j, i]]


def fill_zeros_systematically(matrix: np.ndarray) -> None:
    """
    Систематически заполняет нулевые элементы матрицы.
    Для каждого нулевого элемента (i, j) прибавляем к строке i 
    другую строку k (где элемент (k, j) ненулевой).
    """
    n = matrix.shape[0]
    
    # Несколько проходов для лучшего заполнения
    for _ in range(3):
        for i in range(n):
            for j in range(n):
                if matrix[i, j] == 0:
                    # Ищем строку с ненулевым элементом в столбце j
                    for k in range(n):
                        if k != i and matrix[k, j] != 0:
                            factor = random.choice([-1, 1])
                            add_row_multiple(matrix, k, i, factor)
                            break


def fill_matrix_dense(matrix: np.ndarray) -> None:
    """
    Заполняет матрицу, используя минимальное количество операций.
    Для диагональной матрицы: прибавляем первую строку ко всем остальным,
    затем первый столбец ко всем остальным - это даёт полностью заполненную матрицу.
    """
    n = matrix.shape[0]
    
    # Прибавляем первую строку ко всем остальным (заполняет первую строку во всех)
    for dst in range(1, n):
        factor = random.choice([-1, 1])
        add_row_multiple(matrix, 0, dst, factor)
    
    # Прибавляем первый столбец ко всем остальным (заполняет первый столбец)
    for dst in range(1, n):
        factor = random.choice([-1, 1])
        matrix[:, dst] += factor * matrix[:, 0]


def shuffle_and_mix(matrix: np.ndarray, num_ops: int) -> None:
    """
    Перемешивает матрицу: случайные перестановки строк (чётное число)
    и случайные прибавления.
    """
    n = matrix.shape[0]
    
    # Случайные прибавления строк
    for _ in range(num_ops):
        src, dst = random.sample(range(n), 2)
        factor = random.choice([-1, 1])
        add_row_multiple(matrix, src, dst, factor)
    
    # Случайные прибавления столбцов
    for _ in range(num_ops):
        src, dst = random.sample(range(n), 2)
        factor = random.choice([-1, 1])
        matrix[:, dst] += factor * matrix[:, src]
    
    # Чётное число перестановок строк
    num_swaps = (num_ops // 2) * 2
    for _ in range(num_swaps):
        i, j = random.sample(range(n), 2)
        swap_rows(matrix, i, j)


def clamp_matrix(matrix: np.ndarray, max_abs: float, det: int, tolerance: float = 1e-6) -> bool:
    """
    Проверяет, что все элементы по модулю не превышают max_abs.
    Возвращает True, если условие выполнено.
    """
    return bool(np.all(np.abs(matrix) <= max_abs + tolerance))


def generate_matrix(
    n: int,
    det: int,
    max_abs: float = 100.0,
    num_operations: Optional[int] = None,
    max_attempts: int = 100,
    seed: Optional[int] = None
) -> Optional[np.ndarray]:
    """
    Генерирует матрицу NxN с заданным определителем.
    
    Args:
        n: размер матрицы
        det: требуемый определитель
        max_abs: максимальный модуль элементов
        num_operations: количество случайных преобразований (по умолчанию n^2)
        max_attempts: максимальное число попыток генерации
        seed: зерно для генератора случайных чисел
    
    Returns:
        Матрица NxN или None, если не удалось сгенерировать за max_attempts попыток
    """
    if seed is not None:
        random.seed(seed)
        np.random.seed(seed)
    
    if num_operations is None:
        num_operations = n * n
    
    for attempt in range(max_attempts):
        # Создаём диагональную матрицу
        matrix = generate_diagonal_matrix(det, n)
        
        # Заполняем матрицу - прибавляем первую строку/столбец к остальным
        fill_matrix_dense(matrix)
        
        # Применяем случайные преобразования для разнообразия
        shuffle_and_mix(matrix, n)
        
        # Заполняем оставшиеся нули (если есть)
        fill_zeros_systematically(matrix)
        
        # Проверяем ограничение на модуль элементов
        if clamp_matrix(matrix, max_abs, det):
            # Проверяем, что определитель совпадает
            actual_det = np.linalg.det(matrix)
            if abs(actual_det - det) < 1e-6:
                # Округляем до целых, если близко
                matrix_int = np.round(matrix).astype(int)
                if abs(np.linalg.det(matrix_int) - det) < 1e-6:
                    return matrix_int
                return matrix
    
    return None


def generate_matrix_adaptive(
    n: int,
    det: int,
    max_abs: float = 100.0,
    seed: Optional[int] = None
) -> np.ndarray:
    """
    Адаптивная генерация матрицы - уменьшает число операций, если не получается.
    """
    if seed is not None:
        random.seed(seed)
        np.random.seed(seed)
    
    # Пробуем с разным количеством операций
    for num_ops in [n * n, n * n // 2, n * 2, n]:
        matrix = generate_matrix(n, det, max_abs, num_ops, max_attempts=25)
        if matrix is not None:
            return matrix
    
    # Если совсем не получается, возвращаем с минимальными преобразованиями
    matrix = generate_matrix(n, det, max_abs, n // 2, max_attempts=50)
    if matrix is not None:
        return matrix
    
    # В крайнем случае - просто диагональная матрица
    print(f"Warning: Could not generate matrix with max_abs={max_abs}, returning diagonal")
    return generate_diagonal_matrix(det, n)


def matrix_to_cpp(matrix: np.ndarray, var_name: str = "m") -> str:
    """Преобразует матрицу в C++ формат (vector<vector>)."""
    n = matrix.shape[0]
    lines = [f"// {n}x{n} matrix, det = {int(np.round(np.linalg.det(matrix)))}"]
    lines.append(f"std::vector<std::vector<double>> {var_name} = {{")
    
    for i, row in enumerate(matrix):
        row_str = ", ".join(f"{int(x):>4}" if x == int(x) else f"{x:>8.2f}" for x in row)
        comma = "," if i < n - 1 else ""
        lines.append(f"    {{{row_str}}}{comma}")
    
    lines.append("};")
    return "\n".join(lines)


def matrix_to_cpp_flat(matrix: np.ndarray, var_name: str = "m") -> str:
    """Преобразует матрицу в C++ формат (плоский vector, row-major)."""
    n = matrix.shape[0]
    flat = matrix.flatten()
    
    lines = [f"// {n}x{n} matrix, det = {int(np.round(np.linalg.det(matrix)))}"]
    lines.append(f"std::vector<double> {var_name} = {{")
    
    # Разбиваем на строки по n элементов для читаемости
    for i in range(n):
        start = i * n
        end = start + n
        row_elements = flat[start:end]
        row_str = ", ".join(f"{int(x):>4}" if x == int(x) else f"{x:>8.2f}" for x in row_elements)
        comma = "," if i < n - 1 else ""
        lines.append(f"    {row_str}{comma}")
    
    lines.append("};")
    return "\n".join(lines)


def matrix_to_plain(matrix: np.ndarray) -> str:
    """Преобразует матрицу в простой формат: N элементы_через_пробел."""
    n = matrix.shape[0]
    flat = matrix.flatten()
    elements_str = " ".join(str(int(x)) if x == int(x) else str(x) for x in flat)
    return f"{n} {elements_str}"


def print_matrix(matrix: np.ndarray) -> None:
    """Красиво печатает матрицу."""
    n = matrix.shape[0]
    print(f"\n{n}x{n} matrix, det = {int(np.round(np.linalg.det(matrix)))}")
    for row in matrix:
        print(" ".join(f"{int(x):>6}" if x == int(x) else f"{x:>8.2f}" for x in row))


def main():
    parser = argparse.ArgumentParser(
        description="Генератор матриц NxN с заданным определителем"
    )
    parser.add_argument("n", type=int, help="Размер матрицы")
    parser.add_argument("det", type=int, help="Требуемый определитель")
    parser.add_argument(
        "--max-abs", type=float, default=100.0,
        help="Максимальный модуль элементов (по умолчанию: 100)"
    )
    parser.add_argument(
        "--seed", type=int, default=None,
        help="Зерно для генератора случайных чисел"
    )
    parser.add_argument(
        "--cpp", action="store_true",
        help="Вывести в формате C++ (vector<vector>)"
    )
    parser.add_argument(
        "--cpp-flat", action="store_true",
        help="Вывести в формате C++ (плоский vector, row-major)"
    )
    parser.add_argument(
        "--plain", action="store_true",
        help="Вывести в простом формате: N элементы_через_пробел"
    )
    parser.add_argument(
        "--count", type=int, default=1,
        help="Количество матриц для генерации"
    )
    
    args = parser.parse_args()
    
    for i in range(args.count):
        seed = args.seed + i if args.seed is not None else None
        matrix = generate_matrix_adaptive(args.n, args.det, args.max_abs, seed)
        
        if args.cpp:
            var_name = f"m{i}" if args.count > 1 else "m"
            print(matrix_to_cpp(matrix, var_name))
            print()
        elif args.cpp_flat:
            var_name = f"m{i}" if args.count > 1 else "m"
            print(matrix_to_cpp_flat(matrix, var_name))
            print()
        elif args.plain:
            print(matrix_to_plain(matrix))
        else:
            print_matrix(matrix)
            print()


if __name__ == "__main__":
    main()
