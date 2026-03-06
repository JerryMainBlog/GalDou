add_library(GalDou_Editor STATIC
    EditorWindow.cpp
    EditorWindow.h
    ScriptEditor.cpp
    ScriptEditor.h
    GalHighlighter.cpp
    GalHighlighter.h
    FileTreePanel.cpp
    FileTreePanel.h
    ScriptValidator.cpp
    ScriptValidator.h
)

target_include_directories(GalDou_Editor PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(GalDou_Editor PUBLIC
    GalDou_Engine
    Qt6::Core
    Qt6::Widgets
    Qt6::Gui
)