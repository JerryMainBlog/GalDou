#pragma once
#include <QWidget>
#include <QGraphicsView>
#include <QVBoxLayout>
#include <QLabel>

#include "../engine/render/RenderScene.h"
#include "../engine/ui/DialogBox.h"
#include "../engine/ui/ChoiceWidget.h"
#include "../engine/core/GameEngine.h"
#include "../engine/audio/AudioManager.h"

/*
 * PreviewPanel — 右侧游戏预览面板
 * 包含完整的游戏运行环境：
 *   - RenderScene（背景+立绘渲染）
 *   - DialogBox（对话框）
 *   - ChoiceWidget（选项）
 *   - GameEngine（脚本执行）
 *   - AudioManager（音频）
 *
 * 对外接口：
 *   loadScript(path) → 加载脚本
 *   run()            → 从头开始运行
 *   stop()           → 停止运行，清空画面
 */
class PreviewPanel : public QWidget {
    Q_OBJECT
public:
    explicit PreviewPanel(QWidget* parent = nullptr);

    void loadAndRun(const QString& scriptPath);
    void stop();

signals:
    void gameEnded();

protected:
    void resizeEvent(QResizeEvent*) override;
    void mousePressEvent(QMouseEvent*) override;
    void keyPressEvent(QKeyEvent*) override;

private:
    void relayout();

    QGraphicsView* m_view;
    RenderScene*   m_scene;
    DialogBox*     m_dialog;
    ChoiceWidget*  m_choice;
    GameEngine*    m_engine;
    AudioManager*  m_audio;

    QLabel*        m_placeholder;  // 未加载时显示的提示
    QString        m_currentScript;
};
