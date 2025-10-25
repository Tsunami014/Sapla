#include "browseScn.hpp"
#include "homeScn.hpp"
#include "../core.hpp"
#include "../cards/cardTree.hpp"
#include "../cards/getCards.hpp"
#include "../cards/cardList.hpp"
#include "../items/base/font.hpp"
#include <QColor>
#include <QTimer>
#include <QHeaderView>

const QString HELP_TXT = "&lt;Ctrl+Delete&gt; to delete currently selected item, &lt;Esc&gt; to go back";

class FormWidget : public QWidget {
public:
    FormWidget(QWidget *parent = nullptr) : QWidget(parent) {
        setAttribute(Qt::WA_TranslucentBackground);
        setFocusPolicy(Qt::ClickFocus);
        setStyleSheet(
            "QTextEdit {"
                "background: rgba(110, 60, 30, 220);"
                "border-radius: 6px;"
                "border: 1px solid #666;"
            "}"
            "QTextEdit:focus {"
                "border: 2px solid #333;"
            "}"
        );
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

void goBack() {
    MG->changeScene(new HomeScene());
}

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
    : BaseScene(), TreeProxy(this), FormProxy(this), backBtn(":/assets/backBtn.svg", this), newCmenu("New Card") {
        helpStr = &HELP_TXT;
        MG->changeBG("dirt");

        tree = getCardTree();

        for (const auto& typ : CardRegistry::registry) {
            QAction* act = newCmenu.addAction(typ.name);
            QObject::connect(act, &QAction::triggered, [this, typ]() {
                addCard(typ.newBlank());
            });
        }

        FormWidget* formWid = new FormWidget();
        formWid->setFont(getFont());

        auto* scroll = new QScrollArea();
        scroll->setAttribute(Qt::WA_TranslucentBackground);
        scroll->setWidgetResizable(true);
        scroll->setWidget(formWid);
        scroll->setFrameShape(QFrame::NoFrame);
        scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        scroll->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        FormProxy.setWidget(scroll);
        FormProxy.setAutoFillBackground(false);
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
        tree->setFont(getFont());
        TreeProxy.setWidget(tree);

        backBtn.onClick = [](){ goBack(); };
    }

void BrowseScene::addCard(BaseCardTyp* card) {
    addCard(card);
    writeCards();

    auto* it = addToTree(tree, card);
    tree->sortItems(
        tree->header()->sortIndicatorSection(),
        tree->header()->sortIndicatorOrder()
    );
    tree->setCurrentItem(it);
}

void BrowseScene::onEvent(QEvent* event) {
    if (event->type() == QEvent::KeyPress) {
        auto* keyEvent = (QKeyEvent*)event;
        int key = keyEvent->key();

        if (key == Qt::Key_Escape) {
            goBack();
        } else if ((key == Qt::Key_Delete || key == Qt::Key_Backspace) && 
                   keyEvent->modifiers() == (Qt::ControlModifier)) {
            QList<QTreeWidgetItem*> selected = tree->selectedItems();
            if (selected.isEmpty())
                return;

            QTreeWidgetItem* item = selected.first();
            BaseCardTyp* data = static_cast<TreeData*>(item->data(0, Qt::UserRole).value<void*>())->card;
            removeCard(data);
            writeCards();
            if (QTreeWidgetItem* parent = item->parent()) {
                parent->removeChild(item);
            } else if (QTreeWidget *tree = item->treeWidget()) {
                tree->takeTopLevelItem(tree->indexOfTopLevelItem(item));
            }
            delete item;
        }
    }
}

void BrowseScene::resize() {
    qreal hWid = rect.width()/2;
    TreeProxy.setPos(0, rect.height()*0.05);
    TreeProxy.resize(hWid, rect.height()*0.95);
    FormProxy.setPos(hWid, 0);
    FormProxy.resize(hWid, rect.height());
    backBtn.setRect({ 0, 0, rect.height()*0.05, rect.height()*0.05 });
}
