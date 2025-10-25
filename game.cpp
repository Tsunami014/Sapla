#include "game.hpp"
#include "main.hpp"
#include "scenes/homeScn.hpp"
#include "games/getGames.hpp"
#include "items/base/font.hpp"
#include <QRandomGenerator>
#include <QTimer>

MainGame::MainGame() {
    bg = nullptr;
}
void MainGame::initScene() {
    curScene = new HomeScene();
    curScene->setZValue(-1);
    MScene->addItem(curScene);
}

void MainGame::changeBG(QString bgName) {
    if (bg != nullptr) {
        MScene->removeItem(bg);
        delete bg;
    }
    bg = new SvgGraphicItem(":/assets/bgs/"+bgName+".svg");
    bg->setZValue(-100);
    MScene->addItem(bg);
}

void MainGame::changeScene(BaseScene* newScene) {
    QTimer::singleShot(0, [=]() {
        MScene->removeItem(curScene);
        delete curScene;
        MScene->views()[0]->viewport()->unsetCursor();
        curScene = newScene;
        curScene->setZValue(-1);
        MScene->addItem(newScene);
        resizeEvent(MScene->sceneRect());
    });
}

void MainGame::nextFC() {
    int gSze = games.size();
    if (gSze == 0) return;
    int gidx = QRandomGenerator::global()->bounded(gSze);
    for (int _ = 0; _ < gSze; _++) {
        bool success = games[gidx].run();
        if (success) break;
        gidx = (gidx + 1) % gSze;
    }
}

void MainGame::resizeEvent(const QRectF& newSze) {
    bg->setRect(newSze);
    curScene->setRect(newSze);
    updateLogs();
}
void MainGame::updateLogs() {
    QRectF rect = bg->boundingRect();
    const int margin = 15;
    const int edgeMargin = 30;
    qreal y = rect.height() - margin;
    qreal width = rect.width()*0.3;
    for (auto* l : logs) {
        l->setTxtWid(width);
        QSizeF size = l->boundingRect().size();
        y -= size.height(); 
        l->setPos({rect.right() - width - edgeMargin, y});
        y -= margin;
    }
}

