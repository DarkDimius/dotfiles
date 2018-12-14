#ifndef SRUBY_PROGRESSINDICATOR_H
#define SRUBY_PROGRESSINDICATOR_H

#include "progressbar/progressbar.h"
#include <atomic>
#include <chrono>
#include <memory>
#include <string>
#include <thread>

class ProgressIndicator {
    const int progressExpected;
    std::string name;
    std::thread::id outputThreadId;
    std::unique_ptr<progressbar> progress;
    bool enabled;
    std::chrono::time_point<std::chrono::system_clock> lastReported;
    static std::chrono::time_point<std::chrono::system_clock> getCurrentTimeMillis() {
        return std::chrono::system_clock::now();
    }

public:
    static inline constexpr std::chrono::milliseconds REPORTING_INTERVAL() {
        using namespace std::chrono_literals;
        return 250ms;
    };

    ProgressIndicator(bool enabled, std::string name, int progressExpected);

    ~ProgressIndicator();

    void reportProgress(int current);
};

#endif // SRUBY_PROGRESSINDICATOR_H
