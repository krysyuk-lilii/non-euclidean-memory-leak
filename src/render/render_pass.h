#pragma once
#include "render/frame_buffer.h"
#include "render/camera.h"
#include <functional>
#include <glm/glm.hpp>
class RenderPass
{
public:
    using DrawFn = std::function<void(const Camera&)>;

    // target == nullptr renders straight to the default framebuffer
    // (the screen) instead of an offscreen FrameBuffer.
    static void run(
        const Camera& camera, FrameBuffer* target,
        const DrawFn& draw,
        const glm::vec4& clearColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
};
