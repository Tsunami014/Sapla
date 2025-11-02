#include "browseScn.hpp"
#include "../core.hpp"
#include "../cards/cardTree.hpp"
#include "../cards/getCards.hpp"
#include "../cards/cardList.hpp"
#include "../base/font.hpp"
#include <QColor>
#include <QTimer>
#include <QHeaderView>
#include <QScrollArea>
#include <QKeyEvent>

const QString HELP_TXT = "&lt;Ctrl+Delete&gt; to delete currently selected item, &lt;Esc&gt; to go back";

class FormWidget : public QWidget {
public:
    FormWidget(QWidget *parent = nullptr) : QWidget(parent) {
        setAttribute(Qt::WA_TranslucentBackground);
        setFocusPolicy(Qt::ClickFocus);
    }

    void paintEvent(QPaintEvent *ev) override {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);

        QPainterPath pth;
        pth.setFillRule(Qt::WindingFill);
        QRectF r = rect().adjusted(4.0, 4.0, -4.0, -4.0);
        pth.addRoundedRect(r, 10.0, 10.0);
        pth.addRect(r.adjusted(0, 10, 0, 0));
        p.fillPath(pth, QColor(140, 70, 45, 130));
    }
};

void deleteLayout(QLayout* lay) {
    QLayoutItem* it;
    while ((it = lay->takeAt(0)) != nullptr) {
        if (QWidget* wid = it->widget()) {
            wid->setParent(nullptr);  // detach from layout
            wid->deleteLater();
        } else if (QLayout* lay2 = it->layout()) {
            lay2->setParent(nullptr);
            deleteLayout(lay2);
            lay2->deleteLater();
        }
        delete it;  // Stop reason: signal SIGSEGV: address not mapped to object (fault address=0x0)
    }
}

BrowseScene::BrowseScene()
    : BaseScene(), newCmenu("New Card") {
        helpStr = &HELP_TXT;
        MG->changeBG("dirt");

        tree = getCardTree(this);
        for (const auto& typ : CardRegistry::registry) {
            QAction* act = newCmenu.addAction(typ.name);
            QObject::connect(act, &QAction::triggered, [this, typ]() {
                addCard(typ.newBlank());
            });
        }

        FormWidget* formWid = new FormWidget(this);
        auto* scroll = new QScrollArea();
        scroll->setAttribute(Qt::WA_TranslucentBackground);
        scroll->setWidgetResizable(true);
        scroll->setWidget(formWid);
        scroll->setFrameShape(QFrame::NoFrame);
        scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        scroll->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        form = new QVBoxLayout(formWid);
        form->setSpacing(4);
        form->setContentsMargins(4, 4, 4, 4);

        QWidget::connect(tree, &QTreeWidget::itemSelectionChanged, [&](){
            deleteLayout(form);

            QList<QTreeWidgetItem*> selected = tree->selectedItems();
            if (selected.isEmpty())
                return;

            QTreeWidgetItem* item = selected.first();
            BaseCardTyp* data = static_cast<TreeData*>(item->data(0, Qt::UserRole).value<void*>())->card;

            data->createForm(form, item);
            form->addStretch();
        });

        auto* mLay = new QHBoxLayout(this);
        mLay->addWidget(tree);
        mLay->addWidget(scroll);
    }

void BrowseScene::addCard(BaseCardTyp* card) {
    CLaddCard(card);
    writeCards();

    auto* it = addToTree(tree, card);
    tree->sortItems(
        tree->header()->sortIndicatorSection(),
        tree->header()->sortIndicatorOrder()
    );
    tree->setCurrentItem(it);
}

void BrowseScene::keyPressEvent(QKeyEvent* event) {
    if (MG->handleEv(event)) return;
    int key = event->key();
    if ((key == Qt::Key_Delete || key == Qt::Key_Backspace) && 
               event->modifiers() == (Qt::ControlModifier)) {
        QList<QTreeWidgetItem*> selected = tree->selectedItems();
        if (selected.isEmpty())
            return;

        QTreeWidgetItem* item = selected.first();
        BaseCardTyp* data = static_cast<TreeData*>(item->data(0, Qt::UserRole).value<void*>())->card;
        CLremoveCard(data);
        writeCards();
        if (QTreeWidgetItem* parent = item->parent()) {
            parent->removeChild(item);
        } else if (QTreeWidget *tree = item->treeWidget()) {
            tree->takeTopLevelItem(tree->indexOfTopLevelItem(item));
        }
        delete item;
    }
}

