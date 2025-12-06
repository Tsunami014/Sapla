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
#include <QPushButton>
#include <QMessageBox>
#include <QSplitter>

bool BrowseScene::doubleCheck(QString prompt) {
    QMessageBox msgBox;
    msgBox.setWindowTitle("Are you sure?");
    msgBox.setText(QString("Are you sure you want to %1?").arg(prompt));
    QAbstractButton* ybtn = msgBox.addButton("Yes", QMessageBox::YesRole);
    msgBox.setDefaultButton(msgBox.addButton("No", QMessageBox::NoRole));

    msgBox.exec();
    if (msgBox.clickedButton() == ybtn) {
        QMessageBox msgBox2;
        msgBox2.setWindowTitle("Are you REALLY sure?");
        msgBox2.setText(QString("Are you REALLY sure you want to %1?").arg(prompt));
        msgBox2.setDefaultButton(msgBox2.addButton("No", QMessageBox::YesRole));
        QAbstractButton* ybtn2 = msgBox2.addButton("Yes", QMessageBox::NoRole);
        msgBox2.exec();
        return msgBox2.clickedButton() == ybtn2;
    }
    return false;
}

void BrowseScene::prevIdxTyp::reset() {
    idx = 0; max = 0;
}

BrowseScene::BrowseScene()
    : BaseScene(), m("New note"),
    clearIt("Clear notes", Menues->FileMenu),
    resetIt("Reset notes to default", Menues->FileMenu) {
        helpStr = &BROWSE_HELP;
        MG->changeBG("dirt");
        MG->removeGame();

        tree = getNoteTree(this);
        QWidget::connect(tree, &QTreeWidget::itemSelectionChanged, this, &BrowseScene::selectionChange);

        te = new MarkdownEdit(this);
        te->setDisabled(true);
        QObject::connect(te, &MarkdownEdit::textChanged, this, &BrowseScene::typed);

        bar = new Topbar(this);
        connect(bar, &Topbar::onBtnPush, this, [=](const QString& apply){
            te->insertMarkdown(apply, "$CUR$");
        });

        prevIdx = {0, 0};
        prevIdxLabl = new QLabel(this);
        prevIdxLabl->setFont(getFont(1.5));
        auto* prevBtn = new SvgBtn(":/assets/btn2.svg");
        prevBtn->setText("←");
        connect(prevBtn, &SvgBtn::clicked, this, [=](){
            if (prevIdx.idx > 1) {
                prevIdx.idx--;
                side = SIDE_FRONT;
                updatePrev();
            }
        });
        auto* nxtBtn = new SvgBtn(":/assets/btn2.svg");
        nxtBtn->setText("→");
        connect(nxtBtn, &SvgBtn::clicked, this, [=](){
            if (prevIdx.idx < prevIdx.max) {
                prevIdx.idx++;
                side = SIDE_FRONT;
                updatePrev();
            }
        });

        preview = new MarkdownEdit(this);
        preview->setButton(true);
        connect(preview, &MarkdownEdit::clicked, this, [=](){
            if (side == SIDE_FRONT) side = SIDE_BACK;
            else if (side == SIDE_BACK) side = SIDE_FRONT;
            updatePrev();
        });
        updatePrev();

        auto* hlay = new QHBoxLayout();
        hlay->addStretch();
        hlay->addWidget(prevIdxLabl);
        hlay->addStretch();
        hlay->addWidget(prevBtn);
        hlay->addWidget(nxtBtn);

        auto* vLay = new QVBoxLayout();
        vLay->addWidget(bar);
        vLay->addWidget(te, 2);
        vLay->addLayout(hlay);
        vLay->addWidget(preview, 1);
        auto* vLayW = new QWidget(this);
        vLayW->setLayout(vLay);

        auto* mSplit = new QSplitter(Qt::Horizontal, this);
        mSplit->addWidget(tree);
        mSplit->addWidget(vLayW);
        mSplit->setHandleWidth(6);
        mSplit->setStretchFactor(0, 1);
        mSplit->setStretchFactor(1, 1);

        QTimer::singleShot(0, this, [=](){
            int w = this->width() / 2;
            if (w > 0) mSplit->setSizes(QList<int>{w, w});
            bar->resize();
        });

        auto* root = new QHBoxLayout(this);
        root->addWidget(mSplit);

        connect(&m, &QAction::triggered, this, &BrowseScene::newNote);
        connect(&clearIt, &QAction::triggered, this, [this](){
            if (doubleCheck("delete all your notes")) {
                for (auto* n : notesL) delete n;
                notesL.clear();
                tree->clear();
                selectionChange();
            }
        });
        connect(&resetIt, &QAction::triggered, this, [this](){
            if (doubleCheck("delete all your notes and replace them with the defaults")) {
                loadDefaultNotes();
                resetNoteTree(tree);
                selectionChange();
            }
        });
    }
Note* BrowseScene::getNote(QTreeWidgetItem* item) {
    return static_cast<TreeData*>(item->data(0, Qt::UserRole).value<void*>())->note;
}
Note* BrowseScene::getSelNote() {
    return getNote(tree->selectedItems().first());
}

void BrowseScene::updatePrev() {
    if (tree->selectedItems().size() == 0) {
        prevIdxLabl->setText("Select a note");
        preview->setMarkdown("");
        return;
    }
    Note* n = getSelNote();
    if (n->error != "") {
        prevIdxLabl->setText("<span style='color:red;'>Encountered errors!</span>");
        preview->setMarkdown(n->error);
        side = SIDE_FRONT;
        return;
    }
    if (prevIdx.idx == 0) {
        prevIdxLabl->setText("No cards to preview!");
        preview->setMarkdown("");
        return;
    }
    prevIdxLabl->setText(QString("Preview card %1/%2").arg(prevIdx.idx).arg(prevIdx.max));
    preview->setMarkdown(n->getFlashCard(prevIdx.idx-1)->getSide(side));
}
void BrowseScene::typed() {
    QList<QTreeWidgetItem*> selected = tree->selectedItems();
    if (selected.size() != 1) {
        te->setText("");
        te->setDisabled(true);
        prevIdx.reset();
        updatePrev();
        return;
    }
    Note* n = getSelNote();
    n->setContents(te->getMarkdown());
    n->updateCards();

    int nOO = n->getNumCards();
    if (prevIdx.max != nOO) {
        int idx = prevIdx.idx;
        if (idx > nOO) {
            idx = nOO;
            side = SIDE_FRONT;
        }
        prevIdx.idx = idx;
        prevIdx.max = nOO;
    }
    if (nOO == 0) { prevIdx.idx = 0; }
    else {
        if (prevIdx.idx == 0)
            prevIdx.idx = 1;
    }
    updatePrev();

    updateItem(selected.first(), n);
    writeNotes();
}
void BrowseScene::selectionChange() {
    QList<QTreeWidgetItem*> selected = tree->selectedItems();
    if (selected.isEmpty()) {
        te->setMarkdown("");
        te->setDisabled(true);
        prevIdx.reset();
        updatePrev();
        return;
    }

    Note* n = getSelNote();

    te->setMarkdown(n->contents());
    te->setDisabled(false);

    int outOf = n->getNumCards();
    side = SIDE_FRONT;
    if (outOf > 0) {
        prevIdx.idx = 1;
        prevIdx.max = outOf;
    } else {
        prevIdx.reset();
    }
    updatePrev();
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

        Note* data = getNote(item);
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
        } else {
            tree->clearSelection();
        }
        selectionChange();
        delete item;
        return true;
    }
    return false;
}

