#pragma once

#include "system.h"

#include "StdAfx.h"

class CMaterial;

enum PRIMITIVEKIND
{
    PRIMITIVEKIND_POINTLIST = 1,
    PRIMITIVEKIND_LINELIST = 2,
    PRIMITIVEKIND_LINESTRIP = 3,
    PRIMITIVEKIND_TRIANGLELIST = 4,
    PRIMITIVEKIND_TRIANGLESTRIP = 5,
    PRIMITIVEKIND_TRIANGLEFAN = 6,
};

enum MATRIXKIND
{
    MATRIXKIND_VIEW = 2,
    MATRIXKIND_PROJECTION = 3,
    MATRIXKIND_TEXTURE0 = 16,
    MATRIXKIND_TEXTURE1 = 17,
    MATRIXKIND_TEXTURE2 = 18,
    MATRIXKIND_TEXTURE3 = 19,
    MATRIXKIND_TEXTURE4 = 20,
    MATRIXKIND_TEXTURE5 = 21,
    MATRIXKIND_TEXTURE6 = 22,
    MATRIXKIND_TEXTURE7 = 23,
};

#define MATRIXKIND_WORLDMATRIX(index) (MATRIXKIND)(index + 256)
#define MATRIXKIND_WORLD  MATRIXKIND_WORLDMATRIX(0)
#define MATRIXKIND_WORLD1 MATRIXKIND_WORLDMATRIX(1)
#define MATRIXKIND_WORLD2 MATRIXKIND_WORLDMATRIX(2)
#define MATRIXKIND_WORLD3 MATRIXKIND_WORLDMATRIX(3)

enum RENDERSTATE
{
    #define _X(NAME, VALUE) NAME = VALUE,
    #include "RenderStates.h"
    #undef _X
};

#define MAX_SAMPLER_STATES 13

enum CLEARFLAG
{
    CLEARFLAG_COLOR = 0x00000001L,
    CLEARFLAG_DEPTH = 0x00000002L,
    CLEARFLAG_STENCIL = 0x00000004L,

    CLEARFLAG_STANDARD = D3DCLEAR_TARGET | CLEARFLAG_COLOR | CLEARFLAG_DEPTH | CLEARFLAG_STENCIL,
};

enum TEXTURESLOT
{
    TEXTURESLOT_ALBEDO,
    TEXTURESLOT_SPECULAR,
    TEXTURESLOT_NORMAL,
    TEXTURESLOT_DISPLACE,

    TEXTURESLOT_USER_END,

    MAX_TEXTURE_SLOTS = 30,
};

struct VERTEX
{
    float x, y, z;
    float nx, ny, nz;
    float tx, ty, tz;
    float bx, by, bz;

    union
    {
        DWORD color;

        struct
        {
            BYTE a, r, g, b;
        };
    };

    float su, tv;
    float su2, tv2;
};

struct VERTEX_2D
{
    float x, y, z, rhw;

    union
    {
        DWORD color;

        struct
        {
            BYTE a, r, g, b;
        };
    };

    float su, tv;
};

extern D3DVERTEXELEMENT9 meshVertexFormat[];
extern D3DVERTEXELEMENT9 meshVertex2DFormat[];

struct ENGINE_API MATERIAL : D3DMATERIAL9
{
    float Opacity;
    bool Shaded;
    bool AlphaIsTransparency;
    bool AlphaTestEnabled;
    DWORD AlphaRef;
};

struct RENDERDATA
{
    D3DCOLOR color;

    union
    {
        unsigned int flags;
        DWORD state;
    };

    unsigned int kind;

    bool usesMatrix;
    D3DXMATRIX matrix;

    CMaterial* mat;

    LPD3DXMESH mesh;
    D3DXVECTOR3 meshOrigin;
    float meshRadius;
    D3DXVECTOR4 meshBounds[2];
};
