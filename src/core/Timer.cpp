//
// Created by mathi on 02/05/2026.
//

#include "Timer.h"

Timer::Timer(double minTime, double maxTime): minTime(minTime), maxTime(maxTime), accumulatedTime(minTime) {}

void Timer::start() {
    if (state != State::RUNNING) {
        lastStartTime = glfwGetTime();
        state = State::RUNNING;
    }
}

void Timer::pause() {
    if (state == State::RUNNING) {
        accumulatedTime = getCurrentValue();
        state = State::PAUSED;
    }
}

void Timer::reset() {
    accumulatedTime = minTime;
    state = State::INITIAL;
}

bool Timer::timeout() const {
    return (getCurrentValue() >= maxTime);
}

double Timer::getCurrentValue() const {
    double total = accumulatedTime;

    if (state == State::RUNNING) {
        total += (glfwGetTime() - lastStartTime);
    }

    return std::clamp(total, minTime, maxTime);
}

Timer::State Timer::getState() const { return state; }
