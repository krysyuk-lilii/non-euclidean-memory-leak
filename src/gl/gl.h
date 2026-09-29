#pragma once
#if defined(__ANDROID__)
    #include <GLES3/gl31.h>
    #include <GLES2/gl2ext.h>
#else
    #include <GLES3/gl3.h>   // real declarations via ANGLE — no manual loading needed
#endif
inline bool loadDesktopGL() { return true; }