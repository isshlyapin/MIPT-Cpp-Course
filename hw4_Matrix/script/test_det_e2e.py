#!/usr/bin/env python3
"""
E2E тесты для программы det.

Читает входные данные из файлов test/data/{n}.in
Сравнивает результат с ожидаемым из test/data/{n}.out

Запуск:
    uv run pytest script/test_det_e2e.py -v
"""

import subprocess
import pytest
from pathlib import Path


# Путь к директории с тестовыми данными
SCRIPT_DIR = Path(__file__).parent
PROJECT_ROOT = SCRIPT_DIR.parent
DATA_DIR = PROJECT_ROOT / "test" / "data"

# Путь к исполняемому файлу det
DET_EXECUTABLE = PROJECT_ROOT / "build" / "Release" / "det"


def get_test_cases() -> list[int]:
    """Получает список номеров тестов из папки data."""
    if not DATA_DIR.exists():
        return []
    
    tests = []
    for in_file in DATA_DIR.glob("*.in"):
        try:
            num = int(in_file.stem)
            out_file = DATA_DIR / f"{num}.out"
            if out_file.exists():
                tests.append(num)
        except ValueError:
            # Игнорируем файлы с нечисловыми именами
            pass
    
    return sorted(tests)


def read_file(path: Path) -> str:
    """Читает содержимое файла."""
    return path.read_text().strip()


def run_det(input_data: str) -> tuple[str, int]:
    """
    Запускает программу det с входными данными.
    Возвращает (stdout, return_code).
    """
    if not DET_EXECUTABLE.exists():
        pytest.skip(f"Executable not found: {DET_EXECUTABLE}")
    
    result = subprocess.run(
        [str(DET_EXECUTABLE)],
        input=input_data,
        capture_output=True,
        text=True,
        timeout=3
    )
    return result.stdout.strip(), result.returncode


class TestDetE2E:
    """E2E тесты для программы det."""
    
    @pytest.fixture(autouse=True)
    def check_executable(self):
        """Проверяет наличие исполняемого файла перед тестами."""
        if not DET_EXECUTABLE.exists():
            pytest.skip(f"Executable not found: {DET_EXECUTABLE}")
    
    @pytest.mark.parametrize("test_num", get_test_cases())
    def test_determinant(self, test_num: int):
        """Тестирует вычисление детерминанта на тестовом наборе."""
        input_file    = DATA_DIR / f"{test_num}.in"
        expected_file = DATA_DIR / f"{test_num}.out"
        
        input_data = read_file(input_file)
        expected   = read_file(expected_file)
        
        actual, return_code = run_det(input_data)
        
        assert return_code == 0, f"Program returned non-zero exit code: {return_code}"
        assert actual == expected, (
            f"Test {test_num} failed.\n"
            f"Input:\n{input_data}\n"
            f"Expected: {expected}\n"
            f"Actual: {actual}"
        )


if __name__ == "__main__":
    pytest.main([__file__, "-v"])
