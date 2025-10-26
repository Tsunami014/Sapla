#include "main.hpp"
#include "log.hpp"
#include "menu.hpp"
#include <QWidget>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QTimer>
#include <QGraphicsScene>
#include <QPropertyAnimation>

constexpr std::string_view lnames[] = {
    "Debug",
    "Info",
    "Warning",
    "Error",
};
LogAlert::LogAlert(Log::Level lvl, QString msg)
    : TxtBtnItem(":/assets/errorDiags/"+QString::fromUtf8(lnames[lvl].data())+".svg", msg) {
        setTxtColour(Qt::black);
        QTextOption opt;
        opt.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
        opt.setAlignment(Qt::AlignHCenter);
        txt->document()->setDefaultTextOption(opt);

        bool* alive = new bool(true);
        MG->logs.insert(MG->logs.begin(), this);
        onClick = [this, alive]() {
            if (*alive) {
                *alive = false;
                deleteMe();
            }
        };

        QTimer::singleShot(2000, [this, alive]() {
            if (!*alive) {
                return;
            }
            *alive = false;
            setAcceptHoverEvents(false);
            if (hovering) {
                hovering = false;
                unsetCursor();
                MScene->views()[0]->viewport()->unsetCursor();
            }
            qreal steps = 20;
            qreal interval = 1000.0 / steps;
            int step = 0;
            QTimer* timer = new QTimer();
            QObject::connect(timer, &QTimer::timeout, [=]() mutable {
                setOpacity(1.0 - (qreal(step) / steps));
                if (step++ > steps) {
                    timer->stop();
                    timer->deleteLater();
                    deleteMe();
                }
            });
            timer->start(interval);
        });
    }
void LogAlert::deleteMe() {
    auto it = std::find(MG->logs.begin(), MG->logs.end(), this);
    if (it != MG->logs.end()) {
        MG->logs.erase(it);
    }
    scene()->removeItem(this);
    if (hovering) {
        MScene->views()[0]->viewport()->unsetCursor();
    }
    MG->updateLogs();
    delete this;
}


namespace Log {
    std::vector<LogType> logs = {};
    Log::~Log() {
        logs.emplace_back(lvl, buf, mod);
        QString lname = QString::fromUtf8(lnames[lvl].data());
        QString txt = QString("%1 [%2] %3").arg(lname).arg(mod).arg(buf);
        txt.replace("\n", "\n > ");
        switch (lvl) {
            case DEBUG: qDebug().noquote() << txt; break;
            case INFO: qInfo().noquote() << txt; break;
            case WARN: qWarning().noquote() << txt; break;
            case ERROR: qCritical().noquote() << txt; break;
        }
        if (lvl != DEBUG) {
            Level lvll = lvl;
            QTimer::singleShot(0, [lvll, txt](){
                auto* la = new LogAlert(lvll, txt);
                la->setZValue(9999);
                MScene->addItem(la);
                MG->updateLogs();
            });
        }
    }
}

constexpr std::string_view colours[] = {
    "#7AA2F7",  // Debug
    "#8AEA8A",  // Info
    "#F8F46A",  // Warn
    "#F79393",  // Error
};
void showLogWindow() {
    QString out;
    for (auto& l : Log::logs) {
        auto colour = QString::fromUtf8(colours[l.lvl].data());
        auto name = QString::fromUtf8(lnames[l.lvl].data());
        QString msg = l.msg;
        msg.replace("\n", "<br>\u00A0> ");
        out += QString("<span style='font-family: monospace;color:%1;'>%2 [%3] %4</span><br>")
            .arg(colour).arg(name).arg(l.mod).arg(msg);
    }
    makeDialog(out, "Logs");
}

