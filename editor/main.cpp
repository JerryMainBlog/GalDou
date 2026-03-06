#include <QApplication>
#include "EditorWindow.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("GalDou Editor");
    app.setApplicationVersion("0.1.0");
    app.setOrganizationName("GalDou");

    // 统一 DPI 缩放
    QApplication::setHighDpiScaleFactorRoundingPolicy(
        Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);

    EditorWindow w;
    w.show();
    return app.exec();
}
