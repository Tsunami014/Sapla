#include "game.hpp"
#include "main.hpp"
#include "scenes/homeScn.hpp"
#include "games/getGames.hpp"
#include "items/base/font.hpp"
#include <QRandomGenerator>
#include <QTimer>

MainGame::MainGame() : s{0, 0} {
    bg = nullptr;
}
void MainGame::initScene() {
    curScene = new HomeScene();
    curScene->setZValue(-1);
    MScene->addItem(curScene);
}

void MainGame::changeBG(QString bgName) {
    QString pth = ":/assets/bgs/"+bgName+".svg";
    if (bg == nullptr) {
        bg = new SvgGraphicItem(pth);
        bg->setZValue(-100);
        MScene->addItem(bg);
    } else {
        bg->setSvg(pth);
    }
}

void MainGame::changeScene(BaseScene* newScene) {
    QTimer::singleShot(0, [=]() {
        auto* view = MScene->views()[0];
        view->setUpdatesEnabled(false);
        MScene->removeItem(curScene);
        delete curScene;
        MScene->views()[0]->viewport()->unsetCursor();
        curScene = newScene;
        curScene->setZValue(-1);
        MScene->addItem(newScene);
        resizeEvent(MScene->sceneRect());
        view->setUpdatesEnabled(true);
        view->viewport()->update();
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
    QRectF rect = MScene->sceneRect();
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

