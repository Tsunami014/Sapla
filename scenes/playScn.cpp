#include "playScn.hpp"
#include "winScn.hpp"
#include "../cards/getCards.hpp"
#include "../main.hpp"
#include <QEvent>
#include <QKeyEvent>
#include <QRandomGenerator>

PlayScene::PlayScene() : 
    BaseScene(), main(new GLayoutGraphicItem(this)), pb(this), tr(this) {
        s.successes = 0;
        s.faliures = 0;

        overlay = NULL;

        pb.setZValue(2);
        QObject::connect(&timer, &QTimer::timeout, [this]() {
            if (main->grid.size() >= 3) {
                tr.grow(-0.05*(main->grid.size()-2));
            }
            double progress = (elapsed.elapsed() + timeOffset) / double(timeLeft);
            if (progress >= 1) {
                resetTimer();
                progress = 0;
            }
            pb.setValue(progress);
        });
        resetTimer();
    }

void PlayScene::pauseTimer() {
    timer.stop();
    timeOffset += elapsed.elapsed();
}
void PlayScene::resumeTimer() {
    resetTimer(false);
}

void PlayScene::resetTimer(bool add) {
    if (add) {
        int cardsAmnt = cards.size();
        int idx = QRandomGenerator::global()->bounded(cardsAmnt);
        if (cardsAmnt >= main->Cols*2) {
            int tries = 0;
            while (tries < cardsAmnt) {
                auto& card = cards[idx];
                bool good = true;
                for (auto& it : main->grid) {
                    if (*card == *it.item) {
                        good = false;
                        break;
                    }
                }
                if (good) {
                    break;
                }
                idx = (idx + 1) % cardsAmnt;
                tries++;
            }
        }

        CardGraphicItem* newItem = cards[idx]->getItem();
        if (!main->addItem(newItem)) {
            delete newItem;
            // Failed to add a new item; you lose a lot of growth
            tr.grow(-150);
        }
        main->update();

        timeOffset = 0;
        // Later TODO: Change time based on how difficult card is
        timeLeft = 4000 + QRandomGenerator::global()->bounded(2001);  // Random between 4 and 6 seconds
    }
    elapsed.restart();
    if (!timer.isActive()) {
        timer.start(16); // ~60 FPS
    }
}

void PlayScene::onEvent(QEvent* event) {
    if (event->type() == QEvent::KeyPress) {
        auto* keyEvent = (QKeyEvent*)event;
        int key = keyEvent->key();

        if (overlay != NULL && (key == Qt::Key_Space || key == Qt::Key_Enter || key == Qt::Key_Return)) {
            for (auto& it : main->grid) {
                if (it.item->side == 255) {
                    if (key == Qt::Key_Space) {
                        s.faliures++;
                        tr.grow(-10);
                    } else {
                        s.successes++;
                        if (!tr.grow(50)) {
                            QTimer::singleShot(0, [this]() { MG->changeScene(new WinScene(s)); });
                        }
                    }
                    it.item->finish();
                    if (main->grid.empty()) {
                        resetTimer();
                    }
                    break;
                }
            }
        }
    }
}

void PlayScene::resize() {
    main->setRect(rect);
    tr.setRect(rect);
    if (overlay != NULL) {
        overlay->setRect(rect);
    }
    pb.setRect({
        rect.x() + rect.width()*0.1, rect.y() + rect.height()*0.92,
        rect.width()*0.8, rect.height()*0.06
    });
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

