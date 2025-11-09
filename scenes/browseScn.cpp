#include "browseScn.hpp"
#include "../core.hpp"
#include "../notes/noteTree.hpp"
#include "../notes/getNotes.hpp"
#include "../notes/cardList.hpp"
#include "../base/font.hpp"
#include <QColor>
#include <QTimer>
#include <QHeaderView>
#include <QScrollArea>
#include <QKeyEvent>

const QString HELP_TXT = "&lt;Ctrl+Delete&gt; to delete currently selected item, &lt;Esc&gt; to go back";

BrowseScene::BrowseScene()
    : BaseScene(), m("New card") {
        helpStr = &HELP_TXT;
        MG->changeBG("dirt");

        tree = getNoteTree(this);

        te = new MarkdownEdit(this);
        te->setDisabled(true);
        QObject::connect(te, &MarkdownEdit::textChanged, this, &BrowseScene::typed);
        auto* scroll = new QScrollArea();
        scroll->setAttribute(Qt::WA_TranslucentBackground);
        scroll->setWidgetResizable(true);
        scroll->setWidget(te);
        scroll->setFrameShape(QFrame::NoFrame);
        scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        scroll->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

        QWidget::connect(tree, &QTreeWidget::itemSelectionChanged, this, &BrowseScene::selectionChange);

        auto* mLay = new QHBoxLayout(this);
        mLay->addWidget(tree);
        mLay->addWidget(scroll);

        connect(&m, &QAction::triggered, this, &BrowseScene::newNote);
    }

void BrowseScene::typed() {
    QList<QTreeWidgetItem*> selected = tree->selectedItems();
    if (selected.size() != 1) {
        te->setText("");
        te->setDisabled(true);
        return;
    }
    QTreeWidgetItem* it = selected.first();
    Note* n = static_cast<TreeData*>(it->data(0, Qt::UserRole).value<void*>())->note;
    n->setContents(te->getMarkdown());
    it->setText(0, n->title());
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
        Note* data = static_cast<TreeData*>(item->data(0, Qt::UserRole).value<void*>())->note;
        notesL.erase(std::remove(notesL.begin(), notesL.end(), data), notesL.end());
        delete data;
        writeNotes();
        if (QTreeWidgetItem* parent = item->parent()) {
            parent->removeChild(item);
        } else if (QTreeWidget *tree = item->treeWidget()) {
            tree->takeTopLevelItem(tree->indexOfTopLevelItem(item));
        }
        tree->clearSelection();
        te->setMarkdown("");
        te->setDisabled(true);
        delete item;
        return true;
    }
    return false;
}

