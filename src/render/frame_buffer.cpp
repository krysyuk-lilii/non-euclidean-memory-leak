#include "render/frame_buffer.h"
#include "gl/gl_check.h"
#include <SDL3/SDL.h>
void FrameBuffer::create(int size)
{
    size_ = size;

    GL_CHECK(glGenFramebuffers(1, &fbo_));
    GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, fbo_));

    GL_CHECK(glGenTextures(1, &texture_));
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, texture_));
    GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size, size, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr));
    
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_, 0));

    GL_CHECK(glGenRenderbuffers(1, &rbo_));
    GL_CHECK(glBindRenderbuffer(GL_RENDERBUFFER, rbo_));
    GL_CHECK(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, size, size));
    GL_CHECK(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo_));

    
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        SDL_LogError(SDL_LOG_CATEGORY_RENDER, "FrameBuffer incomplete (size %d)", size);
    }

    GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

FrameBuffer::~FrameBuffer()
{
    // Deliberately not calling glDelete*: on desktop these are
    // function pointers that may already be null if the GL context
    // was torn down first (e.g. at process exit). A production build
    // should route teardown through an explicit shutdown() called
    // before context destruction rather than relying on the
    // destructor's ordering relative to the context.
}

void FrameBuffer::bindAsTarget() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    glViewport(0, 0, size_, size_);
}

void FrameBuffer::unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::bindAsTexture(uint32_t unit) const
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, texture_);
}