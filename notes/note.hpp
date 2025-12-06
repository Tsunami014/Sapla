#pragma once
#include <QString>
#include <QStringList>
#include <map>
#include <chrono>

using days = std::chrono::duration<long, std::ratio<86400>>;
using weeks = std::chrono::duration<long, std::ratio<604800>>;
using months = std::chrono::duration<long, std::ratio<2629746>>;

enum Side {
    SIDE_NAME,
    SIDE_FRONT,
    SIDE_BACK
};

using Duration = std::chrono::duration<long long>;
struct _scheduleInf {
    _scheduleInf(std::vector<float> ratingScos, QString timings, Duration skipAmnt);
    std::vector<float> ratingScos; // The score increase/decrease for each rating
    int ratesLen() { return ratingScos.size(); }

    QString origTs;
    std::vector<Duration> timings;
    void setTimings(QString newtimings);
    Duration skipAmnt;
    void setSkip(QString newSkip);
};
extern _scheduleInf ScheduleInfo;
using TimePoint = std::chrono::system_clock::time_point;
struct Schedule {
    Schedule(int idx);
    Schedule(int idx, float score, long long nxtTime);
    QString toInf(QString title);

    void update(int rating = -1);
    Duration getUpdatedTime(int rating = -1);

    float score;
    TimePoint nxt;
    int idx;
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

    void updateSchedules();

    void destroy();
private:
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

    Schedule schd;
    Note* parent;
    QString title;
    QString getSide(Side s) const;
    QString getSideHtml(Side s) const;
private:
    QString front;
    QString back;
    bool alive;

    void MoveContents(FlashCard&& other);
};

