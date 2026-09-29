#pragma once
#include "gl/gl.h"
#include <SDL3/SDL.h>

// Wrap any GL call with GL_CHECK(...) in debug builds to catch the class of
// bug this engine's original JS version had none of: a bad enum or bad
// call silently doing nothing instead of failing loudly. Costs a
// glGetError() round trip per call, so it's compiled out in release.
#if !defined(NDEBUG)
	#define GL_CHECK(expr) \
		do { \
			expr; \
			GLenum portal_err = glGetError(); \
			if (portal_err != 0) \
			{ \
				SDL_LogError(SDL_LOG_CATEGORY_RENDER, \
					"GL error 0x%04X at %s:%d — %s", \
					portal_err, __FILE__, __LINE__, #expr); \
				SDL_TriggerBreakpoint(); \
			} \
		} while (0)
#else
	#define GL_CHECK(expr) expr
#endif
