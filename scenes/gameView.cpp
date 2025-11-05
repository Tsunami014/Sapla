#include "gameView.hpp"
#include "../core.hpp"
#include "../log.hpp"
#include "../games/getGames.hpp"
#include "../wids/listWid.hpp"
#include <QTextEdit>
#include <QFileDialog>
#include <QMessageBox>

struct ListData {
    QString text;
    bool working;
};
Q_DECLARE_METATYPE(ListData)

GameViewScene::GameViewScene()
    : BaseScene(), m("Load game") {
        qRegisterMetaType<ListData>("GVSData");
        MG->changeBG("dirt");
        connect(&m, &QAction::triggered, this, &GameViewScene::loadF);

        auto* li = new ListWidget(this);
        li->setHeaderHidden(true);
        li->setRootIsDecorated(false);
        li->setIndentation(0);
        li->setItemsExpandable(false);

        for (auto& fg : failedGames) {
            auto* it = new QTreeWidgetItem(QStringList("💔 "+fg.first));
            it->setData(0, Qt::UserRole, QVariant::fromValue(ListData{"Game loaded with error:\n"+fg.second, false}));
            li->addTopLevelItem(it);
        }
        for (auto* g : games) {
            auto* it = new QTreeWidgetItem(QStringList("💖 "+g->name));
            it->setData(0, Qt::UserRole, QVariant::fromValue(ListData{"Game loaded successfully!", true}));
            li->addTopLevelItem(it);
        }

        auto* txt = new QTextEdit(this);
        txt->setDisabled(true);

        auto* sideLay = new QVBoxLayout();
        sideLay->addWidget(txt);

        QWidget::connect(li, &QTreeWidget::itemSelectionChanged, [=](){
            QTreeWidgetItem* sel = li->currentItem();
            if (sel) {
                ListData dat = sel->data(0, Qt::UserRole).value<ListData>();
                txt->setText(dat.text);
            }
        });

        auto* mainLay = new QHBoxLayout(this);
        mainLay->addWidget(li, 4);
        mainLay->addLayout(sideLay, 1);
    }

void GameViewScene::loadF() {
    const QString MODULE = "GameLoadDialog";
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
            if (!QFile(fromName).rename(toName)) {
                // Fallback (e.g. when moving to a different drive)
                if (!QFile::copy(fromName, toName)) {
                    Log::Error(MODULE) << "Unknown error when moving `" << fromName << "` to `" << toName << "` - copy faliure!";
                }
                if (!QFile::remove(fromName)) {
                    Log::Warn(MODULE) << "Unknown error when moving `" << fromName << "` to `" << toName << "` - old file delete faliure! (File still loaded into plugin dir tho)";
                }
            }
        } else {
            if (!QFile::copy(fromName, toName)) {
                Log::Error(MODULE) << "Unknown error when copying `" << fromName << "` to `" << toName << "`!";
            }
        }
    }
    Log::Info(MODULE) << "Successfully copied/moved " << fnames.size() << " game plugins!";
    loadGames();
    // TODO: Refresh the game view if it still exists
}

