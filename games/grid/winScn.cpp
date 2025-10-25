#include "game.hpp"
#include "winScn.hpp"
#include "playScn.hpp"
#include <QEvent>
#include <QTimer>
#include <QTextDocument>
#include <QKeyEvent>

const QString HELP_TXT = "&lt;Esc&gt; to go home";

WinScene::WinScene(Stats s) : txt(this), tr(this) {
    helpStr = &HELP_TXT;
    MG->changeBG("win");
    tr.lastPhase();
    txt.setPlainText(QString::fromStdString(
        "You win!\n"
        "You had " + std::to_string(s.successes) + " correct cards and " + std::to_string(s.faliures) + " wrong cards"
    ));
    txt.setScale(2);
    QTextOption opt = txt.document()->defaultTextOption();
    opt.setAlignment(Qt::AlignHCenter);
    txt.document()->setDefaultTextOption(opt);
}

void WinScene::onEvent(QEvent* event) {
    if (event->type() == QEvent::KeyPress) {
        auto* keyEvent = (QKeyEvent*)event;
        if (keyEvent->key() == Qt::Key_Escape) MG->changeScene(new HomeScene());
    }
}

void WinScene::resize() {
    tr.setRect(rect);
    txt.setTextWidth(tr.boundingRect().left() / 2.0);

    qreal textHeight = txt.document()->size().height();
    qreal y = (rect.height() - textHeight) / 2.0;
    if (y < 0) y = 0;
    txt.setPos(0, y);
}

