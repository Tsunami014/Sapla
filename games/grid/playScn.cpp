#include "playScn.hpp"
#include "items/cardIt.hpp"
#include "items/cardLayouts.hpp"
#include <QEvent>
#include <QKeyEvent>
#include <QRandomGenerator>

const QString HELP_TXT =
    "Click on card to flip, once flipped; &lt;Space&gt; if you got it wrong, &lt;Enter&gt; if right.\n"
    "Press &lt;Esc&gt; to go back to the home screen.";

PlayScene::PlayScene()
    : GameScene(), main(new GLayoutGraphicItem()), pb(), tr() {
        helpStr = &HELP_TXT;
        MG->changeBG("pretty");

        scn.addItem(main);
        scn.addItem(&pb);
        scn.addItem(&tr);

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
                scn.removeItem(it.item);
                break;
            }
        }
    }
    scn.removeItem(main);
    delete main;
}

void PlayScene::pauseTimer() {
    timer.stop();
    timeOffset += elapsed.elapsed();
}
void PlayScene::resumeTimer() {
    resetTimer();
}
void PlayScene::dialogOpen() {
    if (!hasOverlay()) {
        pauseTimer();
    }
}
void PlayScene::dialogClose() {
    if (!hasOverlay()) {
        resumeTimer();
    }
}

void PlayScene::resetTimer() {
    elapsed.restart();
    if (!timer.isActive()) {
        timer.start(16); // ~60 FPS
    }
}
int PlayScene::addAnother() {
    const FlashCard fc = *NextFC();
    auto lay = Single;
    auto* nCGI = new CardGraphicItem(lay, fc);
    if (!main->addItem(nCGI)) {
        delete nCGI;
        // Failed to add a new item; you lose some growth
        tr.grow(-50);
    }
    main->update();

    // Later TODO: Change time based on how difficult card is
    return 2000 + QRandomGenerator::global()->bounded(4001);  // Random between 2 and 6 seconds
}

bool PlayScene::keyEv(QKeyEvent* event) {
    if (MG->handleEv(event)) return true;
    int key = event->key();

    if (overlay != NULL && (key == Qt::Key_Space || key == Qt::Key_Enter || key == Qt::Key_Return)) {
        for (auto& it : main->grid) {
            if (it.item->side == 255) {
                if (key == Qt::Key_Space) {
                    MG->s.bads++;
                    tr.grow(-10);
                } else {
                    MG->s.goods++;
                    if (!tr.grow(50)) {
                        it.item->finish();
                        MG->changeScene(new WinScene());
                        return true;
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
            return true;
        }
    }
    return false;
}

void PlayScene::resize() {
    QRectF rect = view.sceneRect();
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
    overlay->setRect(view.sceneRect());
}
bool PlayScene::hasOverlay() {
    return overlay != NULL;
}
void PlayScene::removeOverlay() {
    if (hasOverlay()) {
        scn.removeItem(overlay);
        delete overlay;
        overlay = NULL;
    }
}

