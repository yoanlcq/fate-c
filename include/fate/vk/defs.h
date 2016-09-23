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

#ifndef FE_VK_DEFS_H
#define FE_VK_DEFS_H

/*! \file fate/vk.h
 *  \brief Definitions and inclusions for the Vulkan backend.
 *  \ingroup vk
 *
 * TODO
 *
 * @{
 */

#ifdef FE_USE_VULKAN

#include <fate/defs.h>

#if defined(FE_TARGET_ANDROID)
    #define VK_USE_PLATFORM_ANDROID_KHR 1
#elif defined(FE_TARGET_LINUX) || defined(FE_TARGET_FREEBSD)
    #define VK_USE_PLATFORM_MIR_KHR 1
    #define VK_USE_PLATFORM_WAYLAND_KHR 1
    #define VK_USE_PLATFORM_XLIB_KHR 1
    #define VK_USE_PLATFORM_XCB_KHR 1
#elif defined(FE_TARGET_WINDOWS)
    #define VK_USE_PLATFORM_WIN32_KHR 1
#endif /**/

#include <vulkan/vulkan.h>

#endif /* FE_USE_VULKAN */

/*! @} */

#endif /* FE_VK_DEFS_H */
