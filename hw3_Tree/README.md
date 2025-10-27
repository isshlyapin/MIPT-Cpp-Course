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
# с тестами
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_TEST=ON
cmake --build build
```

```bash
# без тестов
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

## Запуск основной программы
```bash
./build/rq
```

# Запуск тестов
```bash
ctest --test-dir build/test
```

## Автор

**Шляпин Илья**

- 📫[Email](mailto:shlyapin2005@gmail.com)
- 🌐[Telegram](https://t.me/isshlyapin)