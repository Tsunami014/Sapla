#include "ovrlWrap.hpp"
#include "../core.hpp"
#include <QMouseEvent>
#include <QCoreApplication>
#include <QAbstractScrollArea>

QWidget* OverlayWrapper::getEventTarget(QWidget* w) {
    if (!w) return nullptr;
    if (auto* scrollArea = qobject_cast<QAbstractScrollArea*>(w)) {
        return scrollArea->viewport();
    }
    return w;
}

void OverlayWrapper::clearForwarding() {
    // End hover on last target
    if (hoverTarget_ && hoverTarget_ != pressTarget_) {
        QEvent leaveEv(QEvent::Leave);
        QCoreApplication::sendEvent(getEventTarget(hoverTarget_), &leaveEv);
    }
    // Safety: ensure any pressed target gets a release
    if (pressTarget_) {
        const QPoint gpos = QCursor::pos();
        QWidget* evTarget = getEventTarget(pressTarget_);
        QMouseEvent relEv(
            QEvent::MouseButtonRelease,
            evTarget->mapFromGlobal(gpos),
            evTarget->window()->mapFromGlobal(gpos),
            gpos,
            Qt::NoButton,
            Qt::NoButton,
            Qt::NoModifier
        );
        QCoreApplication::sendEvent(evTarget, &relEv);
    }
    hoverTarget_ = nullptr;
    pressTarget_ = nullptr;
}

bool OverlayWrapper::event(QEvent* ev) {
    switch (ev->type()) {
        case QEvent::MouseMove:
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
        case QEvent::MouseButtonDblClick: {
            auto* me = static_cast<QMouseEvent*>(ev);

            if (QWidget* overlayChild = childAt(me->pos())) {
                handleLeaveAll();
                return QWidget::event(ev);
            }

            const QPoint globalPos = mapToGlobal(me->pos());
            QWidget* scene = MG->curScene;
            QWidget* under = scene->childAt(scene->mapFromGlobal(globalPos));
            if (!under) under = scene;

            if (under)
                setCursor(under->cursor());

            if (ev->type() == QEvent::MouseButtonPress) {
                transitionHover(under);
                pressTarget_ = under;
                forwardMouse(pressTarget_, me, globalPos);
                return true;
            }

            if (ev->type() == QEvent::MouseMove) {
                if (pressTarget_) {
                    // While pressed, lock delivery
                    forwardMouse(pressTarget_, me, globalPos);
                    return true;
                } else {
                    transitionHover(under);
                    forwardMouse(under, me, globalPos);
                    return true;
                }
            }

            if (ev->type() == QEvent::MouseButtonRelease) {
                if (pressTarget_) {
                    forwardMouse(pressTarget_, me, globalPos);
                    pressTarget_ = nullptr;
                } else {
                    forwardMouse(under, me, globalPos);
                }
                // After release, update hover
                QWidget* newUnder = scene->childAt(scene->mapFromGlobal(globalPos));
                if (!newUnder) newUnder = scene;
                transitionHover(newUnder);
                return true;
            }

            return true;
        }

        case QEvent::Leave:
        case QEvent::WindowDeactivate:
        case QEvent::Hide: {
            handleLeaveAll();
            return QWidget::event(ev);
        }

        default:
            return QWidget::event(ev);
    }
}

void OverlayWrapper::forwardMouse(QWidget* target, QMouseEvent* src, const QPoint& globalPos) {
    QWidget* evTarget = getEventTarget(target);
    if (!evTarget) return;

    if (src->type() == QEvent::MouseButtonPress) {
        if ((target->focusPolicy() & Qt::ClickFocus)) {
            for (QWidget* p = target->parentWidget(); p; p = p->parentWidget()) {
                if (p->focusPolicy() & Qt::ClickFocus) { target = p; break; }
            }
        }
        target->setFocus(Qt::MouseFocusReason);
        if (QWidget* win = target->window()) {
            win->activateWindow();
        }
    }

    const QPointF localPos  = evTarget->mapFromGlobal(globalPos);
    const QPointF windowPos = evTarget->window()->mapFromGlobal(globalPos);

    auto* fwd = new QMouseEvent(
        src->type(),
        localPos,
        windowPos,
        globalPos,
        src->button(),
        src->buttons(),
        src->modifiers()
    );
    QCoreApplication::postEvent(evTarget, fwd);
}

void OverlayWrapper::transitionHover(QWidget* newTarget) {
    if (newTarget == hoverTarget_) return;
    if (hoverTarget_) {
        QEvent leaveEv(QEvent::Leave);
        QCoreApplication::sendEvent(getEventTarget(hoverTarget_), &leaveEv);
    }
    if (newTarget) {
        QEvent enterEv(QEvent::Enter);
        QCoreApplication::sendEvent(getEventTarget(newTarget), &enterEv);
    }
    hoverTarget_ = newTarget;
}

void OverlayWrapper::handleLeaveAll() {
    if (hoverTarget_) {
        QEvent leaveEv(QEvent::Leave);
        QCoreApplication::sendEvent(getEventTarget(hoverTarget_), &leaveEv);
        hoverTarget_ = nullptr;
    }
    if (pressTarget_) {
        const QPoint gpos = QCursor::pos();
        QWidget* evTarget = getEventTarget(pressTarget_);
        QMouseEvent relEv(
            QEvent::MouseButtonRelease,
            evTarget->mapFromGlobal(gpos),
            evTarget->window()->mapFromGlobal(gpos),
            gpos,
            Qt::NoButton,
            Qt::NoButton,
            Qt::NoModifier
        );
        QCoreApplication::sendEvent(evTarget, &relEv);
        pressTarget_ = nullptr;
    }
}
