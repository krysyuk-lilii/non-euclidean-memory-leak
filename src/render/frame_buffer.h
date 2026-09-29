#pragma once
#include <cstdint>
#include "gl/gl_check.h"

class FrameBuffer
{
public:
    FrameBuffer() = default;
    void create(int size);
    ~FrameBuffer();
    FrameBuffer(const FrameBuffer&) = delete;
    FrameBuffer& operator=(const FrameBuffer&) = delete;

    void bindAsTarget() const;
    static void unbind();
    void bindAsTexture(uint32_t unit) const;

    uint32_t size() { return size_; }

private:
    GLuint fbo_ = 0, texture_ = 0, rbo_ = 0;
    uint32_t size_ = 0;
};