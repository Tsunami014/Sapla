#include "playScn.hpp"
#include "winScn.hpp"
#include "../cards/getCards.hpp"
#include "../main.hpp"
#include <QEvent>
#include <QKeyEvent>
#include <QRandomGenerator>

PlayScene::PlayScene() : 
    BaseScene(), main(new GLayoutGraphicItem(this)), pb(this), tr(this) {
        MG->setBottomTxt("Click on card to flip, once flipped; <Space> if you got it wrong, <Enter> if right");
        MG->changeBG("pretty");
        s.successes = 0;
        s.faliures = 0;

        overlay = NULL;

        QObject::connect(&timer, &QTimer::timeout, [this]() {
            if (main->grid.size() >= 3) {
                tr.grow(-0.05*(main->grid.size()-2));
            }
            double progress = (elapsed.elapsed() + timeOffset) / double(timeLeft);
            if (progress >= 1) {
                timeOffset = 0;
                resetTimer();
                timeLeft = addAnother();
                progress = 0;
            }
            pb.setValue(progress);
        });
        timeOffset = 0;
        resetTimer();
        timeLeft = addAnother();
    }

void PlayScene::pauseTimer() {
    timer.stop();
    timeOffset += elapsed.elapsed();
}
void PlayScene::resumeTimer() {
    resetTimer();
}

void PlayScene::resetTimer() {
    elapsed.restart();
    if (!timer.isActive()) {
        timer.start(16); // ~60 FPS
    }
}
int PlayScene::addAnother() {
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

    // Later TODO: Change time based on how difficult card is
    return 2000 + QRandomGenerator::global()->bounded(4001);  // Random between 2 and 6 seconds
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
                        int remaining = timeLeft - timeOffset;
                        int existingTime = qMin(remaining, 2000);  // Max of 2 seconds carried in from before
                        int fixedOffs = qMin(int(timeOffset), 3000);  // Max of 3 seconds of already completed time from before
                        timeOffset = fixedOffs;
                        timeLeft = existingTime + addAnother() + fixedOffs;
                    }
                    break;
                }
            }
        }
    }
}

void PlayScene::resize() {
    tr.setRect(rect);
    main->setRect({rect.x(), rect.y(), rect.width()-tr.boundingRect().width(), rect.height()});
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

