#include "gameView.hpp"
#include "../core.hpp"
#include "../games/getGames.hpp"
#include "../wids/listWid.hpp"
#include <QTextEdit>

struct ListData {
    QString text;
    bool working;
};
Q_DECLARE_METATYPE(ListData)

GameViewScene::GameViewScene()
    : BaseScene() {
        qRegisterMetaType<ListData>("GVSData");
        MG->changeBG("dirt");

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

