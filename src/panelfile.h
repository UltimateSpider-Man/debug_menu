#pragma once

#include "float.hpp"

 #include "fetext.h"
#include "mvector.h"
#include "panelquad.h"


struct nglMeshFile;



struct PanelAnim;
struct PanelFile;

struct PanelAnimFile {
    mVector<PanelAnim> field_0;
    float field_14;
    int field_18;
    int field_1C;
    float field_20;
    int field_24;
    int field_28;
    bool field_2C;
    bool field_2D;

    //0x00628A40
    void Update(Float a2);

    //0x00617170
    void PostUnmashFixup(PanelFile* a1);
};
struct PanelFile {
    mVector<PanelQuad> pquads;
    mVector<FEText> ptext;
    mVector<PanelAnimFile> field_28;

    //0x00616A60
    void Draw();

    //0x00628960
    void PostUnmashFixup(panel_layer a3);


    //0x00616B40
  FEText *GetTextPointer(const char *a2);


  PanelQuad* GetPQ(const char* a2) {
      TRACE("PanelFile::GetTextPointer", a2);


      CDECL_CALL(0x000639E40, a2);



      return nullptr;


  }
  PanelAnimFile* GetAnimationPointer(int a1);

    //0x0062E4D0
   void Update(Float a2) {
       TRACE("PanelFile::GetTextPointer", a2);


       CDECL_CALL(0x0062E4D0, a2);


   }



    static PanelFile* UnmashPanelFile(const char* a1, panel_layer a2) {
        


     return (PanelFile*) THISCALL(0x00643000, a1, a2);

        

    }

    static inline nglMeshFile *& g_curmeshfile = var<nglMeshFile *>(0x0096B444);
};

extern void PanelFile_patch();
