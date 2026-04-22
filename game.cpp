#include "game.hpp"
#include "core.hpp"
#include "scenes/homeScn.hpp"
#include "scenes/browseScn.hpp"
#include "scenes/pluginView.hpp"
#include "plugins/getPlugins.hpp"
#include "notes/note.hpp"
#include "base/font.hpp"
#include "base/svgRend.hpp"
#include <QRandomGenerator>
#include <QTimer>
#include <QKeyEvent>
#include <QApplication>
#include <QLineEdit>
#include <QComboBox>

MainGame::MainGame() : logLay(), logLayWrap(this), curGame(nullptr) {
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

void MainGame::removeGame(bool force) {
    if (curGame != nullptr) {
        if (force) {
            delete curGame;
        } else {
            curGame->deleteLater();
        }
    }
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
    if (old && old != curGame) old->deleteLater();
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
    if (gSze == 0) {
        Log::Warn("Game") << "Could not find any game to run!";
        return;
    }
    int gidx = QRandomGenerator::global()->bounded(gSze);
    bool success = false;
    for (int _ = 0; _ < gSze; _++) {
        success = games[gidx]();
        if (success) break;
        gidx = (gidx + 1) % gSze;
    }
    if (!success) {
        Log::Warn("Game") << "Could not find any game willing to run!";
    }
}

void MainGame::editCard(FlashCard* card) {
    changeScene(new BrowseScene(card->parent));
}
int MainGame::cardFin(FlashCard* card, int key, Tree* tree) {
    if (key == Qt::Key_E) {
        editCard(card);
        return -1;
    }
    int rating;
    if (key >= Qt::Key_1 && key < Qt::Key_1+ScheduleInfo.ratesLen()) {
        rating = key - Qt::Key_1;
    } else if (key == Qt::Key_Minus || key == Qt::Key_Equal) {
        rating = key == Qt::Key_Minus ? -2 : -1;
    } else {
        return -1;
    }
    float sco = card->update(rating);
    if (sco == 0) sco = -0.5; // So it's still increasing
    card->parent->updateSchedules();
    float min = *std::min_element(ScheduleInfo.ratingScos.begin(), ScheduleInfo.ratingScos.end());
    float sze = *std::max_element(ScheduleInfo.ratingScos.begin(), ScheduleInfo.ratingScos.end()) - min;
    int grow = ((sco+min)/sze) * 10;
    return tree->grow(grow, sco > 0);
}

bool MainGame::handleEv(QKeyEvent* event) {
    if (dialogging) return false;
    QWidget* fw = QApplication::focusWidget();
    if (fw) {
        if (qobject_cast<QLineEdit*>(fw) ||
            qobject_cast<QTextEdit*>(fw) ||
            qobject_cast<QComboBox*>(fw)) {
            return false;
        }
    }

    switch (event->key()) {
        case Qt::Key_Escape:
        case Qt::Key_H:
            changeScene(new HomeScene());
            return true;
        case Qt::Key_B:
            changeScene(new BrowseScene());
            return true;
        case Qt::Key_P:
            changeScene(new PlugViewScene());
            return true;
        case Qt::Key_G:
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
