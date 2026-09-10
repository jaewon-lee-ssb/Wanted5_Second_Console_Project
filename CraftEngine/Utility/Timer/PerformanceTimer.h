#pragma once

#include <chrono>
#include <functional>
#include <string>

// 범위를 벗어날 때까지 걸린 시간을 측정하고 TXT 파일에 누적 기록한다.
// 화면 출력이나 Sleep처럼 측정에서 제외할 구간은 Pause/Resume을 사용한다.
class PerformanceTimer
{
public:
    explicit PerformanceTimer(
        std::string measurementName,
        std::string outputFileName = "AStarPerformance.txt");
    ~PerformanceTimer();

    PerformanceTimer(const PerformanceTimer&) = delete;
    PerformanceTimer& operator=(const PerformanceTimer&) = delete;

    void Pause();
    void Resume();

    // 즉시 측정을 끝내고 기록한다. 호출하지 않아도 소멸자가 자동으로 처리한다.
    void StopAndSave();

    double GetElapsedMilliseconds() const;

    // 측정 종료 시 추가로 기록할 내용을 만든다.
    // 탐색 도중 문자열을 계속 만들지 않도록 종료 시 한 번만 호출된다.
    void SetDetailsProvider(std::function<std::string()> provider);

private:
    using Clock = std::chrono::steady_clock;

    void SaveToFile(double elapsedMilliseconds) const;

    std::string measurementName;
    std::string outputFileName;
    Clock::time_point startedAt;
    Clock::duration accumulatedTime = Clock::duration::zero();
    bool isRunning = true;
    bool isSaved = false;
    std::function<std::string()> detailsProvider;
};
