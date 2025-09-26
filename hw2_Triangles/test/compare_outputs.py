import sys
import subprocess

# ANSI цвета
GREEN = "\033[92m"
RED = "\033[91m"
RESET = "\033[0m"

if len(sys.argv) < 5:
    print("Usage: compare_outputs.py <test_number> <program> <input_file> <output_file>")
    sys.exit(1)

test_number, prog, infile, outfile = sys.argv[1:]

print(f"\n=== Test #{test_number} ===")
print(f"Program : {prog}")
print(f"Input   : {infile}")
print(f"Expected: {outfile}")

# Запуск программы
try:
    with open(infile, "r") as fin:
        result = subprocess.run(
            [prog],
            stdin=fin,
            capture_output=True,
            text=True,
            timeout=5
        )
except Exception as e:
    print(f"{RED}❌ Error running program: {e}{RESET}")
    sys.exit(1)

if result.returncode != 0:
    print(f"{RED}❌ Program exited with non-zero code {result.returncode}{RESET}")
    sys.exit(1)

# Чтение эталонного результата
with open(outfile, "r") as f:
    expected = f.read()

# Разбиваем на числа
def parse_numbers(s: str):
    return list(map(int, s.split()))

actual_numbers = parse_numbers(result.stdout)
expected_numbers = parse_numbers(expected)

if actual_numbers == expected_numbers:
    print(f"{GREEN}✅ PASSED{RESET}")
    sys.exit(0)
else:
    print(f"{RED}❌ FAILED{RESET}")
    print("Expected:", expected_numbers)
    print("Got     :", actual_numbers)
    sys.exit(1)
