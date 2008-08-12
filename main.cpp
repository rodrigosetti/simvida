#include <QApplication>
#include "gui/main-window.h"

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(recursos);

    QApplication app(argc, argv);
	MainWindow *mainWindow = new MainWindow();

    mainWindow->show();
    return app.exec();
}
