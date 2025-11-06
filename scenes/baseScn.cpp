#include "baseScn.hpp"
#include "../core.hpp"
#include <QLineEdit>
#include <QTextEdit>
#include <QApplication>

BaseScene::BaseScene() : QWidget(MG) {
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_TranslucentBackground);
    qApp->installEventFilter(this);
}
BaseScene::~BaseScene() {
    qApp->removeEventFilter(this);
}

bool isTextInput(QObject* o) {
    if (!o) return false;
    auto* w = qobject_cast<QWidget*>(o);
    if (!w) return false;
    while (w) {
        if (qobject_cast<QLineEdit*>(w) && qobject_cast<QLineEdit*>(w)->isEnabled() ||
            qobject_cast<QTextEdit*>(w) && qobject_cast<QTextEdit*>(w)->isEnabled()) {
            return true;
        }
        w = w->parentWidget();
    }
    return false;
}
bool BaseScene::keyEv(QKeyEvent* event) {
    return MG->handleEv(event);
}
bool BaseScene::eventFilter(QObject* obj, QEvent* event) {
    if (this != MG->curScene) {
        return false;
    }
    if (event->type() == QEvent::KeyPress) {
        auto* ke = static_cast<QKeyEvent*>(event);

        if (QWidget* modal = QApplication::activeModalWidget()) {
            if (ke->key() == Qt::Key_Escape) {
                modal->close();
            }
            return true;
        }
        if (isTextInput(qApp->focusObject()) && ke->key() != Qt::Key_Escape) {
            return QWidget::eventFilter(obj, event);
        }
        
        if (keyEv(ke)) return true;
    }
    return QWidget::eventFilter(obj, event);
}

