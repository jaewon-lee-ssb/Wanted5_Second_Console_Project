#include "PerformanceTimer.h"

#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <utility>

PerformanceTimer::PerformanceTimer(
    std::string measurementName,
    std::string outputFileName)
    : measurementName(std::move(measurementName)),
      outputFileName(std::move(outputFileName)),
      startedAt(Clock::now())
{
}

PerformanceTimer::~PerformanceTimer()
{
    StopAndSave();
}

void PerformanceTimer::Pause()
{
    if (!isRunning || isSaved)
    {
        return;
    }

    accumulatedTime += Clock::now() - startedAt;
    isRunning = false;
}

void PerformanceTimer::Resume()
{
    if (isRunning || isSaved)
    {
        return;
    }

    startedAt = Clock::now();
    isRunning = true;
}

void PerformanceTimer::StopAndSave()
{
    if (isSaved)
    {
        return;
    }

    if (isRunning)
    {
        accumulatedTime += Clock::now() - startedAt;
        isRunning = false;
    }

    SaveToFile(GetElapsedMilliseconds());
    isSaved = true;
}

double PerformanceTimer::GetElapsedMilliseconds() const
{
    Clock::duration elapsed = accumulatedTime;

    if (isRunning)
    {
        elapsed += Clock::now() - startedAt;
    }

    return std::chrono::duration<double, std::milli>(elapsed).count();
}

void PerformanceTimer::SetDetailsProvider(std::function<std::string()> provider)
{
    detailsProvider = std::move(provider);
}

void PerformanceTimer::SaveToFile(double elapsedMilliseconds) const
{
    std::ofstream output(outputFileName, std::ios::app);
    if (!output)
    {
        std::cerr << "성능 로그 파일을 열 수 없습니다: " << outputFileName << '\n';
        return;
    }

    const std::time_t now = std::time(nullptr);
    std::tm localTime = {};
    localtime_s(&localTime, &now);

    output << '[' << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S") << "] "
           << measurementName << " | "
           << std::fixed << std::setprecision(6)
           << elapsedMilliseconds << " ms";

    if (detailsProvider)
    {
        const std::string details = detailsProvider();
        if (!details.empty())
        {
            output << " | " << details;
        }
    }

    output << '\n';
}
