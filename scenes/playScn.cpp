#include "playScn.hpp"
#include "../cards/getCards.hpp"
#include "../main.hpp"
#include <QEvent>
#include <QKeyEvent>

PlayScene::PlayScene() : 
    BaseScene(), main(new GLayoutGraphicItem(this)), pb(new ProgressBarItem(this)) {
        overlay = NULL;
        for (auto& c : cards) {
            main->addItem(c->getItem());
        }

        timeLeft = 10000;

        pb->setZValue(2);
        QObject::connect(&timer, &QTimer::timeout, [this]() {
            double progress = (elapsed.elapsed() + timeOffset) / double(timeLeft);
            if (progress >= 1) {
                resetTimer();
                progress = 0;
            }
            pb->setValue(progress);
        });
        resetTimer();
    }

void PlayScene::pauseTimer() {
    timer.stop();
    timeOffset += elapsed.elapsed();
}
void PlayScene::resumeTimer() {
    double offs = timeOffset;
    resetTimer();
    timeOffset = offs;
}

void PlayScene::resetTimer() {
    timeOffset = 0;
    elapsed.restart();
    if (!timer.isActive()) {
        timer.start(16); // ~60 FPS
    }
}

void PlayScene::onEvent(QEvent* event) {
    if (event->type() == QEvent::KeyPress) {
        auto* keyEvent = (QKeyEvent*)event;
        int key = keyEvent->key();

        if (key == Qt::Key_Space && overlay != NULL) {
            for (auto& it : main->grid) {
                if (it.item->side == 255) {
                    it.item->finish();
                    break;
                }
            }
        }
    }
}

void PlayScene::resize() {
    main->setRect(rect);
    pb->setRect(rect);
    if (overlay != NULL) {
        overlay->setRect(rect);
    }
}

void PlayScene::setOverlay(QGraphicsRectItem* ovl) {
    overlay = ovl;
    overlay->setRect(rect);
}
bool PlayScene::hasOverlay() {
    return overlay != NULL;
}
void PlayScene::removeOverlay() {
    MScene->removeItem(overlay);
    overlay = NULL;
}

