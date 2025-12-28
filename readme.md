# pack3d

_Pack All~_

## 1. 属性

- 名称：pack3d
- 目标：提供一个高效的程序帮助用户优化三维装箱的效果
- 模块：采用模块化设计，输入输出、算法、约束，均可替换

## 2. 特点

- 简洁：Stay simple, stay young. 在保证好用和健壮的前提下，尽量简洁，便于维护和阅读
- 好用：经过我的精心设计，用起来非常方便
- 健壮：怎么折腾都不会崩
- 轻量：使用及拓展非常轻便和简单
- 高效：速度非常快，BR 测试数据统统一秒内解决

## 3. 用法

1. 按照下述输入格式准备 JSON 输入文件
2. 运行程序，通过参数指定输入文件路径
3. 程序将在 `result` 目录下生成 JSON 输出文件，并自动画图展示

## 4. 输入输出

### 坐标系定义

首先约定，程序统一采用右手坐标系，定义如下：

- X 轴：长度，向右为正方向。
- Y 轴：宽度，向后为正方向。
- Z 轴：高度，向上为正方向。

### 输入格式

输入文件为 JSON 格式，包含以下字段：

#### ContainerType (容器类型)

| 字段名     | 类型    | 描述                                   |
| ---------- | ------- | -------------------------------------- |
| `id`       | string  | 容器类型的唯一标识符                   |
| `lx`       | integer | 容器类型的长度                         |
| `ly`       | integer | 容器类型的宽度                         |
| `lz`       | integer | 容器类型的高度                         |
| `payload`  | number  | 容器类型的最大载重（可选）             |
| `quantity` | integer | 容器类型的可用数量（可选，默认无限制） |

#### BoxType（箱型）

| 字段名    | 类型    | 描述                       |
| --------- | ------- | -------------------------- |
| `id`      | string  | 箱型的唯一标识符           |
| `lx`      | integer | 箱型的长度                 |
| `ly`      | integer | 箱型的宽度                 |
| `lz`      | integer | 箱型的高度                 |
| `orients` | array   | 箱型允许的放置方向（可选） |

##### Orients (允许的放置方向)

箱型的放置方向使用整数表示，共有六种可能的放置方向：

- `0`：长度 (lx) 沿 X 轴，宽度 (ly) 沿 Y 轴，高度 (lz) 沿 Z 轴（高度垂直）
- `1`：宽度 (ly) 沿 X 轴，长度 (lx) 沿 Y 轴，高度 (lz) 沿 Z 轴（高度垂直）
- `2`：长度 (lx) 沿 X 轴，高度 (lz) 沿 Y 轴，宽度 (ly) 沿 Z 轴（宽度垂直）
- `3`：高度 (lz) 沿 X 轴，长度 (lx) 沿 Y 轴，宽度 (ly) 沿 Z 轴（宽度垂直）
- `4`：宽度 (ly) 沿 X 轴，高度 (lz) 沿 Y 轴，长度 (lx) 沿 Z 轴（长度垂直）
- `5`：高度 (lz) 沿 X 轴，宽度 (ly) 沿 Y 轴，长度 (lx) 沿 Z 轴（长度垂直）

比如 `"orients": [0, 1]` 表示该箱型的箱子只能平着放置，不能竖着放置。

如果未指定 `orients` 字段，默认是`[0, 1]`，也就是只允许平着放置。

#### Box (箱子)

| 字段名   | 类型   | 描述               |
| -------- | ------ | ------------------ |
| `id`     | string | 箱子的唯一标识符   |
| `type`   | string | 箱子的类型         |
| `weight` | number | 箱子的重量（可选） |

#### Input (输入数据)

| 字段名            | 类型  | 描述               |
| ----------------- | ----- | ------------------ |
| `container_types` | array | 可用的容器类型列表 |
| `box_types`       | array | 可用的箱型列表     |
| `boxes`           | array | 待装载的箱子列表   |

示例输入请参考 [demo.json](data/demo.json)

### 输出格式

输出文件为 JSON 格式，包含以下字段：

#### Container (容器)

| 字段名        | 类型   | 描述             |
| ------------- | ------ | ---------------- |
| `type`        | object | 容器的类型       |
| `boxes`       | array  | 已装载的箱子列表 |
| `volume_rate` | number | 容器的体积利用率 |
| `weight_rate` | number | 容器的载重利用率 |

##### Box (箱子)

| 字段名   | 类型    | 描述             |
| -------- | ------- | ---------------- |
| `id`     | string  | 箱子的唯一标识符 |
| `type`   | string  | 箱型的唯一标识符 |
| `x`      | integer | 装载的 X 轴坐标  |
| `y`      | integer | 装载的 Y 轴坐标  |
| `z`      | integer | 装载的 Z 轴坐标  |
| `orient` | integer | 装载时的放置方向 |

#### Output (输出数据)

| 字段名           | 类型  | 描述             |
| ---------------- | ----- | ---------------- |
| `box_types`      | array | 箱型列表         |
| `containers`     | array | 容器列表         |
| `unpacked_boxes` | array | 未装载的箱子列表 |

完整的输出格式树状图如下：

```
output
├── box_types
│   ├── box_type#1
│   ├── box_type#2
│   └── ...
├── containers
│   ├── container#1
│   │   ├── type
│   │   └── boxes
│   │   │   ├── box#1
│   │   │   ├── box#2
│   │   │   └── ...
│   │   ├── volume_rate
│   │   └── weight_rate
│   ├── container#2
│   │   ├── type
│   │   └── boxes
│   │   │   ├── box#3
│   │   │   ├── box#4
│   │   │   └── ...
│   │   ├── volume_rate
│   │   └── weight_rate
│   └── ...
└── unpacked_boxes
    ├── box#5
    ├── box#6
    └── ...
```

## 5. 开发

- 语言：C++ ，要求 [C++20](https://en.cppreference.com/w/cpp/20.html)
- 构建：使用 [XMake](https://xmake.io) 进行构建
- 画图：使用 [Python](https://www.python.org) 进行画图
- 风格：使用 [ClangFormat](https://clang.llvm.org/docs/ClangFormat.html) 统一代码风格，大部分遵循 [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html) ，小部分基于项目规模和源码简洁性的考虑采用自己的风格
- 测试：使用 [Catch2](https://github.com/catchorg/Catch2) 进行测试
- 安全：使用 [Dr. Memory](https://drmemory.org) 进行检查
- 数据：使用 [OR-Library](https://people.brunel.ac.uk/~mastjjb/jeb/orlib/thpackinfo.html) 的测试数据以评估装箱效果

note:

```bash
# 格式化代码
xmake format

# 配置 debug 模式
xmake config -m debug

# 运行 demo 并画图
xmake run pack3d ./data/demo.json

# 单元测试 & 集成测试
xmake run test

# 覆盖率测试
opencppcoverage --sources sources --export_type html:build/coverage -- ./build/windows/x64/debug/test.exe

# 配置 release 模式
xmake config -m release

# 性能测试
xmake run bench --benchmark-no-analysis

# 生成报告
xmake run report
```
