#include "simulation/SimEngine.h"
#include <cmath>
#include <stdexcept>

std::string scenarioName(Scenario s) {
    switch (s) {
        case Scenario::STRAIGHT_AND_LEVEL: return "Straight and Level";
        case Scenario::CLIMB:              return "Climb";
        case Scenario::DESCENT:            return "Descent";
        case Scenario::TURN_LEFT:          return "Turn Left";
        case Scenario::TURN_RIGHT:         return "Turn Right";
        case Scenario::COMBINED:           return "Combined (Climb + Turn)";
    }
    return "Unknown";
}

SimEngine::SimEngine(double dt_s) : dt_(dt_s) {
    if (dt_s <= 0.0)
        throw std::invalid_argument("dt must be positive");
}

void SimEngine::setState(const FlightState& state) { state_ = state; }
const FlightState& SimEngine::getState() const { return state_; }

void SimEngine::applyPhysics(Scenario scenario) {
    constexpr double CLIMB_RATE_FPM    =  1500.0;
    constexpr double DESCENT_RATE_FPM  = -1000.0;
    constexpr double TURN_RATE_DPS     =  3.0;   // standard rate turn
    constexpr double CRUISE_VS_FPM     =  0.0;

    // Set vertical speed and roll based on scenario
    switch (scenario) {
        case Scenario::STRAIGHT_AND_LEVEL:
            state_.vertical_speed_fpm = CRUISE_VS_FPM;
            state_.roll_deg = 0.0;
            state_.pitch_deg = 0.0;
            break;
        case Scenario::CLIMB:
            state_.vertical_speed_fpm = CLIMB_RATE_FPM;
            state_.roll_deg = 0.0;
            state_.pitch_deg = 8.0;
            break;
        case Scenario::DESCENT:
            state_.vertical_speed_fpm = DESCENT_RATE_FPM;
            state_.roll_deg = 0.0;
            state_.pitch_deg = -3.0;
            break;
        case Scenario::TURN_LEFT:
            state_.vertical_speed_fpm = CRUISE_VS_FPM;
            state_.roll_deg = -25.0;
            state_.pitch_deg = 2.0;
            break;
        case Scenario::TURN_RIGHT:
            state_.vertical_speed_fpm = CRUISE_VS_FPM;
            state_.roll_deg = 25.0;
            state_.pitch_deg = 2.0;
            break;
        case Scenario::COMBINED:
            state_.vertical_speed_fpm = CLIMB_RATE_FPM;
            state_.roll_deg = 20.0;
            state_.pitch_deg = 8.0;
            break;
    }

    // Integrate altitude: fpm -> ft/s -> ft
    state_.altitude_ft += state_.vertical_speed_fpm / 60.0 * dt_;

    // Integrate heading for turns
    if (scenario == Scenario::TURN_LEFT || scenario == Scenario::COMBINED) {
        state_.heading_deg -= TURN_RATE_DPS * dt_;
    } else if (scenario == Scenario::TURN_RIGHT) {
        state_.heading_deg += TURN_RATE_DPS * dt_;
    }

    // Normalize heading to [0, 360)
    state_.heading_deg = std::fmod(state_.heading_deg + 360.0, 360.0);

    // Simple airspeed model: climbs bleed speed, descents gain speed
    if (scenario == Scenario::CLIMB || scenario == Scenario::COMBINED) {
        state_.airspeed_kts = std::max(state_.airspeed_kts - 0.5 * dt_, 150.0);
    } else if (scenario == Scenario::DESCENT) {
        state_.airspeed_kts = std::min(state_.airspeed_kts + 0.3 * dt_, 350.0);
    }

    state_.altitude_ft = std::max(state_.altitude_ft, 0.0);
    state_.time_s += dt_;
}

void SimEngine::step(Scenario scenario) {
    applyPhysics(scenario);
}

void SimEngine::run(double duration_s, Scenario scenario,
                    std::function<void(const FlightState&)> on_step) {
    int steps = static_cast<int>(duration_s / dt_);
    for (int i = 0; i < steps; ++i) {
        applyPhysics(scenario);
        if (on_step) on_step(state_);
    }
}
