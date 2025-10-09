#include "game.hpp"
#include "main.hpp"
#include "scenes/homeScn.hpp"
#include "items/base/font.hpp"

MainGame::MainGame() {
    bg = new SvgGraphicItem(":/assets/bg.svg");
    bg->setZValue(-100);
    MScene->addItem(bg);

    bottomTxt = new QGraphicsTextItem("");
    bottomTxt->setDefaultTextColor({0, 0, 0});
    bottomTxt->setZValue(100);
    MScene->addItem(bottomTxt);
}
void MainGame::initScene() {
    curScene = new HomeScene();
    curScene->setZValue(-1);
    MScene->addItem(curScene);
}

void MainGame::setBottomTxt(const QString& newTxt) {
    bottomTxt->setPlainText(newTxt);
}

void MainGame::changeScene(BaseScene* newScene) {
    MScene->removeItem(curScene);
    delete curScene;
    MScene->views()[0]->viewport()->unsetCursor();
    curScene = newScene;
    curScene->setZValue(-1);
    MScene->addItem(newScene);
    resizeEvent(bg->boundingRect());
}

void MainGame::resizeEvent(const QRectF& newSze) {
    bg->setRect(newSze);
    qreal fontSze = newSze.height()*0.03;
    bottomTxt->setFont(getFont(fontSze));
    qreal fontY = newSze.height()-fontSze*1.1;
    bottomTxt->setPos(0, newSze.y()+fontY);
    curScene->setRect({newSze.x(), newSze.y(), newSze.width(), fontY});
}

