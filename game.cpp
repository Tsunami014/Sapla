#include "game.hpp"
#include "core.hpp"
#include "scenes/homeScn.hpp"
#include "scenes/browseScn.hpp"
#include "scenes/gameView.hpp"
#include "games/getGames.hpp"
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
    pendingScn = nullptr;
    resume = false;
    setFont(getFont(1.5));
    setStyleSheet(
        "QTextEdit {"
            "color: white;"
            "background: rgba(110, 60, 30, 220);"
            "border-radius: 6px;"
            "border: 1px solid #666;"
            "margin: 4px;"
            "padding: 4px;"
        "}"
        "QTextEdit:focus {"
            "border: 2px solid #333;"
        "}"
        "SvgBtn {"
            "color: #B87333;"
        "}"
        "QTreeWidget::item { background: rgb(193, 154, 107); }"
        "QTreeWidget {"
            "color: rgb(70, 35, 10);"
            "border: 1px solid #333;"
            "padding: 4px;"
        "}"
        "QTreeWidget::item:selected {"
            "background-color: rgba(210, 125, 45, 200);"
        "}"
        "QTreeWidget::item:hover {"
            "background-color: rgba(185, 115, 50, 140);"
        "}"
        "QHeaderView::section {"
            "background-color: rgb(90, 40, 10);"
            "border-top-left-radius: 10px;"
            "border-top-right-radius: 10px;"
            "padding: 4px;"
        "}"
    );
    bg = new SvgWidget(this);
    logLayWrap.setLayout(&logLay);
    logLay.addStretch();
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

void MainGame::changeScene(BaseScene* newScene, bool resm) {
    GameScene* newGame = dynamic_cast<GameScene*>(newScene);
    if (!newGame) {
        GameScene* game = dynamic_cast<GameScene*>(curScene);
        if (game && game != newScene) {
            game->hide();
            game->setParent(sceneStash);
            curGame = game;
        }
    } else {
        if (curGame && curGame != newGame) {
            curGame->deleteLater();
        }
        curGame = newGame;
    }
    logLayWrap.clearForwarding();

    resume = resm && newGame;
    if (!pendingScn) {
        if (QWidget* w = QApplication::widgetAt(QCursor::pos())) {
            QEvent leaveEv(QEvent::Leave);
            QCoreApplication::sendEvent(w, &leaveEv);
        }
        QTimer::singleShot(0, this, [this]() {
            setCentralWidget(pendingScn);
            if (!pendingScn->isVisible()) pendingScn->show();
            curScene = pendingScn;
            BaseScene* nscn = pendingScn;
            pendingScn = nullptr;
            if (resume) {
                if (auto* ngame = dynamic_cast<GameScene*>(nscn)) {
                    ngame->resume();
                }
                resume = false;
            }
            bg->lower();
            logLayWrap.raise();

            QCursor::setPos(QCursor::pos());  // Force cursor update
        });
    } else {
        if (pendingScn && curGame != pendingScn) {
            pendingScn->deleteLater();
        }
    }
    pendingScn = newScene;
}

void MainGame::showFC() {
    if (curGame != nullptr) {
        changeScene(curGame, true);
    } else {
        nextFC();
    }
}
void MainGame::nextFC() {
    int gSze = games.size();
    if (gSze == 0) return;
    int gidx = QRandomGenerator::global()->bounded(gSze);
    for (int _ = 0; _ < gSze; _++) {
        bool success = games[gidx]->run();
        if (success) break;
        gidx = (gidx + 1) % gSze;
    }
}

bool MainGame::cardFin(const FlashCard& card, bool correct) {
    if (correct) s.goods++;
    else s.bads++;

    int grow = correct ? 20 : 50;
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
            changeScene(new GameViewScene());
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

