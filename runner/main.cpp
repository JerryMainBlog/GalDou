#include <QApplication>
#include "MainAppWindow.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("GalDou");
    app.setApplicationVersion("0.1.0");
    app.setOrganizationName("GalDou");

    QApplication::setHighDpiScaleFactorRoundingPolicy(
        Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);

    MainAppWindow w;
    w.show();
    return app.exec();
}