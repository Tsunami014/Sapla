#include <QApplication>
#include <QMainWindow>
#include <QScreen>
#include <QLabel>
#include <QWidget>
#include <QGraphicsView>
#include "main.hpp"
#include "menu.cpp"
#include "log.hpp"
#include "items/base/font.hpp"
#include "cards/cardTyps.hpp"
#include "cards/getCards.hpp"
#include "games/getGames.hpp"

BetterScene* MScene = nullptr;
MainGame* MG = nullptr;
const QString* helpStr = nullptr;

class BetterView : public QGraphicsView {
public:
    BetterView(QGraphicsScene* scene) : QGraphicsView(scene) {}

protected:
    void resizeEvent(QResizeEvent* event) override {
        QGraphicsView::resizeEvent(event);
        QRectF newRect = QRectF(QPointF(0, 0), viewport()->size());
        MScene->setSceneRect(newRect);
        MG->resizeEvent(newRect);
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setFont(getFont());
    Log::Debug("Main") << "Configuration located at:\n" << getPath();
    QMainWindow* w = new QMainWindow;
    initMenu(w->menuBar());
    MScene = new BetterScene();
    MG = new MainGame();
    registerCardTypes();
    initCards();
    loadGames();
    MG->initScene();

    BetterView* view = new BetterView(MScene);
    w->setCentralWidget(view);
    w->showMaximized();
    w->show();
    return app.exec();
}
