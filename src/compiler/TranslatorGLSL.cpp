//
// Copyright (c) 2002-2010 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#include "compiler/TranslatorGLSL.h"

#include "compiler/OutputGLSL.h"
#include "compiler/VersionGLSL.h"

extern bool glsl14;

static void writeVersion(ShShaderType type, TIntermNode* root,
                         TInfoSinkBase& sink) {
    TVersionGLSL versionGLSL(type);
    root->traverse(&versionGLSL);
    int version = versionGLSL.getVersion();
    // We need to write version directive only if it is greater than 110.
    // If there is no version directive in the shader, 110 is implied.

    if (glsl14) sink << "#version 140 \n";
    else sink << "#version " << version << "\n";

    if (glsl14 && type == SH_FRAGMENT_SHADER) sink << "out vec4 kore_FragColor;\n";
}

TranslatorGLSL::TranslatorGLSL(ShShaderType type, ShShaderSpec spec)
    : TCompiler(type, spec) {
}

void TranslatorGLSL::translate(TIntermNode* root) {
    TInfoSinkBase& sink = getInfoSink().obj;

    // Write GLSL version.
    writeVersion(getShaderType(), root, sink);

    // Write emulated built-in functions if needed.
    getBuiltInFunctionEmulator().OutputEmulatedFunctionDefinition(
        sink, false);

    // Write array bounds clamping emulation if needed.
    getArrayBoundsClamper().OutputClampingFunctionDefinition(sink);

    // Write translated shader.
    TOutputGLSL outputGLSL(sink, getArrayIndexClampingStrategy(), getHashFunction(), getNameMap(), getSymbolTable());
    root->traverse(&outputGLSL);
}
