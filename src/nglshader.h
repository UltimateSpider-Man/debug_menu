#pragma once

#include "nglrendernode.h"

#include "variable.h"

#include <cstdint>

struct color;
struct nglMeshNode;
struct nglMeshSection;
struct nglMaterialBase;
struct tlFixedString;

template<typename>
struct nglParamSet;

struct nglShaderParamSet_Pool;

struct nglShader {
    std::intptr_t m_vtbl;
    nglShader *field_4;
    int field_8;

    nglShader();

    void Register();

    /* virtual */ tlFixedString GetName();

    //virtual
    void AddNode(nglMeshNode *a1, nglMeshSection *a2, nglMaterialBase *a3);

    /* virtual */ void BindMaterial(nglMaterialBase *mat);

    /* virtual */ void ReleaseMaterial(nglMaterialBase *mat);

    /* virtual */ void RebaseMaterial(nglMaterialBase *mat, unsigned int a2);

    /* virtual */ bool CheckMaterialVersion(nglMaterialBase *mat);

    /* virtual */ bool CheckVertexDefVersion(nglMeshSection *Section);

    /* virtual */ void BindSection(nglMeshSection *Section);

    /* virtual */ bool IsSwitchable();

    static Var<int> NextID;
};

struct nglShaderNode : nglRenderNode {
    nglMeshNode *m_meshNode;
    nglMeshSection *m_meshSection;

    void sub_413AF0();

    void Render();
};

extern color *sub_413F80(color *a1, nglMaterialBase *a2, nglParamSet<nglShaderParamSet_Pool> *a3, uint32_t a4);

extern void nglShader_patch();
