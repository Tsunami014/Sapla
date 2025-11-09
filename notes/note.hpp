#pragma once
#include <QString>

class FlashCard; // Forward reference
class Note {
public:
    explicit Note(QString contents);
    ~Note();
    bool operator==(const Note& other) const;
    // Copy & move stuff
    Note(const Note& other);
    Note& operator=(const Note& other);
    Note(Note&& other) noexcept;
    Note& operator=(Note&& other) noexcept;

    void setContents(QString newContents);
    QString contents() const;
    QString title();
    void destroy();
private:
    void ridCards();
    QString orig;
    std::vector<FlashCard> cards;
};

class FlashCard {
public:
    FlashCard(Note* parent, const QString& front, const QString& back);

    bool operator==(const FlashCard& other) const;

    Note* parent;
    QString front;
    QString back;
};

