#include "schedule.hpp"
#include "../log.hpp"

const QString MODULE = "Schedule";

using DurationFn = std::function<Duration(long long)>;
#define makeTimeFn(time) [](long long n){ return time(n); }
static const std::unordered_map<QString, DurationFn> & getUnitMap() {
    static const std::unordered_map<QString, DurationFn> unitMap = []{
        std::unordered_map<QString, DurationFn> m;
        auto add = [&](const QStringList &keys, DurationFn fn) {
            for (const auto &k : keys) m.emplace(k, fn);
        };
        add({ "y", "yr", "ys", "yrs", "year", "years" }, makeTimeFn(years));
        add({ "mo", "mos", "mth", "mths", "month", "months" }, makeTimeFn(months));
        add({ "w", "wk", "wks", "week", "weeks" }, makeTimeFn(weeks));
        add({ "d", "dy", "dys", "day", "days" }, makeTimeFn(days));
        add({ "h", "hr", "hrs", "hour", "hours" }, makeTimeFn(std::chrono::hours));
        add({ "m", "min", "mins", "minute", "minutes" }, makeTimeFn(std::chrono::minutes));
        add({ "s", "sec", "secs", "second", "seconds" }, makeTimeFn(std::chrono::seconds));
        return m;
    }();
    return unitMap;
}


Duration parseDuration(QString amount, QString unit, QString& problems) {
    bool ok;
    long amnt = amount.toLong(&ok);
    if (!ok) {
        problems += "Could not parse number: " + amount + "!\n";
        return {};
    }
    Duration time;

    const auto &map = getUnitMap();
    auto it = map.find(unit);
    if (it != map.end()) {
        return it->second(amnt);
    }
    problems += "Unit not found: " + unit + "\n";
    return {};
}
Duration parseDuration(QString amount, QString unit) {
    QString ps;
    return parseDuration(amount, unit, ps);
}
enum Parts { NUM, TXT, UNKNOWN, END };
Duration parseWholeDuration(QString inp, QString& problems) {
    QString part;
    Duration dur = {};
    QString lastnum;
    Parts typ = NUM;
    for (QChar c : (inp+'\n')) {
        Parts thistyp;
        if ((c >= '0' && c <= '9')) {
            thistyp = NUM;
        } else if (c >= 'A' && c <= 'Z') {
            c = c.toLower();
            thistyp = TXT;
        } else if (c >= 'a' && c <= 'z') {
            thistyp = TXT;
        } else if (c == ' ') {
            continue;
        } else if (c == '\n') {
            thistyp = END;
        } else {
            thistyp = UNKNOWN;
        }
        if (thistyp == typ) {
            part += c;
        } else {
            if (typ == NUM) {
                if (!lastnum.isEmpty()) {
                    problems += "Two numbers found in a row: " + lastnum + " and " + part + "!\n";
                }
                lastnum = part;
            } else if (typ == TXT) {
                if (lastnum.isEmpty()) {
                    problems += "Text found without a number prefix: " + part + "\n";
                } else {
                    dur += parseDuration(lastnum, part, problems);
                    lastnum.clear();
                }
            } else {
                problems += "Found an unknown section: " + part + "\n";
            }
            typ = thistyp;
            part = c;
        }
    }
    return dur;
}
Duration parseWholeDuration(QString inp) {
    QString ps;
    return parseWholeDuration(inp, ps);
}



_scheduleInf::_scheduleInf(
    std::vector<float> rScos,
    QString ts,
    Duration skpAmnt,
    Duration rdoAmnt,
    Duration flAmnt,

    Duration notnewAmnt,
    Duration learntAmnt,
    Duration leaveAmnt)
    :ratingScos(rScos), skipAmnt(skpAmnt), redoAmnt(rdoAmnt), failAmnt(flAmnt) {
        setTimings(ts);
        const size_t maxSze = timings.size()-1;

        uint8_t untilbreak = 3;
        notnewSco = maxSze;
        learntSco = maxSze;
        leaveSco = maxSze;
        for (int i = 0; i <= maxSze; i++) {
            auto t = timings[i];
            if (leaveSco == maxSze && t >= leaveAmnt) {
                leaveSco = i;
                untilbreak--;
            }
            if (notnewSco == maxSze && t >= notnewAmnt) {
                notnewSco = i;
                untilbreak--;
            }
            if (learntSco == maxSze && t >= learntAmnt) {
                learntSco = i;
                untilbreak--;
            }
            if (untilbreak == 0) break;
        }
    }

void _scheduleInf::setTimings(QString newtimings) {
    auto ntimes = timings;
    QString problems = "";
    for (QString part : newtimings.split('\n')) {
        auto dur = parseWholeDuration(part, problems);
        if (problems.isEmpty()) ntimes.push_back(dur);
    }
    if (problems.isEmpty()) {
        timings.clear();
        timings = ntimes;
        origTs = newtimings;
    } else {
        Log::Error(MODULE) << "Problems with new time parsing config:\n" << problems;
    }
}
void _scheduleInf::setSkip(QString newSkip) {
    QString problems;
    auto nskip = parseWholeDuration(newSkip, problems);
    if (problems.isEmpty()) {
        skipAmnt = nskip;
    } else {
        Log::Error(MODULE) << "Problems with new skip time parsing:\n" << problems;
    }
}

_scheduleInf ScheduleInfo(
    {-2, -1, 0.5, 1.5, 3},
    "30sec\n"
    "2min\n"
    "5mins\n"
    "10mins\n"
    "30mins\n"
    "1hr\n"
    "1hr30min\n"
    "3hrs\n"
    "5hrs\n"
    "10hrs\n"
    "16hrs\n"
    "1day\n"
    "2days\n"
    "4days\n"
    "1wk\n"
    "1wk4days\n"
    "2wks3days\n"
    "4wks\n"
    "2mos\n"
    "4mos\n"
    "6mos\n"
    "10mos\n"
    "1yr\n"
    , parseDuration("30", "mins")  // Skip amount
    , parseDuration("1", "min")  // Redo amount
    , parseDuration("1", "min")  // Fail amount

    , parseDuration("2", "hours")  // NotNew amount
    , parseDuration("4", "days")  // Learnt amount

    , parseDuration("6", "mins")  // Leave amount
);

Schedule::Schedule(int id, float sco, long long nxtTime) : idx(id), score(sco) {
    std::chrono::seconds dur(nxtTime);
    nxt = TimePoint(dur);
}
Schedule::Schedule(int id) : idx(id), score(0), nxt() {}
QString Schedule::toInf(QString title) {
    return title + "," + QString::number(idx) + "," + QString::number(score) + "," +
        QString::number(std::chrono::duration_cast<std::chrono::seconds>(nxt.time_since_epoch()).count());
}

void Schedule::update(int rating) {
    if (rating == -1) {
        nxt += ScheduleInfo.skipAmnt;
        return;
    } else if (rating == -2) {
        nxt += ScheduleInfo.redoAmnt;
        return;
    }
    if (rating < 0 || rating >= ScheduleInfo.ratesLen()) {
        Log::Error(MODULE) << "Found bad rating value: " << rating << "!";
        return;
    }
    auto diff = ScheduleInfo.ratingScos[rating];
    score = std::clamp(score + diff, 0.0f, float(ScheduleInfo.timings.size()-1));
    if (diff <= 0) {
        nxt = std::chrono::system_clock::now() + ScheduleInfo.failAmnt;
    } else {
        nxt = std::chrono::system_clock::now() + ScheduleInfo.timings[std::round(score)];
    }
}
Duration Schedule::getUpdatedTime(int rating) {
    auto diff = ScheduleInfo.ratingScos[rating];
    if (diff <= 0) {
        return ScheduleInfo.failAmnt;
    }
    return ScheduleInfo.timings[std::round(
        std::clamp(score + diff, 0.0f, float(ScheduleInfo.timings.size()-2))
    )];
}

float Schedule::percentage() const {
    return std::clamp(score / ScheduleInfo.learntSco, 0.0f, 1.0f);
}
TimePoint Schedule::trueNxt(TimePoint now) const {
    if (nxt == TimePoint{}) {
        return now;
    }
    return nxt;
}
bool Schedule::dueNow() const {
    return nxt <= std::chrono::system_clock::now();
}
bool Schedule::isNew() const {
    return nxt == TimePoint{};
}
