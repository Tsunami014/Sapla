#include "browseScn.hpp"
#include "../core.hpp"
#include "../help.hpp"
#include "../notes/noteTree.hpp"
#include "../notes/getNotes.hpp"
#include "../notes/cardList.hpp"
#include "../base/font.hpp"
#include <QTimer>
#include <QHeaderView>
#include <QKeyEvent>
#include <QTextBlock>

BrowseScene::BrowseScene()
    : BaseScene(), m("New card") {
        helpStr = &BROWSE_HELP;
        MG->changeBG("dirt");

        tree = getNoteTree(this);
        QWidget::connect(tree, &QTreeWidget::itemSelectionChanged, this, &BrowseScene::selectionChange);

        te = new MarkdownEdit(this);
        te->setDisabled(true);
        QObject::connect(te, &MarkdownEdit::textChanged, this, &BrowseScene::typed);

        bar = new Topbar(this);
        connect(bar, &Topbar::onBtnPush, this, [=](const QString& apply){
            te->insertMarkdown(apply, "$CUR$");
        });

        auto* vLay = new QVBoxLayout();
        vLay->addWidget(bar);
        vLay->addWidget(te);

        auto* mLay = new QHBoxLayout(this);
        mLay->addWidget(tree);
        mLay->addLayout(vLay);

        connect(&m, &QAction::triggered, this, &BrowseScene::newNote);
    }

void BrowseScene::typed() {
    QList<QTreeWidgetItem*> selected = tree->selectedItems();
    if (selected.size() != 1) {
        te->setText("");
        te->setDisabled(true);
        return;
    }
    updateNoteOnTree(selected.first(), te->getMarkdown());
    writeNotes();
}
void BrowseScene::selectionChange() {
    QList<QTreeWidgetItem*> selected = tree->selectedItems();
    if (selected.isEmpty()) {
        te->setMarkdown("");
        te->setDisabled(true);
        return;
    }

    QTreeWidgetItem* item = selected.first();
    Note* n = static_cast<TreeData*>(item->data(0, Qt::UserRole).value<void*>())->note;

    te->setMarkdown(n->contents());
    te->setDisabled(false);
}
void BrowseScene::newNote() {
    auto* n = new Note("");
    n->updateCards();
    notesL.push_back(n);
    auto* it = addToTree(tree, n);
    tree->setCurrentItem(it);
    writeNotes();

    tree->sortItems(
        tree->header()->sortIndicatorSection(),
        tree->header()->sortIndicatorOrder()
    );
}

bool BrowseScene::keyEv(QKeyEvent* event) {
    if (MG->handleEv(event)) return true;
    int key = event->key();
    if ((key == Qt::Key_Delete || key == Qt::Key_Backspace) && 
               event->modifiers() == (Qt::ControlModifier)) {
        QList<QTreeWidgetItem*> selected = tree->selectedItems();
        if (selected.isEmpty())
            return true;

        QTreeWidgetItem* item = selected.first();
        QSignalBlocker block(tree);

        QTreeWidgetItem* parent = item->parent();
        int idx; int maxlen;
        if (parent) {
            idx = parent->indexOfChild(item);
            maxlen = parent->childCount();
        } else {
            idx = tree->indexOfTopLevelItem(item);
            maxlen = tree->topLevelItemCount();
        }
        bool nxt = maxlen > 1;
        if (nxt && idx == maxlen-1) idx--;

        Note* data = static_cast<TreeData*>(item->data(0, Qt::UserRole).value<void*>())->note;
        notesL.erase(std::remove(notesL.begin(), notesL.end(), data), notesL.end());
        delete data;
        writeNotes();
        QTreeWidgetItem* nxtSel;
        if (parent) {
            parent->removeChild(item);
            if (nxt) nxtSel = parent->child(idx);
        } else if (QTreeWidget *tree = item->treeWidget()) {
            tree->takeTopLevelItem(tree->indexOfTopLevelItem(item));
            if (nxt) nxtSel = tree->topLevelItem(idx);
        }

        if (nxt) {
            tree->setCurrentItem(nxtSel);
            selectionChange();
        } else {
            tree->clearSelection();
            te->setMarkdown("");
            te->setDisabled(true);
        }
        delete item;
        return true;
    }
    return false;
}

