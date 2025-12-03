#pragma once
#include <QString>
#include <QStringList>
#include <map>
#include <chrono>

enum Side {
    SIDE_NAME,
    SIDE_FRONT,
    SIDE_BACK
};

struct Schedule {
    void update(int rating);
    static Schedule blank();

    int score;
    std::chrono::system_clock::time_point nxt;
};

extern std::map<QString, QString> globalTemplates;

class FlashCard; // Forward reference
class Note {
public:
    explicit Note(QString contents);
    ~Note();
    bool operator==(const Note& other) const;
    // Do not allow copy, but do allow move (and override it to work)
    Note(const Note& other) = delete;
    Note& operator=(const Note& other) = delete;
    Note(Note&& other) noexcept;
    Note& operator=(Note&& other) noexcept;

    FlashCard* getFlashCard(int idx);
    int getNumCards();
    int getNumTemplates();

    QStringList tags;
    int prio;

    QString error;

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
    FlashCard(Note* parent, const QString& front, const QString& back, Schedule s);
    ~FlashCard();
    bool isAlive();
    bool operator==(const FlashCard& other) const;

    Schedule schd;
    Note* parent;
    QString getSide(Side s) const;
    QString getSideHtml(Side s) const;
private:
    QString front;
    QString back;
    bool alive;
};

