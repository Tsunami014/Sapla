#pragma once
#include <QString>
#include <QStringList>
#include <map>
#include "schedule.hpp"
#include "template.hpp"

enum Side {
    SIDE_NAME,
    SIDE_HIDE,
    SIDE_GETFC,
    SIDE_FRONT,
    SIDE_BACK
};

QString trimNL(const QString& orig);

using ScheduleMap = std::map<QString, std::map<int, Schedule>>;

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
    QString error;

    bool setContents(const QString& newContents);
    void update();
    void updateGlobals();
    void updateCards();

    QString contents() const;
    QString title();
    QString preview();

    bool isGlobal();

    ScheduleMap getSchdMap();
    void updateSchedules();
    void removeSchedules();

    void destroy();
    void rmCards();
private:
    void update(bool isGlobal);
    void reset();
    QString orig;
    std::vector<std::unique_ptr<FlashCard>> cards;
    std::vector<QString> templates;
};

class FlashCard {
public:
    FlashCard(Note* parent, const QString& front, const QString& back, QString title, Schedule s);
    ~FlashCard();
    // Do not allow copy, but do allow move (and override it to work)
    FlashCard(const FlashCard& other) = delete;
    FlashCard& operator=(const FlashCard& other) = delete;
    FlashCard(FlashCard&& other) noexcept;
    FlashCard& operator=(FlashCard&& other) noexcept;

    bool isAlive();
    bool operator==(const FlashCard& other) const;

    virtual void update(int rating = -1);

    Schedule schd;
    Note* parent;
    QString title;
    virtual QString getSide(Side s) const;
    QString getSideHtml(Side s) const;
protected:
    QString front;
    QString back;
private:
    bool alive;

    void MoveContents(FlashCard&& other);
};

