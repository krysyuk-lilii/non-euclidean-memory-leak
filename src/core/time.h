#pragma once
#include <cstdint>
// Fixed-timestep accumulator. update() is called once per real frame
// with the elapsed wall-clock seconds; consumeStep() is then called in
// a loop until it returns false, each true call meaning "advance
// simulation by fixedDt seconds". This is what makes GameObject
// movement and portal warp math independent of display refresh rate —
// the original engine's `pos[0] += vel[0]` per rendered frame did not
// have this property.
class Time
{
public:
    explicit Time(double fixedDt = 1.0 / 60.0) : fixedDt_(fixedDt) {}

    // spikeGuard caps how much wall-clock time a single frame can
    // contribute — protects against a huge simulation catch-up burst
    // after e.g. the app was backgrounded.
    void update(double frameSeconds, double spikeGuard = 0.25)
    {
        if (frameSeconds > spikeGuard) frameSeconds = spikeGuard;
        accumulator_ += frameSeconds;
        frameCount_++;
    }

    bool consumeStep()
    {
        if (accumulator_ < fixedDt_) return false;
        accumulator_ -= fixedDt_;
        return true;
    }

    // 0..1 — how far between the last simulated step and the next one
    // the current real frame is. Use to interpolate render transforms
    // for smooth visuals at fixedDt < display refresh rate.
    double alpha() const { return accumulator_ / fixedDt_; }
    double fixedDt() const { return fixedDt_; }
    uint64_t frameCount() const { return frameCount_; }

private:
    double fixedDt_;
    double accumulator_ = 0.0;
    uint64_t frameCount_ = 0;
};