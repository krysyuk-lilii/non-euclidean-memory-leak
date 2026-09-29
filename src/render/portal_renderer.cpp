#include "render/portal_renderer.h"
#include "render/camera.h"
#include "render/render_pass.h"
#include "core/portal.h"
#include "core/scene.h"
#include "core/game_object.h"
#include "gl/gl_check.h"
#include <SDL3/SDL.h>
#include <glm/gtc/matrix_transform.hpp>

bool PortalRenderer::init(const std::string &shader, uint32_t fbSize)
{
    shader_ = Shader::loadFromFiles(shader);
    if (!shader_.valid()) return false;
    fbSize_ = fbSize;

    constexpr int segments = 32;
    std::vector<float> verts;
    verts.reserve((segments + 2) * 3);
    verts.insert(verts.end(), {0, 0, 0}); // center
    for (int i = 0; i <= segments; ++i)
    {
        float t = (float)i / (float)segments * 2.0f * 3.14159265f;
        verts.insert(verts.end(), {cosf(t), sinf(t), 0.0f});
    }

    GL_CHECK(glGenVertexArrays(1, &vao_));
    GL_CHECK(glBindVertexArray(vao_));
    GL_CHECK(glGenBuffers(1, &vbo_));
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vbo_));
    GL_CHECK(glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), verts.data(), GL_STATIC_DRAW));
    GLuint aPos = static_cast<GLuint>(shader_.attribute("a_position"));
    GL_CHECK(glEnableVertexAttribArray(aPos));
    GL_CHECK(glVertexAttribPointer(aPos, 3, GL_FLOAT, GL_FALSE, 0, nullptr));
    GL_CHECK(glBindVertexArray(0));
    vertexCount_ = segments + 2;
    return true;
}
void PortalRenderer::shutdown()
{
    fbs_.clear();
    if (vbo_) glDeleteBuffers(1, &vbo_);
    if (vao_) glDeleteVertexArrays(1, &vao_);
    vbo_ = vao_ = 0;
}
FrameBuffer& PortalRenderer::ensureFb(const Portal& p)
{
    auto it = fbs_.find(&p);
    if (it != fbs_.end()) return *it->second;
    auto fb = std::make_unique<FrameBuffer>();
    fb->create(fbSize_);
    return *fbs_.emplace(&p, std::move(fb)).first->second;
}
void PortalRenderer::renderViews(const Camera& camera, Scene& current, const DrawSceneFn& drawScene)
{
    // Create every FBO that could be touched this frame BEFORE any pass starts.
    // FrameBuffer::create() unbinds the framebuffer when it finishes, so creating
    // one lazily inside a pass would silently kick us out of the active target.
    for (auto& p : current.portals)
    {
        ensureFb(*p);
        if (p->linkedScene)
            for (auto& q : p->linkedScene->portals)
                ensureFb(*q);
    }

    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);

    const glm::mat4 playerWorld = glm::inverse(camera.view());

    for (auto& up : current.portals)
    {
        Portal& p = *up;
        if (!p.linked || !p.linkedScene) continue;

        Camera portalCam;
        portalCam.setup(camera.width, camera.height, camera.near, camera.far, camera.fovY);
        portalCam.setView(Camera::viewThroughPortal(playerWorld,
            p.obj->localToWorld(), p.linked->localToWorld()));
        portalCam.clipOblique(p.linked->pos, -p.linked->forward());

        FrameBuffer& fb = ensureFb(p);
        activeTarget_ = &fb;
        activeDest_ = p.linked;
        RenderPass::run(
            portalCam, &fb,
            [&](const Camera& cam) { drawScene(cam, *p.linkedScene); },
            p.linkedScene->backgroundColor);
    }
    activeTarget_ = nullptr;
    activeDest_ = nullptr;

    glViewport(vp[0], vp[1], vp[2], vp[3]);
}
void PortalRenderer::draw(const Portal& p, const Camera& cam)
{
    auto it = fbs_.find(&p);
    if (it == fbs_.end()) return;
    const FrameBuffer& fb = *it->second;

    // Never sample the texture we are currently rendering into, and don't draw
    // the destination portal's quad (it sits exactly on the oblique clip plane).
    if (&fb == activeTarget_ || p.obj == activeDest_) return;

    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);   // correct in both the main pass and FBO passes
    glm::mat4 world = p.obj->localToWorld() * glm::scale(glm::mat4(1.0f), glm::vec3(p.radius, p.radius, 1.0f));
    glm::mat4 mvp = cam.viewProjection() * world;

    shader_.use();
    GL_CHECK(glUniformMatrix4fv(shader_.uniform("u_mvp"), 1, GL_FALSE, &mvp[0][0]));
    GL_CHECK(glUniform2f(shader_.uniform("u_resolution"),
        static_cast<float>(vp[2]), static_cast<float>(vp[3])));
    fb.bindAsTexture(0);
    GL_CHECK(glUniform1i(shader_.uniform("u_portalTex"), 0));

    GL_CHECK(glDisable(GL_CULL_FACE));
    GL_CHECK(glBindVertexArray(vao_));
    GL_CHECK(glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount_));
    GL_CHECK(glBindVertexArray(0));
    GL_CHECK(glEnable(GL_CULL_FACE));
}