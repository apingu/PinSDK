/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#ifndef DSCANVAS_H
#define DSCANVAS_H

/* This is a trivial JNI example where we use a native method
 * to return a new VM String. See the corresponding Java source
 * file located at:
 *
 *   apps/samples/hello-jni/project/src/com/example/HelloJni/HelloJni.java
 *
 */
#include "DsDef.h"
#include <Graphic3d.h>

namespace DrawSprit
{
    class Canvas
    {
        int           m_Top;
        int           m_Left;
        uint_t        m_Width;
        uint_t        m_Height;
        DWORD         m_SetFlag;

        LG::Axis3f    m_CameraAxis;

    public:

        Canvas();
         ~Canvas();

        bool    Create(int top, int left, uint_t width, uint_t height);
        void    Resize(int top, int left, uint_t width, uint_t height);
        uint_t  GetWidth();
        uint_t  GetHeight();
        bool    BeginFrame(DWORD argv);
        void    EndFrame();

        uint_t  LoadImage(const void* pixels, int width, int height);
        void    FreeImage(uint_t textureid);
    };

}

#endif
