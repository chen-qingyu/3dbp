# 3DBP

_Pack All~_

## 1. 属性

- 名称：3DBP (意为 **3D** **B**in **P**acking)
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

#### Container (容器)

| 字段名 | 类型    | 描述                   |
| ------ | ------- | ---------------------- |
| `id`   | string  | 容器的唯一标识符       |
| `lx`   | integer | 容器的长度             |
| `ly`   | integer | 容器的宽度             |
| `lz`   | integer | 容器的高度             |
| `load` | number  | 容器的最大负载（可选） |

#### Box (箱子)

| 字段名   | 类型    | 描述               |
| -------- | ------- | ------------------ |
| `id`     | string  | 箱子的唯一标识符   |
| `lx`     | integer | 箱子的长度         |
| `ly`     | integer | 箱子的宽度         |
| `lz`     | integer | 箱子的高度         |
| `weight` | number  | 箱子的重量（可选） |

#### Input (输入数据)

| 字段名       | 类型  | 描述             |
| ------------ | ----- | ---------------- |
| `containers` | array | 可用的容器列表   |
| `boxes`      | array | 待装载的箱子列表 |

示例输入请参考 [demo.json](data/demo.json)

### 输出格式

输出文件为 JSON 格式，包含以下字段：

#### Container (容器)

| 字段名 | 类型    | 描述                   |
| ------ | ------- | ---------------------- |
| `id`   | string  | 容器的唯一标识符       |
| `lx`   | integer | 容器的长度             |
| `ly`   | integer | 容器的宽度             |
| `lz`   | integer | 容器的高度             |
| `load` | number  | 容器的最大负载（可选） |

#### Box (箱子)

| 字段名   | 类型    | 描述               |
| -------- | ------- | ------------------ |
| `id`     | string  | 箱子的唯一标识符   |
| `lx`     | integer | 箱子的长度         |
| `ly`     | integer | 箱子的宽度         |
| `lz`     | integer | 箱子的高度         |
| `x`      | integer | 放置的 X 轴坐标    |
| `y`      | integer | 放置的 Y 轴坐标    |
| `z`      | integer | 放置的 Z 轴坐标    |
| `weight` | number  | 箱子的重量（可选） |

#### Plan (装箱计划)

| 字段名        | 类型   | 描述                     |
| ------------- | ------ | ------------------------ |
| `container`   | object | 使用的容器信息           |
| `boxes`       | array  | 已装载的箱子列表         |
| `volume_rate` | number | 容器的体积利用率         |
| `weight_rate` | number | 容器的负载利用率（可选） |

#### Output (输出数据)

| 字段名           | 类型  | 描述             |
| ---------------- | ----- | ---------------- |
| `plans`          | array | 装箱计划列表     |
| `unpacked_boxes` | array | 未装载的箱子列表 |

## 5. 开发

- 语言：C++ ，要求 [C++20](https://en.cppreference.com/w/cpp/20.html)
- 构建：使用 [XMake](https://xmake.io) 进行构建
- 画图：使用 [Python](https://www.python.org) 进行画图
- 风格：使用 [ClangFormat](https://clang.llvm.org/docs/ClangFormat.html) 统一代码风格，大部分遵循 [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html) ，小部分基于项目规模和源码简洁性的考虑采用自己的风格
- 安全：使用 [Dr. Memory](https://drmemory.org) 进行检查
- 数据：使用 [OR-Library](https://people.brunel.ac.uk/~mastjjb/jeb/orlib/thpackinfo.html) 的测试数据以评估装箱效果
