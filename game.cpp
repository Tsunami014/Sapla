#include "game.hpp"
#include "core.hpp"
#include "scenes/homeScn.hpp"
#include "scenes/browseScn.hpp"
#include "scenes/pluginView.hpp"
#include "plugins/getPlugins.hpp"
#include "notes/note.hpp"
#include "items/treeItem.hpp"
#include "base/font.hpp"
#include "base/svgRend.hpp"
#include <QRandomGenerator>
#include <QTimer>
#include <QKeyEvent>
#include <QApplication>

MainGame::MainGame() : s{0, 0}, logLay(), logLayWrap(this), curGame(nullptr) {
    sceneStash = new QWidget(this);
    sceneStash->hide();
    bg = new SvgWidget(this);
    logLayWrap.setLayout(&logLay);
    logLay.addStretch();
    setFont(getFont(1.5));
    qApp->installEventFilter(this);
}
void MainGame::initScene() {
    curScene = new HomeScene();
    setCentralWidget(curScene);
    bg->lower();
    logLayWrap.raise();
}

void MainGame::changeBG(QString bgName) {
    QString pth = ":/assets/bgs/"+bgName+".svg";
    bg->renderer()->load(RenderSvg(pth));
}

void MainGame::removeGame() {
    if (curGame) curGame->deleteLater();
    curGame = nullptr;
}

void MainGame::changeScene(BaseScene* newScene, bool resm) {
    GameScene* newGame = dynamic_cast<GameScene*>(newScene);
    if (newGame) {
        if (curGame && curGame != newGame) {
            curGame->deleteLater();
        }
        curGame = newGame;
    } else {
        GameScene* game = dynamic_cast<GameScene*>(curScene);
        if (game && game != curGame) {
            game->hide();
            game->setParent(sceneStash);
            if (curGame) curGame->deleteLater();
            curGame = game;
        }
    }
    logLayWrap.clearForwarding();

    if (QWidget* w = QApplication::widgetAt(QCursor::pos())) {
        QEvent leaveEv(QEvent::Leave);
        QCoreApplication::sendEvent(w, &leaveEv);
    }

    QWidget* old = takeCentralWidget();
    if (old != curGame) old->deleteLater();
    setCentralWidget(newScene);
    if (!newScene->isVisible()) newScene->show();
    curScene = newScene;
    if (resm && newGame) {
        newGame->resume();
    }
    bg->lower();
    logLayWrap.raise();

    QCursor::setPos(QCursor::pos());  // Force cursor update
}

void MainGame::showFC() {
    if (curGame != nullptr) {
        changeScene(curGame, true);
    } else {
        nextFC();
    }
}
void MainGame::nextFC() {
    auto games = PlugFns->playFns;
    int gSze = games.size();
    if (gSze == 0) return;
    int gidx = QRandomGenerator::global()->bounded(gSze);
    for (int _ = 0; _ < gSze; _++) {
        bool success = games[gidx]();
        if (success) break;
        gidx = (gidx + 1) % gSze;
    }
}

bool MainGame::cardFin(const FlashCard& card, bool correct) {
    if (correct) s.goods++;
    else s.bads++;

    int grow = correct ? 50 : 20;
    return Tree::getTree().grow(grow, correct);
}

bool MainGame::handleEv(QKeyEvent* event) {
    switch (event->key()) {
        case Qt::Key_Escape:
            changeScene(new HomeScene());
            return true;
        case Qt::Key_B:
            changeScene(new BrowseScene());
            return true;
        case Qt::Key_G:
            changeScene(new PlugViewScene());
            return true;
        case Qt::Key_P:
            showFC();
            return true;
    }
    return false;
}

void MainGame::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);
    bg->setGeometry(rect());
    fixLogs();
    bg->lower();
    logLayWrap.raise();
}
void MainGame::fixLogs() {
    const int margin = 8;
    int w = width() / 3;
    logLayWrap.setFixedWidth(w);

    logLayWrap.move(width() - w - margin, 0);
    logLayWrap.resize(w, height());
}

bool MainGame::eventFilter(QObject *obj, QEvent *event) {
    // Update widget font
    if (event->type() == QEvent::Polish || event->type() == QEvent::PolishRequest) {
        QWidget* w = qobject_cast<QWidget*>(obj);
        if (w && !w->testAttribute(Qt::WA_SetFont)) {
            w->setFont(font());
        }
    }
    return QObject::eventFilter(obj, event);
}
