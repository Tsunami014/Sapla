#include "loseScn.hpp"
#include "playScn.hpp"
#include "homeScn.hpp"
#include "../main.hpp"
#include <QEvent>
#include <QTimer>
#include <QTextDocument>

LoseScene::LoseScene(Stats s) : txt(this) {
    txt.setPlainText(QString::fromStdString(
        "You lost!\nPress any button to try again\n"
        "You amassed 💰 " + std::to_string(s.coins) + "\n"
        "With " + std::to_string(s.successes) + " correct cards and " + std::to_string(s.faliures) + " wrong cards"
    ));
    txt.setScale(2);
    QTextOption opt = txt.document()->defaultTextOption();
    opt.setAlignment(Qt::AlignHCenter);
    txt.document()->setDefaultTextOption(opt);
}

void LoseScene::onEvent(QEvent* event) {
    if (event->type() == QEvent::KeyPress) {
        QTimer::singleShot(0, [this]() { MG->changeScene(new HomeScene()); });
    }
}

void LoseScene::resize() {
    txt.setTextWidth(rect.width() / 2.0);

    qreal textHeight = txt.document()->size().height();
    qreal y = (rect.height() - textHeight) / 2.0;
    if (y < 0) y = 0;
    txt.setPos(0, y);
}

