#pragma once
#include <QString>
#include <map>

enum Side {
    SIDE_FRONT,
    SIDE_BACK
};

extern std::map<QString, QString> globalTemplates;

class FlashCard; // Forward reference
class Note {
public:
    explicit Note(QString contents);
    ~Note();
    bool operator==(const Note& other) const;
    // No not allow copy, but do allow move (and override it to work)
    Note(const Note& other) = delete;
    Note& operator=(const Note& other) = delete;
    Note(Note&& other) noexcept;
    Note& operator=(Note&& other) noexcept;

    FlashCard* getFlashCard(int idx);
    int getNumCards();
    int getNumTemplates();

    void setContents(const QString& newContents);
    void updateCards();
    QString contents() const;
    QString title();

    void destroy();
private:
    void reset();
    QString orig;
    std::vector<FlashCard> cards;
    std::vector<QString> templates;
};

class FlashCard {
public:
    FlashCard(Note* parent, const QString& front, const QString& back);
    bool operator==(const FlashCard& other) const;

    Note* parent;
    QString getSide(Side s) const;
    QString getSideHtml(Side s) const;
private:
    QString front;
    QString back;
};

