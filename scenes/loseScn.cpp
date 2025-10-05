#include "loseScn.hpp"
#include "playScn.hpp"
#include "../main.hpp"
#include <QEvent>
#include <QTimer>
#include <QTextDocument>

LoseScene::LoseScene() : txt(this) {
    txt.setPlainText("You lost!\nPress any button to try again");
    txt.setScale(2);
    QTextOption opt = txt.document()->defaultTextOption();
    opt.setAlignment(Qt::AlignHCenter);
    txt.document()->setDefaultTextOption(opt);
}

void LoseScene::onEvent(QEvent* event) {
    if (event->type() == QEvent::KeyPress) {
        QTimer::singleShot(0, [this]() { MG->changeScene(new PlayScene()); });
    }
}

void LoseScene::resize() {
    txt.setTextWidth(rect.width() / 2.0);

    qreal textHeight = txt.document()->size().height();
    qreal y = (rect.height() - textHeight) / 2.0;
    if (y < 0) y = 0;
    txt.setPos(0, y);
}

