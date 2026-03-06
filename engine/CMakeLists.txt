add_library(GalDou_Engine STATIC
    core/EventBus.cpp
    core/FlagManager.cpp
    core/ScriptParser.cpp
    core/GameEngine.cpp
    render/Animator.cpp
    render/RenderScene.cpp
    audio/AudioManager.cpp
    ui/DialogBox.cpp
    ui/ChoiceWidget.cpp
)

target_link_libraries(GalDou_Engine PUBLIC
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Multimedia
)

target_include_directories(GalDou_Engine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)