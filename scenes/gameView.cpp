#include "gameView.hpp"
#include "../core.hpp"
#include "../games/getGames.hpp"
#include "../items/base/font.hpp"

struct ListData {
    QString text;
    bool working;
};
Q_DECLARE_METATYPE(ListData)

GameViewScene::GameViewScene()
    : BaseScene(), li(), prox(this) {
        qRegisterMetaType<ListData>("GVSData");
        MG->changeBG("dirt");
        QWidget* base = new QWidget();
        base->setFont(getFont());

        auto* mainLay = new QHBoxLayout(base);
        mainLay->addWidget(&li, 5);
        li.setHeaderHidden(true);
        li.setRootIsDecorated(false);
        li.setIndentation(0);
        li.setItemsExpandable(false);

        for (auto& fg : failedGames) {
            auto* it = new QTreeWidgetItem(QStringList("💔 "+fg.first));
            it->setData(0, Qt::UserRole, QVariant::fromValue(ListData{"Game loaded with error:\n"+fg.second, false}));
            li.addTopLevelItem(it);
        }
        for (auto* g : games) {
            auto* it = new QTreeWidgetItem(QStringList("💖 "+g->name));
            it->setData(0, Qt::UserRole, QVariant::fromValue(ListData{"Game loaded successfully!", true}));
            li.addTopLevelItem(it);
        }

        QWidget::connect(&li, &QTreeWidget::itemSelectionChanged, [&](){
            QTreeWidgetItem* sel = li.currentItem();
            if (sel) {
                ListData dat = sel->data(0, Qt::UserRole).value<ListData>();
            }
        });

        auto* sideLay = new QVBoxLayout(base);

        auto* txt = new QTextEdit(base);
        txt->setDisabled(true);
        sideLay->addWidget(txt);

        mainLay->addLayout(sideLay, 1);
        prox.setWidget(base);
        prox.setPos(0, 0);
    }

void GameViewScene::onEvent(QEvent* event) {
    MG->handleEv(event);
}
void GameViewScene::resize() {
    prox.resize(rect.size());
}

