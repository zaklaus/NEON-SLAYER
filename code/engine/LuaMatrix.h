#pragma once

#include "system.h"

#include <lua/lua.hpp>

auto matrix_new(lua_State* L) -> int
{
    auto* const mat = static_cast<D3DXMATRIX*>(lua_newuserdata(L, sizeof(D3DXMATRIX)));
    D3DXMatrixIdentity(mat);

    luaL_setmetatable(L, L_MATRIX);
    return 1;
}

static auto matrix_translate(lua_State* L) -> int
{
    auto* const mat = static_cast<D3DXMATRIX*>(luaL_checkudata(L, 1, L_MATRIX));
    const D3DXVECTOR3 vec = luaH_getcomps(L);

    D3DXMATRIX t;
    D3DXMatrixTranslation(&t, vec.x, vec.y, vec.z);

    *mat *= t;

    lua_pushvalue(L, 1);
    return 1;
}

static auto matrix_rotate(lua_State* L) -> int
{
    auto* const mat = static_cast<D3DXMATRIX*>(luaL_checkudata(L, 1, L_MATRIX));
    const D3DXVECTOR3 vec = luaH_getcomps(L);

    D3DXMATRIX t;
    D3DXMatrixRotationYawPitchRoll(&t, vec.x, vec.y, vec.z);

    *mat *= t;

    lua_pushvalue(L, 1);
    return 1;
}

static auto matrix_scale(lua_State* L) -> int
{
    auto* const mat = static_cast<D3DXMATRIX*>(luaL_checkudata(L, 1, L_MATRIX));
    const D3DXVECTOR3 vec = luaH_getcomps(L);
    D3DXMATRIX t;
    D3DXMatrixScaling(&t, vec.x, vec.y, vec.z);
    *mat *= t;

    lua_pushvalue(L, 1);
    return 1;
}

static auto matrix_reflect(lua_State* L) -> int
{
    auto* const mat = static_cast<D3DXMATRIX*>(luaL_checkudata(L, 1, L_MATRIX));
    const auto vec = luaH_getcomps(L);
    D3DXMATRIX t;
    D3DXPLANE plane(vec.x, vec.y, vec.z, vec.w);
    D3DXMatrixReflect(&t, &plane);
    *mat *= t;

    lua_pushvalue(L, 1);
    return 1;
}

static auto matrix_shadow(lua_State* L) -> int
{
    auto* const mat = static_cast<D3DXMATRIX*>(luaL_checkudata(L, 1, L_MATRIX));
    const auto vec = luaH_getcomps(L);
    auto lit = luaH_getcomps(L, 1);
    D3DXMATRIX t;
    D3DXPLANE plane(vec.x, vec.y, vec.z, vec.w);
    D3DXMatrixShadow(&t, &lit, &plane);
    *mat *= t;

    lua_pushvalue(L, 1);
    return 1;
}

static auto matrix_mul(lua_State* L) -> int
{
    auto* const mat = static_cast<D3DXMATRIX*>(luaL_checkudata(L, 1, L_MATRIX));
    auto* const matRHS = static_cast<D3DXMATRIX*>(luaL_checkudata(L, 2, L_MATRIX));

    auto* const out = static_cast<D3DXMATRIX*>(lua_newuserdata(L, sizeof(D3DXMATRIX)));
    D3DXMatrixMultiply(out, mat, matRHS);

    luaL_setmetatable(L, L_MATRIX);
    return 1;
}

static auto matrix_bind(lua_State* L) -> int
{
    auto* const mat = static_cast<D3DXMATRIX*>(luaL_checkudata(L, 1, L_MATRIX));
    const auto kind = static_cast<float>(luaL_checknumber(L, 2));
    RENDERER->SetMatrix(static_cast<unsigned int>(kind), *mat);

    lua_pushvalue(L, 1);
    return 1;
}

static auto matrix_lookat(lua_State* L) -> int
{
    auto* mat = static_cast<D3DXMATRIX*>(luaL_checkudata(L, 1, L_MATRIX));
    auto* const eye = static_cast<D3DXVECTOR3*>(luaL_checkudata(L, 2, L_VECTOR));
    auto* const at = static_cast<D3DXVECTOR3*>(luaL_checkudata(L, 3, L_VECTOR));
    auto* const up = static_cast<D3DXVECTOR3*>(luaL_checkudata(L, 4, L_VECTOR));
    D3DXMATRIX t;

    D3DXMatrixLookAtLH(&t, eye, at, up);

    *mat *= t;
    lua_pushvalue(L, 1);
    return 1;
}

static auto matrix_persp(lua_State* L) -> int
{
    auto* const mat = static_cast<D3DXMATRIX*>(luaL_checkudata(L, 1, L_MATRIX));
    const auto fov = static_cast<float>(luaL_checknumber(L, 2));
    auto zNear = 0.1f;
    auto zFar = 1000.0f;
    auto flipHandedness = FALSE;

    if (lua_gettop(L) >= 4)
    {
        zNear = static_cast<float>(luaL_checknumber(L, 3));
        zFar = static_cast<float>(luaL_checknumber(L, 4));
    }

    if (lua_gettop(L) >= 5)
    {
        flipHandedness = static_cast<int>(static_cast<bool>(lua_toboolean(L, 5)));
    }

    const auto res = RENDERER->GetSurfaceResolution();

    if (flipHandedness != 0)
    {
        D3DXMatrixPerspectiveFovRH(mat,
                                   D3DXToRadian(fov),
                                   static_cast<float>(res.right) / static_cast<float>(res.bottom),
                                   zNear,
                                   zFar);
    }
    else
    {
        D3DXMatrixPerspectiveFovLH(mat,
                                   D3DXToRadian(fov),
                                   static_cast<float>(res.right) / static_cast<float>(res.bottom),
                                   zNear,
                                   zFar);
    }

    lua_pushvalue(L, 1);
    return 1;
}

static auto matrix_ortho(lua_State* L) -> int
{
    auto* const mat = static_cast<D3DXMATRIX*>(luaL_checkudata(L, 1, L_MATRIX));
    const auto res = RENDERER->GetSurfaceResolution();
    auto w = static_cast<float>(res.right);
    auto h = static_cast<float>(res.bottom);
    auto flipHandedness = FALSE;

    if (lua_gettop(L) >= 3)
    {
        w = static_cast<float>(luaL_checknumber(L, 2)) * (static_cast<float>(res.right) / static_cast<float>(res.bottom)
        );
        h = static_cast<float>(luaL_checknumber(L, 3));
    }

    auto zNear = 0.01f;
    auto zFar = 100.0f;

    if (lua_gettop(L) >= 5)
    {
        zNear = static_cast<float>(luaL_checknumber(L, 4));
        zFar = static_cast<float>(luaL_checknumber(L, 5));
    }

    if (lua_gettop(L) >= 6)
    {
        flipHandedness = static_cast<int>(static_cast<bool>(lua_toboolean(L, 6)));
    }

    if (flipHandedness != 0)
    {
        D3DXMatrixOrthoRH(mat,
                          w,
                          h,
                          zNear,
                          zFar);
    }
    else
    {
        D3DXMatrixOrthoLH(mat,
                          w,
                          h,
                          zNear,
                          zFar);
    }

    lua_pushvalue(L, 1);
    return 1;
}


static auto matrix_orthoex(lua_State* L) -> int
{
    auto* const mat = static_cast<D3DXMATRIX*>(luaL_checkudata(L, 1, L_MATRIX));
    const auto res = RENDERER->GetSurfaceResolution();
    auto l = 0.0f;
    auto t = 0.0f;
    auto r = static_cast<float>(res.right);
    auto b = static_cast<float>(res.bottom);
    auto flipHandedness = FALSE;

    if (lua_gettop(L) >= 5)
    {
        l = static_cast<float>(luaL_checknumber(L, 2));
        r = static_cast<float>(luaL_checknumber(L, 3));
        b = static_cast<float>(luaL_checknumber(L, 4));
        t = static_cast<float>(luaL_checknumber(L, 5));
    }

    auto zNear = 0.01f;
    auto zFar = 100.0f;

    if (lua_gettop(L) >= 6)
    {
        zNear = static_cast<float>(luaL_checknumber(L, 6));
        zFar = static_cast<float>(luaL_checknumber(L, 7));
    }

    if (lua_gettop(L) >= 8)
    {
        flipHandedness = static_cast<int>(static_cast<bool>(lua_toboolean(L, 8)));
    }

    if (flipHandedness != 0)
    {
        D3DXMatrixOrthoOffCenterRH(mat,
                                   l, r, b, t,
                                   zNear,
                                   zFar);
    }
    else
    {
        D3DXMatrixOrthoOffCenterLH(mat,
                                   l, r, b, t,
                                   zNear,
                                   zFar);
    }

    lua_pushvalue(L, 1);
    return 1;
}

static auto matrix_getfield(lua_State* L) -> int
{
    auto* matPtr = static_cast<D3DXMATRIX*>(luaL_checkudata(L, 1, L_MATRIX));
    const auto row = static_cast<unsigned int>(luaL_checkinteger(L, 2)) - 1;
    const auto col = static_cast<unsigned int>(luaL_checkinteger(L, 3)) - 1;
    auto val = 0.0F;

    if (lua_gettop(L) >= 4)
    {
        val = static_cast<float>(luaL_checknumber(L, 4));
        matPtr->m[row][col] = val;
        matrix_new(L);
        auto* const newMat = static_cast<D3DXMATRIX*>(luaL_checkudata(L, 5, L_MATRIX));
        *newMat = *matPtr;
        return 1;
    }

    auto mat = *matPtr;
    lua_pushnumber(L, mat(row, col));
    return 1;
}

static auto matrix_getrow(lua_State* L) -> int
{
    auto* const matPtr = static_cast<D3DXMATRIX*>(luaL_checkudata(L, 1, L_MATRIX));
    const auto row = static_cast<unsigned int>(luaL_checkinteger(L, 2)) - 1;

    if (lua_gettop(L) >= 3)
    {
        auto* const vec = static_cast<D3DXVECTOR4*>(luaL_checkudata(L, 3, L_VECTOR));
        matrix_new(L);
        auto* const newMat = static_cast<D3DXMATRIX*>(luaL_checkudata(L, 4, L_MATRIX));
        *newMat = *matPtr;

        newMat[row][0] = vec->x;
        newMat[row][1] = vec->y;
        newMat[row][2] = vec->z;
        newMat[row][3] = vec->w;
        return 1;
    }

    auto mat = *matPtr;
    auto* const vec = static_cast<D3DXVECTOR4*>(lua_newuserdata(L, sizeof(D3DXVECTOR4)));
    luaL_setmetatable(L, L_VECTOR);
    *vec = D3DXVECTOR4(mat(row, 0), mat(row, 1), mat(row, 2), mat(row, 3));
    return 1;
}

static auto matrix_getcol(lua_State* L) -> int
{
    auto* const matPtr = static_cast<D3DXMATRIX*>(luaL_checkudata(L, 1, L_MATRIX));
    const auto col = static_cast<unsigned int>(luaL_checkinteger(L, 2)) - 1;

    if (lua_gettop(L) >= 3)
    {
        auto* const vec = static_cast<D3DXVECTOR4*>(luaL_checkudata(L, 3, L_VECTOR));
        matrix_new(L);
        auto* const newMat = static_cast<D3DXMATRIX*>(luaL_checkudata(L, 4, L_MATRIX));
        *newMat = *matPtr;

        newMat[0][col] = vec->x;
        newMat[0][col] = vec->y;
        newMat[0][col] = vec->z;
        newMat[0][col] = vec->w;
        return 1;
    }

    auto mat = *matPtr;
    auto* const vec = static_cast<D3DXVECTOR4*>(lua_newuserdata(L, sizeof(D3DXVECTOR4)));
    luaL_setmetatable(L, L_VECTOR);
    *vec = D3DXVECTOR4(mat(0, col), mat(1, col), mat(2, col), mat(3, col));
    return 1;
}

static auto matrix_inverse(lua_State* L) -> int
{
    auto* const mat = static_cast<D3DXMATRIX*>(luaL_checkudata(L, 1, L_MATRIX));
    D3DXMATRIX t;
    float d;

    auto* ok = D3DXMatrixInverse(&t, &d, mat);

    if (ok == nullptr)
    {
        lua_pushnil(L);
    }
    else
    {
        lua_newtable(L);

        lua_pushinteger(L, 1);
        lua_pushvalue(L, 1);
        lua_settable(L, 2);

        lua_pushinteger(L, 2);
        lua_pushnumber(L, d);
        lua_settable(L, 2);

        lua_pushvalue(L, 2);
    }

    return 1;
}

static void LuaMatrix$Register(lua_State* L)
{
    lua_register(L, L_MATRIX, matrix_new);
    luaL_newmetatable(L, L_MATRIX);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");

    REGC("translate", matrix_translate);
    REGC("rotate", matrix_rotate);
    REGC("scale", matrix_scale);
    REGC("bind", matrix_bind);
    REGC("lookAt", matrix_lookat);
    REGC("persp", matrix_persp);
    REGC("ortho", matrix_ortho);
    REGC("orthoEx", matrix_orthoex);
    REGC("__mul", matrix_mul);
    REGC("m", matrix_getfield);
    REGC("inverse", matrix_inverse);
    REGC("reflect", matrix_reflect);
    REGC("shadow", matrix_shadow);
    REGC("row", matrix_getrow);
    REGC("col", matrix_getcol);

    lua_pop(L, 1);
}
