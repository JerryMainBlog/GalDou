cmake_minimum_required(VERSION 3.16)
project(GalDou VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS
    Core Widgets Gui Multimedia
)

# ── 引擎静态库 ──────────────────────────────────────────────
add_subdirectory(engine)

# ── 编辑器静态库（供主程序使用）────────────────────────────
add_subdirectory(editor)

# ── 主程序（编辑器+预览器合一）─────────────────────────────
add_subdirectory(runner)