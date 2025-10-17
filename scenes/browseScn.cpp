#include "browseScn.hpp"
#include "homeScn.hpp"
#include "../main.hpp"
#include "../cards/cardTree.hpp"
#include "../cards/getCards.hpp"
#include "../items/base/font.hpp"
#include <QColor>
#include <QTimer>
#include <QHeaderView>

#ifdef _WIN32
// Windows being JUST THAT BAD
class FixedMenuBar : public QMenuBar {
public:
    using QMenuBar::QMenuBar;

protected:
    void mousePressEvent(QMouseEvent* ev) override {
        if (QAction* act = actionAt(ev->pos())) {
            if (QMenu* menu = act->menu()) {
                // We have to make a new menu with the same items otherwise Windows will just not show it
                QMenu dyn;
                dyn.addActions(menu->actions());
                dyn.exec(QCursor::pos());
                ev->accept();
                return;
            }
        }
        QMenuBar::mousePressEvent(ev);
    }
};
#else
using FixedMenuBar = QMenuBar;
#endif

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
            "QMenuBar {"
                "margin: 4px;"
                "border-top-left-radius: 10px;"
                "border-top-right-radius: 10px;"
                "border: 1px solid #666;"
                "background: rgb(90, 40, 10);"
            "}"
            "QMenuBar::item {"
                "padding: 4px;"
                "background: rgb(70, 35, 10);"
                "border-radius: 6px;"
            "}"
            "QMenuBar::item:selected {"
                "background: rgb(110, 60, 30);"
            "}"
            "QMenu {"
                "background: rgba(90,40,10,220);"
                "border-radius: 6px;"
            "}"
            "QMenu::item {"
                "border-radius: 4px;"
                "padding: 4px;"
            "}"
            "QMenu::item:selected {"
                "background: rgba(255,255,255,30);"
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
    QTimer::singleShot(0, [](){ MG->changeScene(new HomeScene()); });
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

BrowseScene::BrowseScene() : BaseScene(), TreeProxy(this), FormProxy(this), backBtn(":/assets/backBtn.svg", this) {
    MG->setBottomTxt("<Ctrl+Delete> to delete currently selected item, <Esc> to go back");
    MG->changeBG("dirt");

    tree = getCardTree();

    FormWidget* formWid = new FormWidget();
    fmenu = new FixedMenuBar(formWid);

    QMenu* newCmenu = fmenu->addMenu("New card");
    for (const auto& typ : CardRegistry::registry) {
        QAction* act = newCmenu->addAction(typ.name);
        QObject::connect(act, &QAction::triggered, [this, typ]() {
            addCard(typ.newBlank());
        });
    }

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
    cards.push_back(card);
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
            auto it = std::find(cards.begin(), cards.end(), data);
            if (it != cards.end()) {
                delete *it;
                cards.erase(it);
                writeCards();
            }
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
    TreeProxy.setPos(0, rect.height()*0.05);
    TreeProxy.resize(rect.width()/2, rect.height()*0.95);
    FormProxy.setPos(rect.width()/2, 0);
    qreal formWid = rect.width()/2;
    FormProxy.resize(formWid, rect.height());
    fmenu->setFixedWidth(formWid);
    form->setContentsMargins(10, 10+fmenu->height(), 10, 10);
    backBtn.setRect({ 0, 0, rect.height()*0.05, rect.height()*0.05 });
}
