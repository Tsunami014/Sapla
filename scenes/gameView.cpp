#include "gameView.hpp"
#include "../core.hpp"
#include "../help.hpp"
#include "../log.hpp"
#include "../games/getGames.hpp"
#include "../wids/listWid.hpp"
#include <QTextEdit>
#include <QFileDialog>
#include <QMessageBox>

struct ListData {
    QString name;
    QString path;
    QString text;
    bool working;
    bool isBI;
};
Q_DECLARE_METATYPE(ListData)

const QString MODULE = "GameBrowser";

void movFile(QString fromName, QString toName) {
    if (!QFile(fromName).rename(toName)) {
        // Fallback (e.g. when moving to a different drive)
        if (!QFile::copy(fromName, toName)) {
            Log::Error(MODULE) << "Unknown error when moving `" << fromName << "` to `" << toName << "` - copy faliure!";
            return;
        }
        if (!QFile::remove(fromName)) {
            Log::Warn(MODULE) << "Unknown error when moving `" << fromName << "` to `" << toName << "` - old file delete faliure! (File still loaded into plugin dir tho)";
        }
    }
}

GameViewScene::GameViewScene()
    : BaseScene(), m("Load game") {
        qRegisterMetaType<ListData>("GVSData");
        helpStr = &GAMEVIEW_HELP;
        MG->changeBG("dirt");
        connect(&m, &QAction::triggered, this, &GameViewScene::loadF);

        li = new ListWidget(this);
        li->setColumnCount(2);
        li->setHeaderLabels({"Status", "Name"});
        li->sortByColumn(1, Qt::AscendingOrder);
        li->setRootIsDecorated(false);
        li->setIndentation(0);
        li->setItemsExpandable(false);

        fillTree();

        txt = new QTextEdit(this);
        txt->setReadOnly(true);

        auto* disBtn = new SvgBtn(":/assets/btn.svg", this);
        disBtn->setText("Toggle disabled");
        connect(disBtn, &SvgBtn::clicked, this, [=](){
            QTreeWidgetItem* sel = li->currentItem();
            if (sel) {
                ListData dat = sel->data(0, Qt::UserRole).value<ListData>();
                if (dat.isBI) {
                    QString fpth = getDisBIGPath();
                    if (QFile::exists(fpth)) {
                        if (!QFile::remove(fpth)) {
                            Log::Warn(MODULE) << "Unknown error when deleting built-in game disabled file indicator at `" << fpth << "`";
                        }
                    } else {
                        QFile file(fpth);
                        if (file.open(QIODevice::WriteOnly)) {
                            file.close();
                        } else {
                            Log::Warn(MODULE) << "Unknown error when creating built-in game disabled file indicator at `" << fpth << "`";
                        }
                    }
                    reset();
                    return;
                }
                QString frpth = dat.path;
                QString topth;
                if (frpth.endsWith(".dis")) {
                    topth = frpth.sliced(0, frpth.size()-4);
                } else {
                    topth = frpth + ".dis";
                }

                clearGames();
                movFile(frpth, topth);
                reset();
            }
        });

        auto* delBtn = new SvgBtn(":/assets/btn.svg", this);
        delBtn->setText("Delete game");
        delBtn->setStyleSheet("color: red;");
        connect(delBtn, &SvgBtn::clicked, this, [=](){
            QTreeWidgetItem* sel = li->currentItem();
            if (sel) {
                ListData dat = sel->data(0, Qt::UserRole).value<ListData>();
                if (dat.isBI) {
                    Log::Warn(MODULE) << "Cannot delete built-in game!";
                } else {
                    auto reply = QMessageBox::question(this, "Are you sure?", 
                            QString("Are you sure you want to delete game \"%1\"?").arg(dat.name),
                        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
                    if (reply == QMessageBox::Yes) {
                        clearGames();
                        if (!QFile::remove(dat.path)) {
                            Log::Warn(MODULE) << "Unknown error when deleting game plugin `" << dat.path << "`!";
                            return;
                        }
                        reset();
                        Log::Info(MODULE) << "Deleted game plugin \"" + dat.name + "\"!";
                    }
                }
            }
        });

        auto* sideLay = new QVBoxLayout();
        sideLay->addWidget(disBtn);
        sideLay->addWidget(txt);
        sideLay->addWidget(delBtn);

        QWidget::connect(li, &QTreeWidget::itemSelectionChanged, [=](){
            QTreeWidgetItem* sel = li->currentItem();
            if (sel) {
                ListData dat = sel->data(0, Qt::UserRole).value<ListData>();
                txt->setText(dat.path + "\n" + dat.text);
            }
        });

        auto* mainLay = new QHBoxLayout(this);
        mainLay->addWidget(li, 4);
        mainLay->addLayout(sideLay, 1);
    }

void GameViewScene::reset() {
    loadGames();
    fillTree();
    li->clearSelection();
    txt->setText("");
}

void GameViewScene::fillTree() {
    li->clear();
    for (auto& dg : disabldGames) {
        auto* it = new QTreeWidgetItem(QStringList({"🤎", dg.name}));
        it->setData(0, Qt::UserRole, QVariant::fromValue(ListData{dg.name, dg.path, "Game disabled", false, dg.isBI}));
        li->addTopLevelItem(it);
    }
    for (auto& fg : failedGames) {
        auto* it = new QTreeWidgetItem(QStringList({"💔", fg.name}));
        it->setData(0, Qt::UserRole, QVariant::fromValue(ListData{fg.name, fg.path, "Game loaded with error:\n"+fg.error, false, fg.isBI}));
        li->addTopLevelItem(it);
    }
    for (auto* g : games) {
        auto* it = new QTreeWidgetItem(QStringList({"💖", g->name}));
        it->setData(0, Qt::UserRole, QVariant::fromValue(ListData{g->name, g->path, "Game loaded successfully!", true, g->isBI}));
        li->addTopLevelItem(it);
    }
}

void GameViewScene::loadF() {
    const QString filter = QString("Game plugin files (*%1)").arg(suffix);
    QStringList fnames = QFileDialog::getOpenFileNames(this, "Load game plugin file", QDir::homePath(), filter);
    QStringList deletes;
    enum runOs { GOING, Yall, Nall, EndAll };
    runOs opt = GOING;
    for (const QString& fname : fnames) {
        bool choice;
        switch (opt) {
            case EndAll:
                break;
            case Yall:
                choice = true;
                break;
            case Nall:
                choice = false;
                break;
            case GOING:
                auto reply = QMessageBox::question(this, "Load plugin type", 
                        "Loading plugins moves it to the configuration directory, "
                        "do you wish to keep the original file?\n"
                        "Current file: "+fname,
                    QMessageBox::Cancel | QMessageBox::Yes | QMessageBox::YesAll | QMessageBox::No | QMessageBox::NoAll, QMessageBox::Yes);
                switch (reply) {
                    case QMessageBox::YesAll:
                        opt = Yall;
                    case QMessageBox::Yes:
                        choice = true;
                        break;
                    case QMessageBox::NoAll:
                        opt = Nall;
                    case QMessageBox::No:
                        choice = false;
                        break;
                    default:
                        opt = EndAll;
                        break;
                }
                break;
        }
        if (opt == EndAll) break;
        if (!choice) {
            deletes.append(fname);
        }
    }
    if (opt == EndAll) {
        Log::Info(MODULE) << "Cancelled!";
        return;
    }
    for (const QString& fromName : fnames) {
        QFileInfo inf(fromName);
        QString toName = getGamesPath() + "/" + inf.fileName();
        if (QFile::exists(toName)) {
            if (!QFile::remove(toName)) {
                Log::Error(MODULE) << "Unknown error when deleting existing file `" << toName << "`!";
                continue;
            }
        }
        if (deletes.contains(fromName)) {
            movFile(fromName, toName);
        } else {
            if (!QFile::copy(fromName, toName)) {
                Log::Error(MODULE) << "Unknown error when copying `" << fromName << "` to `" << toName << "`!";
            }
        }
    }
    Log::Info(MODULE) << "Copied/moved " << fnames.size() << " game plugins!";
    reset();
}

