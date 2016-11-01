/* 
 * This file is part of F.A.T.E - the Fight And Travel game Engine.
 * Copyright (c) 2015-2016 Yoan Lecoq (yoanlecoq.io at gmail.com)
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from the
 * use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 
 * 1. The origin of this software must not be misrepresented;
 *    you must not claim that you wrote the original software. 
 *    If you use this software in a product, an acknowledgment in the product
 *    documentation would be appreciated but is not required.
 * 
 * 2. Altered source versions must be plainly marked as such, 
 *    and must not be misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 *
 *
 * Contributors :
 * - Please make sure your indentation is set to 4 spaces;
 * - Please avoid exceeding 80 character columns.
 *
 */

/*! \file fate/d3d/defs.h
 *  \brief Common definitions for the Direct3D backend.
 *
 *  TODO
 */

#ifndef FE_D3D_DEFS_H
#define FE_D3D_DEFS_H

#include <fate/defs.h>
#ifdef FE_TARGET_WINDOWS
/*
 * This would only be necessary if we didn't link with 'dxguid.lib'.
#ifndef INITGUID
#define INITGUID
#endif
*/
#include <fate/stdsys.h>
//#ifndef FE_CC_MSVC 
/* XXX This is a bad hack for stuff to compile on CI. 
 * This is some something I absolutely did not feel like 
 * dealing with when it happened. */
#define COBJMACROS
#include <d3d10.h>
//#endif
#define  DXCALL(_OBJ_, _METHOD_) \
        _OBJ_->lpVtbl->_METHOD_(_OBJ_)
#define DXCALLV(_OBJ_, _METHOD_, ...) \
        _OBJ_->lpVtbl->_METHOD_(_OBJ_, __VA_ARGS__)

#endif /* FE_TARGET_WINDOWS */

#endif /* FE_D3D_DEFS_H */
