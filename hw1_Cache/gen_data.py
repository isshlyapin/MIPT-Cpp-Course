#!/usr/bin/env python3
import sys
import random

def main():
    if len(sys.argv) != 5:
        print("Usage: gen_data.py <cache_size> <n_unique> <n_requests> <output_file>")
        sys.exit(1)

    cache_size = int(sys.argv[1])
    n_unique   = int(sys.argv[2])
    n_requests = int(sys.argv[3])
    output_file = sys.argv[4]

    if n_unique <= 0 or n_requests <= 0 or cache_size <= 0:
        print("Arguments must be positive integers")
        sys.exit(1)

    # Генерация последовательности запросов
    requests = [random.randint(1, n_unique) for _ in range(n_requests)]

    # Запись в файл
    with open(output_file, "w") as f:
        f.write(f"{cache_size} {n_requests} ")
        f.write(" ".join(map(str, requests)))
        f.write("\n")

if __name__ == "__main__":
    main()
