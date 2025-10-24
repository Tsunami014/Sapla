#pragma once
#include "items/base/svgBtnItem.hpp"

namespace Log {
    enum Level { DEBUG, INFO, WARN, ERROR };
    struct LogType {
        inline LogType(Level l, QString m, QString module) : lvl(l), msg(m), mod(module) {};
        Level lvl;
        QString msg;
        QString mod;
    };
    extern std::vector<LogType> logs;

    class Log {
    public:
        Log(QString module, Level level = INFO) : mod(module), lvl(level), buf(), stream(&buf) {}
        ~Log();

        template <typename T>
        inline Log& operator<<(const T& value) {
            stream << value;
            return *this;
        }

    private:
        QString buf;
        QTextStream stream;
        Level lvl;
        QString mod;
    };

    inline Log Debug(QString module) { return Log(module, DEBUG); }
    inline Log Info(QString module) { return Log(module, INFO); }
    inline Log Warn(QString module) { return Log(module, WARN); }
    inline Log Error(QString module) { return Log(module, ERROR); }
}

void showLogWindow();

class LogAlert : public TxtBtnItem {
public:
    LogAlert(Log::Level lvl, QString msg);
    void deleteMe();
};

