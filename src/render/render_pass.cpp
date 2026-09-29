#include "render/render_pass.h"
#include "gl/gl_check.h"

void RenderPass::run(const Camera& camera, FrameBuffer* target, const DrawFn& draw, const glm::vec4& clearColor)
{
    if (target) target->bindAsTarget();
    else FrameBuffer::unbind();
    GL_CHECK(glEnable(GL_DEPTH_TEST));
    GL_CHECK(glEnable(GL_CULL_FACE));
    GL_CHECK(glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a));
    GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
    draw(camera);
    if (target) FrameBuffer::unbind();
}
