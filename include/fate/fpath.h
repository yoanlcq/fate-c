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

#ifndef FE_FPATH_H
#define FE_FPATH_H

/*! \file fate/fpath.h
 *  \brief File path utils.
 *  \defgroup fpath File path utils.
 *
 * TODO
 *
 * @{
 */


#ifdef FE_TARGET_EMSCRIPTEN
typedef struct {
    union {
        char *path;
        struct {
            char *url;
        } wget;
        struct {
            char *path;
        } memfs;
        struct {
            char *key;
            char *object_store_name;
            char *db_name;
        } idb;
    };
    enum {
        FE_FPATH_EMSCRIPTEN_MEMFS=0,
        FE_FPATH_EMSCRIPTEN_IDB, 
        FE_FPATH_EMSCRIPTEN_WGET, 
    } type;
} fe_fpath;
#else
/*! \brief Typedef-ed into a struct to prevent accidental
 *         use of raw strings in fe_fs_*() routines. */
typedef struct {
    char *path;
} fe_fpath; 
#endif

/*! \bried TODO */
void fe_fpath_deinit(fe_fpath fpath);

/* These routines are available for building paths. Free them using #fe_path_deinit(). */
#if defined(FE_TARGET_EMSCRIPTEN)
fe_fpath fe_fpath_emscripten_idb(const char *db_name, const char *object_store_name, const char *key);
fe_fpath fe_fpath_emscripten_wget(const char *url);
fe_fpath fe_fpath_emscripten_memfs(const char *filepath);
#elif FE_TARGET_OSX
fe_fpath fe_fpath_osx_sdl2basepath(const char *filepath);
fe_fpath fe_fpath_osx_sdl2prefpath(const char *org, const char *app, const char *filepath);
fe_fpath fe_fpath_osx_tmp(const char *filepath);
#elif defined(FE_TARGET_IOS)
fe_fpath fe_fpath_ios_sdl2basepath(const char *filepath);
fe_fpath fe_fpath_ios_sdl2prefpath(const char *org, const char *app, const char *filepath);
fe_fpath fe_fpath_ios_tmp(const char *tmpdir, const char *filepath); //tmpdir = specifié par app.
#elif defined(FE_TARGET_ANDROID)
fe_fpath fe_fpath_android_external_storage(const char *filepath);
fe_fpath fe_fpath_android_internal_storage(const char *not_assetdir, const char *filepath);
fe_fpath fe_fpath_android_apk_asset(const char *assetdir, const char *filepath);
fe_fpath fe_fpath_android_tmp(const char *tmpdir, const char *filepath); //tmpdir = spécifié par app.
#elif defined(FE_TARGET_LINUX) || defined(FE_TARGET_FREEBSD)
fe_fpath fe_fpath_linux_executable_dir(const char *filepath);
fe_fpath fe_fpath_linux_xdg_data_home(const char *filepath);
fe_fpath fe_fpath_linux_tmp_dir(const char *filepath);
#elif defined(FE_TARGET_WINDOWS_RT)
fe_fpath fe_fpath_winrt_installed_location(const char *filepath);
fe_fpath fe_fpath_winrt_local_folder(const char *filepath);
fe_fpath fe_fpath_winrt_roaming_folder(const char *filepath);
fe_fpath fe_fpath_winrt_temp_folder(const char *filepath);
#elif defined(FE_TARGET_WINDOWS)
fe_fpath fe_fpath_windows_executable_folder(const char *filepath);
fe_fpath fe_fpath_windows_appdata(const char *filepath);
fe_fpath fe_fpath_windows_temp_folder(const char *filepath);
#endif

/*! @} */

#endif /* FE_PATH_H */
