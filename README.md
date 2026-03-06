# GalDou

> 基于 Qt 6 / C++17 的 Galgame 开发套件

GalDou 不是一个游戏，而是一套用于制作 Galgame 的开发工具。左侧编写剧本脚本，右侧实时预览效果，保存即可运行。

---

## 界面预览

```
┌─────────────────────────────────────────────────────────────┐
│  工具栏：📂打开  💾保存  ▶运行  ⏹停止  ↺重载               │
├──────────────────────────┬──────────────────────────────────┤
│  文件树                  │                                  │
│  ├── scene1.gal          │        右侧预览区                 │
│  └── demo.gal            │   (RenderScene + 对话框 + 选项)   │
│                          │                                  │
│  脚本编辑区（多Tab）      │                                  │
│  [scene] start           │                                  │
│  [bg] "bg_room.png"      │                                  │
│  [say] "角色" "你好"     │                                  │
│                          │                                  │
│  ⚠ 错误面板              │                                  │
└──────────────────────────┴──────────────────────────────────┘
```

---

## 项目结构

```
GalDou/
├── CMakeLists.txt
├── engine/                     # 引擎静态库
│   ├── core/                   # EventBus, FlagManager, GameEngine, ScriptParser
│   ├── render/                 # RenderScene, Animator
│   ├── ui/                     # DialogBox, ChoiceWidget
│   └── audio/                  # AudioManager
├── editor/                     # 编辑器组件静态库
│   ├── ScriptEditor            # 代码编辑器（行号+高亮+错误标记）
│   ├── GalHighlighter          # 语法高亮
│   ├── FileTreePanel           # 文件树
│   └── ScriptValidator         # 语法检查
├── runner/                     # 主程序
│   ├── MainAppWindow           # 主窗口（编辑+预览合一）
│   ├── EditorPanel             # 左侧编辑面板
│   └── PreviewPanel            # 右侧预览面板
└── examples/
    └── demo.gal                # 示例脚本
```

---

## 脚本语法

| 命令 | 格式 | 说明 |
|------|------|------|
| `[scene]` | `[scene] 场景名` | 定义场景入口 |
| `[bg]` | `[bg] "路径"` | 设置背景图 |
| `[bgm]` | `[bgm] "路径"` | 播放背景音乐 |
| `[show]` | `[show] "路径" left/center/right` | 显示立绘 |
| `[hide]` | `[hide] "路径"` | 隐藏立绘 |
| `[say]` | `[say] "角色名" "台词"` | 显示对话 |
| `[choice]` | `[choice]` | 开始选项分支 |
| `[option]` | `[option] "按钮文字" "目标场景"` | 定义选项 |
| `[set]` | `[set] 变量名 值` | 设置变量 |
| `[if]` | `[if] 条件 场景_true 场景_false` | 条件跳转 |
| `[end]` | `[end]` | 结束 |
| `#` | `# 注释` | 注释 |

---

## 构建方法

**依赖：** Qt 6.x、CMake 3.16+、C++17 编译器

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --parallel
```

产物为 `runner/GalDou.exe`。

---

## 架构说明

各模块通过 `EventBus` 发布订阅事件，完全解耦：

```
GameEngine → publish("render.bg")   → RenderScene
           → publish("dialog.say")  → DialogBox
           → publish("choice.show") → ChoiceWidget
           → publish("audio.bgm")   → AudioManager

ChoiceWidget → publish("choice.made") → GameEngine
```

---

## 计划功能

- [ ] 命令自动补全
- [ ] 存档 / 读档系统
- [ ] 打包导出
- [ ] 素材资源管理器
- [ ] 角色表情切换

---

## 开源协议

基于 Qt 6 LGPL v3，发布时需动态链接 Qt DLL 并保留版权声明。