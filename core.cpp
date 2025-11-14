#include <QApplication>
#include <QMainWindow>
#include <QScreen>
#include <QLabel>
#include <QWidget>
#include <QGraphicsView>
#include "core.hpp"
#include "menu.hpp"
#include "log.hpp"
#include "base/font.hpp"
#include "notes/features.hpp"
#include "notes/getNotes.hpp"
#include "plugins/getPlugins.hpp"

MainGame* MG = nullptr;
const QString* helpStr = nullptr;

int runApp(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setFont(getFont(1));
    Log::Debug("Main") << "Configuration located at:\n" << getPath();
    MG = new MainGame();
    initMenu(MG->menuBar());
    registerNoteFeatures();
    initNotes();
    loadPlugins();
    MG->initScene();

    MG->showMaximized();
    return app.exec();
}
