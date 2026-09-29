#pragma once
#include "core/time.h"
#include "core/scene.h"
#include <SDL3/SDL.h>
#include <functional>
#include <string>
class Engine
{
public:
    bool init(const std::string& windowTitle, int32_t width, int32_t height);
    void shutdown();

    SDL_Window* window() const { return window_; }

    void setPaused(bool p) { paused_ = p; }
    bool isPaused() const { return paused_; }
    
    void run(const std::function<void(double fixedDt)>& onFixedUpdate,
			const std::function<void(double alpha)>& onRender,
			const std::function<void(const SDL_Event&)>& onEvent = nullptr);

private:
    SDL_Window* window_ = nullptr;
    SDL_GLContext glContext_ = nullptr;
    Time time_;
    bool running_ = false;
    bool paused_ = false;
};