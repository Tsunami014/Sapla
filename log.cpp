#include "main.hpp"
#include "log.hpp"
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
            showLogWindow();
            deleteMe();
            *alive = false;
        };

        QTimer::singleShot(5000, [this, alive]() {
            if (*alive) {
                onClick = nullptr;
                *alive = false;
            } else {
                return;
            }
            setAcceptHoverEvents(false);
            if (hovering) {
                hovering = false;
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
                    delete timer;
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
    if (hovering) {
        MScene->views()[0]->viewport()->unsetCursor();
    }
    scene()->removeItem(this);
    delete this;
}


namespace Log {
    std::vector<LogType> logs = {};
    Log::~Log() {
        logs.emplace_back(lvl, buf, mod);
        QString lname = QString::fromUtf8(lnames[lvl].data());
        QString txt = QString("%1 [%2] %3").arg(lname).arg(mod).arg(buf);
        switch (lvl) {
            case DEBUG: qDebug() << txt; break;
            case INFO: qInfo() << txt; break;
            case WARN: qWarning() << txt; break;
            case ERROR: qCritical() << txt; break;
        }
        Level lvll = lvl;
        QTimer::singleShot(0, [lvll, txt](){
            auto* la = new LogAlert(lvll, txt);
            la->setZValue(9999);
            MScene->addItem(la);
        });
    }
}

constexpr std::string_view colours[] = {
    "#7AA2F7",  // Debug
    "#8AEA8A",  // Info
    "#F8F46A",  // Warn
    "#F79393",  // Error
};
class TextWindow : public QWidget {
public:
    TextWindow(QWidget* parent = nullptr) : QWidget(parent) {
        setWindowTitle("Logs");

        QTextEdit* txt = new QTextEdit;
        txt->setReadOnly(true);

        QString out;
        for (auto& l : Log::logs) {
            QString colour = QString::fromUtf8(colours[l.lvl].data());
            QString name = QString::fromUtf8(lnames[l.lvl].data());
            out += QString("<span style='color:%1;'>%2 [%3] %4</span><br>").arg(colour).arg(name).arg(l.mod).arg(l.msg);
        }
        txt->setHtml(out);

        QVBoxLayout *layout = new QVBoxLayout;
        layout->addWidget(txt);
        setLayout(layout);

        resize(500, 700);
    }
};

void showLogWindow() {
    TextWindow* w = new TextWindow();
    w->setAttribute(Qt::WA_DeleteOnClose);
    w->show();
}

