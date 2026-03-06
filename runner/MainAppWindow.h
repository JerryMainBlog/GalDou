#pragma once
#include <QMainWindow>
#include <QSplitter>
#include <QAction>
#include <QLabel>
#include <QGraphicsView>

#include "EditorPanel.h"
#include "PreviewPanel.h"
#include "../engine/render/RenderScene.h"
#include "../engine/ui/DialogBox.h"
#include "../engine/ui/ChoiceWidget.h"
#include "../engine/core/GameEngine.h"
#include "../engine/audio/AudioManager.h"

class MainAppWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainAppWindow(QWidget* parent = nullptr);

protected:
    void resizeEvent(QResizeEvent*) override;
    void closeEvent(QCloseEvent*) override;
    bool eventFilter(QObject* obj, QEvent* e) override;

private slots:
    void onRun();
    void onStop();
    void onReload();
    void onScriptSaved(const QString& path);
    void onGameEnded();

private:
    void setupPreviewPanel();
    void setupToolBar();
    void setupMenuBar();
    void setupStatusBar();
    void relayoutPreview();

    EditorPanel*   m_editorPanel;
    QWidget*       m_previewWidget;
    QGraphicsView* m_view;
    RenderScene*   m_scene;
    DialogBox*     m_dialog;
    ChoiceWidget*  m_choice;
    GameEngine*    m_engine;
    AudioManager*  m_audio;

    QAction* m_actRun;
    QAction* m_actStop;
    QAction* m_actReload;

    QLabel*  m_cursorLabel;
    QLabel*  m_statusLabel;

    QString  m_currentScriptPath;
    bool     m_isRunning = false;
};