"""
绘制 3D 装箱图。
Usage:
    python draw.py output.json
"""

import sys
import json

import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d.art3d import Poly3DCollection


def cuboid_faces(x, y, z, l, w, h):
    """将长方体转为 6 个面片"""
    pts = [[x, y, z], [x + l, y, z], [x + l, y + w, z], [x, y + w, z],
           [x, y, z + h], [x + l, y, z + h], [x + l, y + w, z + h], [x, y + w, z + h]]
    return [
        [pts[0], pts[1], pts[2], pts[3]],  # bottom
        [pts[4], pts[5], pts[6], pts[7]],  # top
        [pts[0], pts[1], pts[5], pts[4]],  # front
        [pts[2], pts[3], pts[7], pts[6]],  # back
        [pts[1], pts[2], pts[6], pts[5]],  # right
        [pts[0], pts[3], pts[7], pts[4]],  # left
    ]


def draw(plan, ax):
    # 画容器
    container = plan["container"]
    cl, cw, ch = container["lx"], container["ly"], container["lz"]
    ax.add_collection3d(Poly3DCollection(cuboid_faces(0, 0, 0, cl, cw, ch), edgecolors="k", alpha=0.1))
    ax.set_box_aspect([cl, cw, ch])
    ax.set(xlim=(0, cl), ylim=(0, cw), zlim=(0, ch), xlabel="X", ylabel="Y", zlabel="Z")

    # 画箱子
    for box in plan["boxes"]:
        ax.add_collection3d(Poly3DCollection(cuboid_faces(box["x"], box["y"], box["z"], box["lx"], box["ly"], box["lz"]), edgecolors="k", linewidths=0.5, alpha=0.5))

    # 显示装载率
    ax.set_title(f"Container {container['id']}")
    ax.text2D(0.5, -0.12, f"Volume Rate: {plan['volume_rate']}", transform=ax.transAxes, ha="center")


def main(file):
    with open(file, "r", encoding="utf-8") as f:
        plans = json.load(f)["plans"]
    n = len(plans)
    cols = min(4, n)
    rows = (n + cols - 1) // cols
    fig = plt.figure(figsize=(6 * cols, 5 * rows))
    for i, plan in enumerate(plans):
        ax = fig.add_subplot(rows, cols, i + 1, projection="3d")
        draw(plan, ax)
    plt.tight_layout()
    plt.show()


if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python draw.py <json_file>")
        sys.exit(1)
    main(sys.argv[1])
