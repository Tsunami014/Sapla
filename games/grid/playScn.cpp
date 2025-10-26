#include "playScn.hpp"
#include "winScn.hpp"
#include "items/cardIt.hpp"
#include "items/cardLayouts.hpp"
#include <QEvent>
#include <QKeyEvent>
#include <QRandomGenerator>

const QString HELP_TXT =
    "Click on card to flip, once flipped; &lt;Space&gt; if you got it wrong, &lt;Enter&gt; if right.\n"
    "Press &lt;Esc&gt; to go back to the home screen.";

PlayScene::PlayScene() : 
    BaseScene(), main(new GLayoutGraphicItem(this)), pb(this), tr(this) {
        helpStr = &HELP_TXT;
        MG->changeBG("pretty");

        overlay = NULL;

        QObject::connect(&timer, &QTimer::timeout, [this]() {
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
PlayScene::~PlayScene() {
    if (overlay) {
        for (auto& it : main->grid) {
            if (it.item->side != 0) {
                it.item->finish();
                break;
            }
        }
    }
}

void PlayScene::pauseTimer() {
    timer.stop();
    timeOffset += elapsed.elapsed();
}
void PlayScene::resumeTimer() {
    resetTimer();
}
void PlayScene::dialogOpen() {
    pauseTimer();
}
void PlayScene::dialogClose() {
    resumeTimer();
}

void PlayScene::resetTimer() {
    elapsed.restart();
    if (!timer.isActive()) {
        timer.start(16); // ~60 FPS
    }
}
int PlayScene::addAnother() {
    layout lay = Single;
    int cardsAmnt = cards.size();
    int idx = QRandomGenerator::global()->bounded(cardsAmnt);
    FlashCard newFc;
    if (cardsAmnt >= main->Cols*2) {
        int tries = 0;
        while (tries < cardsAmnt) {
            auto& card = cards[idx];
            int fcAmnt = card->flashCs.size();
            int fcIdx = QRandomGenerator::global()->bounded(fcAmnt);
            int tries2 = 0;
            bool good = true;
            while (tries2 < fcAmnt) {
                auto& fc = card->flashCs[fcIdx];
                for (auto& it : main->grid) {
                    if (*it.item == fc) {
                        good = false;
                        break;
                    }
                }
                if (good) {
                    newFc = fc;
                    break;
                }
                fcIdx = (fcIdx + 1) % fcAmnt;
                tries2++;
            }
            if (good) break;
            idx = (idx + 1) % cardsAmnt;
            tries++;
        }
    } else {
        int fcIdx = QRandomGenerator::global()->bounded(int(cards[idx]->flashCs.size()));
        newFc = cards[idx]->flashCs[fcIdx];
    }

    auto* nCGI = new CardGraphicItem(lay, newFc);
    if (!main->addItem(nCGI)) {
        delete nCGI;
        // Failed to add a new item; you lose some growth
        tr.grow(-50);
    }
    main->update();

    // Later TODO: Change time based on how difficult card is
    return 2000 + QRandomGenerator::global()->bounded(4001);  // Random between 2 and 6 seconds
}

void PlayScene::onEvent(QEvent* event) {
    if (event->type() == QEvent::KeyPress) {
        auto* keyEvent = (QKeyEvent*)event;
        int key = keyEvent->key();

        if (key == Qt::Key_Escape) {
            auto* keyEvent = (QKeyEvent*)event;
            if (keyEvent->key() == Qt::Key_Escape) MG->changeScene(new HomeScene());
        }
        if (overlay != NULL && (key == Qt::Key_Space || key == Qt::Key_Enter || key == Qt::Key_Return)) {
            for (auto& it : main->grid) {
                if (it.item->side != 0) {
                    if (key == Qt::Key_Space) {
                        MG->s.bads++;
                        tr.grow(-10);
                    } else {
                        MG->s.goods++;
                        if (!tr.grow(50)) {
                            MG->changeScene(new WinScene());
                        }
                    }
                    it.item->finish();
                    if (tr.isDone()) break;
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
    delete overlay;
    overlay = NULL;
}

