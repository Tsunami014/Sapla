#pragma once
#include <QString>
#include <chrono>
#include <vector>

using days = std::chrono::duration<long, std::ratio<86400>>;
using weeks = std::chrono::duration<long, std::ratio<604800>>;
using months = std::chrono::duration<long, std::ratio<2629746>>;
using years = std::chrono::duration<long, std::ratio<31556952>>;

using Duration = std::chrono::duration<long long>;
struct _scheduleInf {
    _scheduleInf(
        std::vector<float> ratingScos,
        QString timings,
        Duration skipAmnt,
        Duration redoAmnt,

        Duration notnewAmnt,
        Duration learntAmnt,

        Duration leaveAmnt
    );

    /// The score increase/decrease for each rating
    std::vector<float> ratingScos;
    int ratesLen() { return ratingScos.size(); }

    QString origTs;
    /// The time of next card for each score
    std::vector<Duration> timings;

    /// The minimum score that means you 'learnt' the card (when the time for next card is at least 3 days)
    float learntSco;
    /// The minimum score that means the card is not new anymore (from new to learning)
    float notnewSco;
    /// The minimum score that means if you pass this then the card should leave the current pile
    float leaveSco;

    void setTimings(QString newtimings);
    Duration skipAmnt;
    Duration redoAmnt;
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
    float percentage() const;
    TimePoint trueNxt(TimePoint now) const;
    bool dueNow() const;
    bool isNew() const;
};
