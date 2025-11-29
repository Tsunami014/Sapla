#include "playScn.hpp"
#include "items/cardLayouts.hpp"
#include <QEvent>
#include <QKeyEvent>
#include <QRandomGenerator>

const QString HELP_TXT =
    "Click on card to flip, once flipped; &lt;Space&gt; if you got it wrong, &lt;Enter&gt; if right.\n"
    "Press &lt;Esc&gt; to go back to the home screen.";

void PlayScene::resume() {
    if (main->grid.empty()) {
        MG->nextFC();
        return;
    }
    helpStr = &HELP_TXT;
    MG->changeBG("pretty");
}
PlayScene::PlayScene()
    : GraphicGameScene(), main(new GLayoutGraphicItem()), tr(Tree::getTree()) {
        scn.addItem(main);
        scn.addItem(&tr);

        overlay = NULL;

        while (true) {
            const FlashCard* fc = NextFC();
            auto lay = Single;
            auto* nCGI = new GridCGI(lay.fname, fc);
            if (!main->addItem(nCGI, lay)) {
                delete nCGI;
                break;
            }
        }
        main->update();
        resume();
    }
PlayScene::~PlayScene() {
    if (tr.scene() == &scn) {
        scn.removeItem(&tr);
    }
    scn.removeItem(main);
    delete main;
}


bool PlayScene::keyEv(QKeyEvent* event) {
    if (MG->handleEv(event)) return true;
    int key = event->key();

    if (hasOverlay() && (key == Qt::Key_Space || key == Qt::Key_Enter || key == Qt::Key_Return)) {
        for (auto& it : main->grid) {
            if (it.item->side == 255) {
                removeOverlay();
                const FlashCard* fc = it.item->fc;
                scn.removeItem(it.item);
                main->removeItem(it.item);
                main->update();
                main->updateAllChildren();
                if (MG->cardFin(fc, key != Qt::Key_Space)) return true;
                resume();  // Only for checking if empty
                return true;
            }
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

