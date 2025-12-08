# Matrix

## Сборка
```bash
podman pull docker.io/isshlyapin/ubuntu_cpp:cxx23
```

```bash
podman run -it --mount type=bind,source=./,target=/workspace isshlyapin/ubuntu_cpp:cxx23 bash
```

```bash
cd workspace
```

```bash
# для сборки тестов добавить опцию "-o btest_opt=True"
# для отключения import std добавить опцию "-o importstd_opt=False"
conan build . --build=missing -s build_type=Release -pr:a clang21
```

## Запуск
```bash
# основная программа расчета детерминанта
./build/Release/det
```

```bash
# тесты
ctest --test-dir build/Release/test --timeout 3
```