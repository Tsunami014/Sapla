#include "core.hpp"
#include "log.hpp"
#include "menu.hpp"
#include "base/svgRend.hpp"
#include <QTimer>
#include <QVBoxLayout>
#include <QApplication>
#include <QGraphicsOpacityEffect>
#include <QMessageBox>
#include <QPushButton>

bool doubleCheck(QString prompt) {
    QMessageBox msgBox;
    msgBox.setWindowTitle("Are you sure?");
    msgBox.setText(QString("Are you sure you want to %1?").arg(prompt));
    QAbstractButton* ybtn = msgBox.addButton("Yes", QMessageBox::YesRole);
    msgBox.setDefaultButton(msgBox.addButton("No", QMessageBox::NoRole));

    msgBox.exec();
    if (msgBox.clickedButton() == ybtn) {
        QMessageBox msgBox2;
        msgBox2.setWindowTitle("Are you REALLY sure?");
        msgBox2.setText(QString("Are you REALLY sure you want to %1?").arg(prompt));
        msgBox2.setDefaultButton(msgBox2.addButton("No", QMessageBox::YesRole));
        QAbstractButton* ybtn2 = msgBox2.addButton("Yes", QMessageBox::NoRole);
        msgBox2.exec();
        return msgBox2.clickedButton() == ybtn2;
    }
    return false;
}

constexpr std::string_view lnames[] = {
    "Debug",
    "Info",
    "Warning",
    "Error",
};
LogAlert::LogAlert(Log::Level lvl, QString msg)
    : SvgBtn(":/assets/errorDiags/"+QString::fromUtf8(lnames[lvl].data())+".svg") {
        setText(msg);

        connect(this, &SvgBtn::clicked, this, [this]() {
            deleteMe();
        });

        QTimer::singleShot(3000, this, [this]() {
            QWidget::setAttribute(Qt::WA_TransparentForMouseEvents);
            unsetCursor();

            QGraphicsOpacityEffect* opac = new QGraphicsOpacityEffect(this);
            opac->setOpacity(1);
            setGraphicsEffect(opac);
            qreal steps = 20;
            qreal interval = 500.0 / steps;
            int step = 0;
            QTimer* timer = new QTimer();
            QObject::connect(timer, &QTimer::timeout, [=]() mutable {
                opac->setOpacity(1.0 - (qreal(step) / steps));
                if (step++ > steps) {
                    timer->stop();
                    timer->deleteLater();
                    deleteMe();
                }
            });
            timer->start(interval);
        });
        MG->logLay.addWidget(this);
        QTimer::singleShot(0, MG, &MainGame::fixLogs);
    }
void LogAlert::deleteMe() {
    MG->logLay.removeWidget(this);
    deleteLater();
    MG->fixLogs();
}


namespace Log {
    std::vector<LogType> logs = {};
    Log::~Log() {
        QString lname = QString::fromUtf8(lnames[lvl].data());
        QString txt = QString("%1 [%2] %3").arg(lname).arg(mod).arg(buf);
        txt.replace("\n", "\n > ");
        switch (lvl) {
            case DEBUG: qDebug().noquote() << txt; break;
            case INFO: qInfo().noquote() << txt; break;
            case WARN: qWarning().noquote() << txt; break;
            case ERROR: qCritical().noquote() << txt; break;
        }
        if (!QApplication::closingDown()) {
            logs.emplace_back(lvl, buf, mod);
            if (lvl != DEBUG) {
                Level lvll = lvl;
                QTimer::singleShot(0, [lvll, txt](){
                    MG->logLay.addWidget(new LogAlert(lvll, txt));
                });
            }
        }
    }
}

constexpr std::string_view cols[] = {
    "#7AA2F7",  // Debug
    "#8AEA8A",  // Info
    "#F8F46A",  // Warn
    "#F79393",  // Error
};
void showLogWindow() {
    QString out;
    for (auto& l : Log::logs) {
        auto colour = QString::fromUtf8(cols[l.lvl].data());
        auto name = QString::fromUtf8(lnames[l.lvl].data());
        QString msg = l.msg;
        msg.replace("\n", "<br>\u00A0> ");
        out += QString("<span style='font-family: monospace;color:%1;'>%2 [%3] %4</span><br>")
            .arg(colour).arg(name).arg(l.mod).arg(msg);
    }
    QString col = QString("background-color: %1;").arg(MG->styls.logBgCol);
    makeDialog(out, "Logs", col);
}

