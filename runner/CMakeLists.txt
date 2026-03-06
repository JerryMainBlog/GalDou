add_executable(GalDou WIN32
    main.cpp
    MainAppWindow.h
    MainAppWindow.cpp
    EditorPanel.h
    EditorPanel.cpp
    PreviewPanel.h
    PreviewPanel.cpp
)

target_include_directories(GalDou PRIVATE
    ${CMAKE_SOURCE_DIR}
)

target_link_libraries(GalDou PRIVATE
    GalDou_Engine
    GalDou_Editor
    Qt6::Core
    Qt6::Widgets
    Qt6::Gui
    Qt6::Multimedia
)