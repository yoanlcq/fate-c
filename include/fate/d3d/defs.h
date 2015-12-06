#ifndef FATE_D3D_DEFS_H
#define FATE_D3D_DEFS_H

#define INITGUID
#include <windows.h>
#include <d3d10.h>
#define  DXCALL(_OBJ_, _METHOD_) \
        _OBJ_->lpVtbl->_METHOD_(_OBJ_)
#define DXCALLV(_OBJ_, _METHOD_, ...) \
        _OBJ_->lpVtbl->_METHOD_(_OBJ_, __VA_ARGS__)

#endif /* FATE_D3D_DEFS_H */
