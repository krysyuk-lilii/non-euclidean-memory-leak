#pragma once
#include "gl/gl.h"
#include "gl/shader.h"
#include "render/frame_buffer.h"
#include "core/portal.h"
#include <glm/glm.hpp>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

class Camera;
class GameObject;
class Scene;

class PortalRenderer
{
public:
    using DrawSceneFn = std::function<void(const Camera&, Scene&)>;

    bool init(const std::string& shaderBasePath, uint32_t fbSize = 1024);
    void shutdown();

    // Pre-pass: render what each portal in `current` looks into, into that portal's FBO.
    void renderViews(const Camera& camera, Scene& current, const DrawSceneFn& drawScene);

    // Called from a portal object's onRender during a normal scene draw.
    void draw(const Portal& p, const Camera& cam);

    void setDebugClearColor(const glm::vec4& c) { clearColor_ = c; }

    bool shouldSkipCase(const Portal& p) const { return p.obj == activeDest_; }

private:
    FrameBuffer& ensureFb(const Portal& p);

    Shader shader_;
    GLuint vao_ = 0, vbo_ = 0;
    uint32_t fbSize_ = 1024;
    int vertexCount_ = 0;
    glm::vec4 clearColor_{0.0f, 0.0f, 0.0f, 1.0f};
    std::unordered_map<const Portal*, std::unique_ptr<FrameBuffer>> fbs_;

    // Set only while a pre-pass is running; used to skip quads that must not draw.
    const FrameBuffer* activeTarget_ = nullptr;
    const GameObject* activeDest_ = nullptr;
};