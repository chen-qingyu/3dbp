"""
绘制 3D 装箱图。
Usage:
    py draw.py <result_json>
"""

import sys
import json
from dataclasses import dataclass

import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d.art3d import Poly3DCollection
from matplotlib.widgets import Button
from matplotlib.colors import TABLEAU_COLORS
from matplotlib.patches import Patch

# 设置中文字体支持
plt.rcParams['font.sans-serif'] = ['SimHei', 'Microsoft YaHei', 'DejaVu Sans']


def main(data: dict):
    """绘制3D装箱图主函数"""
    containers = data["containers"]
    box_types = {bt["id"]: bt for bt in data["box_types"]}

    # 计算绘图相关参数
    max_dims = calc_max_dims(containers)
    n = len(containers)
    cols = min(4, n)  # 最多4列
    rows = (n + cols - 1) // cols

    # 创建图形和子图
    fig = plt.figure(figsize=(6 * cols, 5 * rows))
    plot_axes = []

    # 绘制每个装箱方案
    for i, container in enumerate(containers):
        ax = fig.add_subplot(rows, cols, i + 1, projection="3d")
        draw(container, ax, max_dims, box_types)
        plot_axes.append(ax)

    # 创建视图切换按钮
    view_button = ViewButton(fig)
    view_button.plot_axes = plot_axes

    # 创建平台颜色图例
    create_legend(containers, fig)

    # 默认最大化窗口显示
    plt.get_current_fig_manager().window.showMaximized()  # type: ignore
    plt.show()


def draw(container: dict, ax, max_dims: tuple[int, int, int], box_types: dict):
    """绘制容器和箱子"""
    # 绘制容器
    cl, cw, ch = container["lx"], container["ly"], container["lz"]
    container_faces = cuboid_faces(0, 0, 0, cl, cw, ch)
    ax.add_collection3d(Poly3DCollection(container_faces, edgecolors="black", alpha=0.1))
    ax.set_title(f"Container {container['id']}")

    # 绘制箱子
    for box in container["boxes"]:
        box_type = box_types[box["type"]]
        box_faces = cuboid_faces(box["x"], box["y"], box["z"], box_type["lx"], box_type["ly"], box_type["lz"])
        ax.add_collection3d(Poly3DCollection(box_faces, facecolors=get_color(box), edgecolors="black", linewidths=0.5, alpha=0.5))

    # 设置轴比例和范围
    ax.set_box_aspect([max_dims[0], max_dims[1], max_dims[2]])
    ax.set(xlim=(0, max_dims[0]), ylim=(0, max_dims[1]), zlim=(0, max_dims[2]), xlabel="X", ylabel="Y", zlabel="Z")

    # 显示容器信息和利用率
    info = f"Volume Rate: {container['volume_rate']:.2%}"
    if container['weight_rate'] is not None:
        info += f", Weight Rate: {container['weight_rate']:.2%}"
    ax.text2D(0.5, -0.05, info, transform=ax.transAxes, ha="center")


def get_color(box: dict, colors={}):
    """颜色映射"""
    base = list(TABLEAU_COLORS.values())
    group = box["group"]
    if group not in colors:
        colors[group] = base[len(colors) % len(base)]
    return colors[group]


def cuboid_faces(x: int, y: int, z: int, l: int, w: int, h: int) -> list[list[list[int]]]:
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


def calc_max_dims(containers: list[dict]) -> tuple[int, int, int]:
    """计算所有容器在长宽高三个维度的最大尺寸"""
    max_l, max_w, max_h = 0, 0, 0
    for container in containers:
        max_l = max(max_l, container["lx"])
        max_w = max(max_w, container["ly"])
        max_h = max(max_h, container["lz"])
    return (max_l, max_w, max_h)


def create_legend(containers: list[dict], fig):
    """创建平台颜色图例"""
    # 收集平台信息
    groups = set()
    for container in containers:
        for box in container["boxes"]:
            groups.add(box["group"])

    # 创建图例元素
    legends = []
    for group in sorted(groups):
        # 创建一个假的box来获取颜色
        dummy = {"group": group}
        color = get_color(dummy)
        legends.append(Patch(facecolor=color, label=group))

    # 添加图例
    fig.legend(handles=legends, loc='center right', title="Groups")


@dataclass
class ViewConfig:
    """视图配置类"""
    name: str
    elev: int
    azim: int


class ViewButton:
    """视图按钮类，封装按钮创建和交互逻辑"""

    def __init__(self, fig):
        """初始化视图按钮"""
        self.fig = fig  # 图形对象
        self.buttons = []  # 存储视图按钮
        self.plot_axes = []  # 存储绘图轴，排除按钮轴
        self.VIEW_CONFIGS = [  # 预定义的视图配置
            ViewConfig("Front", 0, 0),      # 前视图
            ViewConfig("Back", 0, 180),     # 后视图
            ViewConfig("Left", 0, 90),      # 左视图
            ViewConfig("Right", 0, -90),    # 右视图
            ViewConfig("Top", 90, 0),       # 顶视图
            ViewConfig("Bottom", -90, 0),   # 底视图
            ViewConfig("Default", 30, -60),   # 默认视图
        ]

        button_width = 0.07
        button_height = 0.035

        for i, config in enumerate(self.VIEW_CONFIGS):
            x_pos = i * button_width
            button_ax = self.fig.add_axes((x_pos, 0, button_width, button_height))
            button = Button(button_ax, config.name)
            button.on_clicked(lambda event, cfg=config: self._on_button_click(cfg))
            self.buttons.append(button)

    def _on_button_click(self, config: ViewConfig):
        for ax in self.plot_axes:
            ax.view_init(elev=config.elev, azim=config.azim)
        self.fig.canvas.draw()


if __name__ == "__main__":
    # 检查命令行参数
    if len(sys.argv) != 2:
        print("Usage: py draw.py <result_json>")
        sys.exit(1)
    # 加载装箱方案数据
    with open(sys.argv[1], "r", encoding="utf-8") as f:
        data = json.load(f)
    # 绘制3D装箱图
    main(data)
