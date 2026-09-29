#include "core/engine.h"
#include "gl/gl.h"
#include "gl/gl_check.h"
#include <SDL3/SDL.h>

bool Engine::init(const std::string& windowTitle, int width, int height)
{
    SDL_SetHint(SDL_HINT_VIDEO_FORCE_EGL, "1");
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_Init failed: %s", SDL_GetError());
        return false;
    }

    // Request a GLES-profile context on every platform (real GLES on
    // Android via the driver; on desktop this asks for a GLES-profile
    // context which Mesa/NVIDIA/AMD drivers on Linux and macOS's
    // ANGLE-backed path support — see README.md for desktop caveats
    // on Windows without ANGLE installed).
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, PORTAL_GLES_MAJOR);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, PORTAL_GLES_MINOR);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    window_ = SDL_CreateWindow(windowTitle.c_str(), width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!window_)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_CreateWindow failed: %s", SDL_GetError());
        return false;
    }

    glContext_ = SDL_GL_CreateContext(window_);
    if (!glContext_)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_GL_CreateContext failed: %s", SDL_GetError());
        return false;
    }

    if (!SDL_GL_MakeCurrent(window_, glContext_))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_GL_MakeCurrent failed: %s", SDL_GetError());
        return false;
    }

    SDL_Log("GL_VERSION: %s", glGetString(GL_VERSION));
    SDL_Log("GLSL_VERSION: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));

    if (!loadDesktopGL())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
            "Failed to load required GL entry points — see README's desktop GLES notes (ANGLE, driver support).");
        return false;
    }

    SDL_GL_SetSwapInterval(1); // vsync
    time_ = Time(1.0 / 60.0);
    running_ = true;
    return true;
}

void Engine::shutdown()
{
    if (glContext_) SDL_GL_DestroyContext(glContext_);
    if (window_) SDL_DestroyWindow(window_);
    SDL_Quit();
    glContext_ = nullptr;
    window_ = nullptr;
}

void Engine::run(
    const std::function<void(double fixedDt)>& onFixedUpdate,
    const std::function<void(double alpha)>& onRender,
    const std::function<void(const SDL_Event&)>& onEvent)
{
    uint64_t lastCounter = SDL_GetPerformanceCounter();
    const double freq = static_cast<double>(SDL_GetPerformanceFrequency());

    while (running_)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT) running_ = false;
            if (event.type == SDL_EVENT_WINDOW_RESIZED)
            {
                GL_CHECK(glViewport(0, 0, event.window.data1, event.window.data2));
            }
            if (onEvent) onEvent(event);
        }

        // lastCounter advances even while paused, so resuming doesn't see one
        // giant frame; time_ isn't fed while paused, so no steps queue up.
        uint64_t now = SDL_GetPerformanceCounter();
        double frameSeconds = static_cast<double>(now - lastCounter) / freq;
        lastCounter = now;

        if (!paused_)
        {
            time_.update(frameSeconds);
            while (time_.consumeStep())
                onFixedUpdate(time_.fixedDt());
        }

        onRender(time_.alpha());
        SDL_GL_SwapWindow(window_);
    }
}