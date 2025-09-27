import random
import argparse

def generate_triangles(num_triangles, coord_limit, output_file):
    """
    Генерирует входные данные для треугольников в 3D пространстве
    
    Args:
        num_triangles (int): количество треугольников
        coord_limit (float): лимит координат (от -limit до +limit)
        output_file (str): путь к выходному файлу
    """
    
    with open(output_file, 'w') as f:
        # Записываем количество треугольников
        f.write(f"{num_triangles}\n")
        
        for i in range(num_triangles):
            # Генерируем 3 случайные точки для треугольника
            points = []
            for _ in range(3):
                x = random.uniform(-coord_limit, coord_limit)
                y = random.uniform(-coord_limit, coord_limit)
                z = random.uniform(-coord_limit, coord_limit)
                points.append((x, y, z))
            
            # Записываем координаты треугольника
            for point in points:
                f.write(f"{point[0]:.6f} {point[1]:.6f} {point[2]:.6f}\n")
            
            # Добавляем пустую строку между треугольниками (опционально)
            if i < num_triangles - 1:
                f.write("\n")

def generate_triangles_compact(num_triangles, coord_limit, output_file):
    """
    Альтернативная версия с более компактным форматом (все в одной строке)
    """
    
    with open(output_file, 'w') as f:
        f.write(f"{num_triangles}\n")
        
        for i in range(num_triangles):
            points = []
            for _ in range(3):
                x = random.uniform(-coord_limit, coord_limit)
                y = random.uniform(-coord_limit, coord_limit)
                z = random.uniform(-coord_limit, coord_limit)
                points.extend([x, y, z])
            
            # Записываем все координаты треугольника в одну строку
            coord_str = " ".join(f"{coord:.6f}" for coord in points)
            f.write(f"{coord_str}\n")

def main():
    parser = argparse.ArgumentParser(description='Генератор треугольников в 3D')
    parser.add_argument('num_triangles', type=int, help='Количество треугольников')
    parser.add_argument('coord_limit', type=float, help='Лимит координат')
    parser.add_argument('output_file', type=str, help='Выходной файл')
    parser.add_argument('--compact', action='store_true', help='Использовать компактный формат')
    
    args = parser.parse_args()
    
    if args.compact:
        generate_triangles_compact(args.num_triangles, args.coord_limit, args.output_file)
    else:
        generate_triangles(args.num_triangles, args.coord_limit, args.output_file)
    
    print(f"Сгенерировано {args.num_triangles} треугольников с лимитом координат ±{args.coord_limit}")
    print(f"Результат сохранен в {args.output_file}")

if __name__ == "__main__":
    # Пример использования через аргументы командной строки
    main()