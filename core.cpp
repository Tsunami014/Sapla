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
#include "cards/cardTyps.hpp"
#include "cards/getCards.hpp"
#include "games/getGames.hpp"

MainGame* MG = nullptr;
const QString* helpStr = nullptr;

int runApp(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setFont(getFont(1));
    Log::Debug("Main") << "Configuration located at:\n" << getPath();
    MG = new MainGame();
    initMenu(MG->menuBar());
    registerCardTypes();
    initCards();
    loadGames();
    MG->initScene();

    MG->showMaximized();
    return app.exec();
}
