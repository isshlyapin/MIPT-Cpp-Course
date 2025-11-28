# Пример программы C++ с использование C++20 модулей

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
conan build . --build=missing -s build_type=Release -pr:a clang21
```

## Запуск
```bash
./build/Release/main
```