//
// Created by mathi on 02/05/2026.
//

#pragma once

#include <GLFW/glfw3.h>
#include <algorithm>

class Timer {
public:
    enum class State { INITIAL, RUNNING, PAUSED };

private:
    State state = State::INITIAL;
    double minTime = 0.0;
    double maxTime = 0.0;

    double accumulatedTime = 0.0;
    double lastStartTime = 0.0;

public:
    Timer(double minTime, double maxTime);

    void start();

    void pause();

    void reset();

    bool timeout() const;

    [[nodiscard]] double getCurrentValue() const;

    [[nodiscard]] State getState() const;
};
