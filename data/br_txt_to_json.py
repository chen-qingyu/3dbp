import json
import os


def parse_br_file(file):
    """转换BR文件为输入JSON格式"""
    results = []

    with open(file, 'r') as f:
        lines = [line.strip() for line in f if line.strip()]

    current_line = 1
    for _ in range(int(lines[0])):
        # 读取问题编号
        problem_num = int(lines[current_line].split()[0])
        current_line += 1

        # 读取容器尺寸
        lx, ly, lz = map(int, lines[current_line].split())
        current_line += 1

        # 读取箱子信息
        num_box_types = int(lines[current_line])
        current_line += 1

        # 创建容器
        container = {
            "id": f"{os.path.basename(file).split('.')[0]}#{problem_num}",
            "lx": lx,
            "ly": ly,
            "lz": lz,
        }

        # 创建箱子
        boxes = []
        box_id = 1
        for _ in range(num_box_types):
            parts = list(map(int, lines[current_line].split()))
            current_line += 1

            # 为每个数量创建一个箱子实例
            for _ in range(parts[7]):
                boxes.append({
                    "id": f"b{box_id}",
                    "lx": parts[1],
                    "ly": parts[3],
                    "lz": parts[5],
                })
                box_id += 1

        # 创建单个问题的结果
        results.append((problem_num, {"containers": [container], "boxes": boxes}))

    return results


def main():
    """处理所有BR文件"""
    # https://people.brunel.ac.uk/~mastjjb/jeb/orlib/thpackinfo.html

    if os.path.exists('data/br_json'):
        return  # 若已存在则跳过处理

    os.makedirs('data/br_json')
    for i in range(1, 11):
        br_path = f'data/br_txt/br{i}.txt'
        results = parse_br_file(br_path)

        # 每个问题都生成一个JSON文件
        for problem_num, result in results:
            with open(f'data/br_json/br{i}_{problem_num}.json', 'w') as f:
                json.dump(result, f, indent=2)
        print(f"OK: {br_path} -> {len(results)} json files")


if __name__ == "__main__":
    main()
