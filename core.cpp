#include <QApplication>
#include <QMainWindow>
#include <QScreen>
#include <QLabel>
#include <QWidget>
#include <QGraphicsView>
#include "core.hpp"
#include "menu.hpp"
#include "log.hpp"
#include "settings.hpp"
#include "base/font.hpp"
#include "notes/features.hpp"
#include "notes/decks.hpp"
#include "notes/getNotes.hpp"
#include "plugins/getPlugins.hpp"
#include <QStandardPaths>

MainGame* MG = nullptr;
const QString* helpStr = nullptr;

int runApp(int argc, char *argv[]) {
    QApplication app(argc, argv);
    initSettings();
    getDecksPath(); // Create decks folder if does not exist
    clearTempNotes();
    initNotes();
    changeDeck(getStrSetting(curDeckSetting), false);
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    Log::Debug("Main") << "Configuration located at:\n" << path;
    MG = new MainGame();
    initMenu(MG->menuBar());
    registerNoteFeatures();
    loadPlugins();
    MG->initScene();

    MG->showMaximized();
    return app.exec();
}
