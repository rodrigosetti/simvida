#include <QApplication>
#include <QTranslator>

#include "gui/main-window.h"

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(recursos);

    QApplication app(argc, argv);

	QTranslator translator;
	if (translator.load("simvida_" + QLocale::system().name()))
		app.installTranslator(&translator);
	else
		qWarning("Translation loading failed.");

	MainWindow *mainWindow = new MainWindow();
    mainWindow->showMaximized();
    return app.exec();
}
