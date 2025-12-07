#include "note.hpp"
#include "../log.hpp"

const QString MODULE = "Schedule";

_scheduleInf::_scheduleInf(std::vector<float> rScos, QString ts, Duration skpAmnt)
    :ratingScos(rScos), skipAmnt(skpAmnt) {
        setTimings(ts);
    }

using DurationFn = std::function<Duration(long long)>;
#define makeTimeFn(time) [](long long n){ return time(n); }
static const std::unordered_map<QString, DurationFn> & getUnitMap() {
    static const std::unordered_map<QString, DurationFn> unitMap = []{
        std::unordered_map<QString, DurationFn> m;
        auto add = [&](const QStringList &keys, DurationFn fn) {
            for (const auto &k : keys) m.emplace(k, fn);
        };
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
    {-1, -0.5, 0.5, 1},
    "30sec\n"
    "1min\n"
    "1min\n"
    "2min\n"
    "5mins\n"
    "10mins\n"
    "20mins\n"
    "40mins\n"
    "1hr\n"
    "1hr30min\n"
    "2hrs\n"
    "4hrs\n"
    "6hrs\n"
    "1day\n"
    "1day12hours\n"
    "2days\n"
    "4days\n"
    "6days\n"
    "1wk\n"
    "1wk5days\n"
    "2wks\n"
    "3wks\n"
    "4wks\n"
    "1mo\n"
    "1mo2wks\n"
    "2mo\n"
    , parseWholeDuration("40mins")
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

Duration redoAmount = std::chrono::minutes(1);
void Schedule::update(int rating) {
    if (rating == -1) {
        nxt += ScheduleInfo.skipAmnt;
        return;
    } else if (rating == -2) {
        nxt += redoAmount;
        return;
    }
    if (rating < 0 || rating >= ScheduleInfo.ratesLen()) {
        Log::Error(MODULE) << "Found bad rating value: " << rating << "!";
        return;
    }
    score = std::clamp(score + ScheduleInfo.ratingScos[rating], 0.0f, float(ScheduleInfo.timings.size()-1));
    nxt = std::chrono::system_clock::now() + ScheduleInfo.timings[std::round(score)];
}
Duration Schedule::getUpdatedTime(int rating) {
    return ScheduleInfo.timings[std::round(
        std::clamp(score + ScheduleInfo.ratingScos[rating], 0.0f, float(ScheduleInfo.timings.size()-1))
    )];
}

