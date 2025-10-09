#include "browseScn.hpp"
#include "../cards/cardTree.hpp"
#include "../cards/cardTyps.hpp"

class FormWidget : public QWidget {
public:
    FormWidget(QWidget *parent = nullptr) : QWidget(parent) {
        setAttribute(Qt::WA_TranslucentBackground, true);
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

protected:
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

BrowseScene::BrowseScene() : BaseScene(), TreeProxy(this), FormProxy(this) {
    tree = new CardTree();
    getCardTree(tree);

    FormWidget* formWid = new FormWidget();  // Set and forget
    FormProxy.setWidget(formWid);
    FormProxy.setAutoFillBackground(false);
    form = new QVBoxLayout(formWid);
    form->setContentsMargins(10, 10, 10, 10);
    form->setSpacing(4);

    QWidget::connect(tree, &QTreeWidget::itemSelectionChanged, [&](){
        QLayoutItem* it;
        while ((it = form->takeAt(0)) != nullptr) {
            if (QWidget* wid = it->widget()) {
                wid->setParent(nullptr);  // detach from layout
                delete wid;
            }
            delete it;
        }

        QList<QTreeWidgetItem*> selected = tree->selectedItems();
        if (selected.isEmpty())
            return;

        QTreeWidgetItem* item = selected.first();
        BaseCardTyp* data = static_cast<BaseCardTyp*>(item->data(0, Qt::UserRole).value<void*>());

        data->createForm(form, item);
        form->addStretch();
    });
    TreeProxy.setWidget(tree);
}

void BrowseScene::resize() {
    TreeProxy.setPos(0, rect.height()*0.05);
    TreeProxy.resize(rect.width()/2, rect.height()*0.95);
    FormProxy.setPos(rect.width()/2, rect.height()*0.05);
    FormProxy.resize(rect.width()/2, rect.height()*0.95);
}
