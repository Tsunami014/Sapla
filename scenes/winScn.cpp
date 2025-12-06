#include "winScn.hpp"
#include "../core.hpp"
#include "../items/treeItem.hpp"
#include "../base/svgWid.hpp"
#include <QEvent>
#include <QLabel>
#include <QTimer>
#include <QTextDocument>
#include <QKeyEvent>
#include <QHBoxLayout>

const QString HELP_TXT = "&lt;Esc&gt; to resume playing";

WinScene::WinScene() {
    helpStr = &HELP_TXT;
    MG->changeBG("win");

    auto* tr = new SvgWidget(this);
    tr->renderer()->load(rendTreePhase(-1));
    auto* lab = new QLabel(this);
    lab->setText(QString::fromStdString("<h1>"
        "Your tree grew!"
    "</h1>"));
    lab->setAlignment(Qt::AlignCenter);
    lab->setWordWrap(true);

    auto* lay = new QHBoxLayout(this);
    lay->addWidget(lab, 2);
    lay->addWidget(tr, 1);
}
bool WinScene::keyEv(QKeyEvent* event) {
    if (event->key() == Qt::Key_Escape) {
        MG->showFC();
        return true;
    }
    return MG->handleEv(event);
}

