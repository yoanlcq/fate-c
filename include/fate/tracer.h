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

/*! \file fate/tracer.h
 *  \brief Support for logging function calls and their parameters' value.
 *  \defgroup tracer Tracer : Logging function calls and parameters at runtime.
 *
 * TODO
 *
 * @{
 */


#ifndef FE_TRACER_H
#define FE_TRACER_H

#include <fate/defs.h>

#ifndef FE_ENABLE_TRACING

    #define  FE_TRACE_0(T,X) \
                          T=X()
    #define  FE_TRACE_1(T,X,A) \
                          T=X(A)
    #define  FE_TRACE_2(T,X,A,B) \
                          T=X(A,B)
    #define  FE_TRACE_3(T,X,A,B,C) \
                          T=X(A,B,C)
    #define  FE_TRACE_4(T,X,A,B,C,D) \
                          T=X(A,B,C,D)
    #define  FE_TRACE_5(T,X,A,B,C,D,E) \
                          T=X(A,B,C,D,E)
    #define  FE_TRACE_6(T,X,A,B,C,D,E,F) \
                          T=X(A,B,C,D,E,F)
    #define  FE_TRACE_7(T,X,A,B,C,D,E,F,G) \
                          T=X(A,B,C,D,E,F,G)
    #define  FE_TRACE_8(T,X,A,B,C,D,E,F,G,H) \
                          T=X(A,B,C,D,E,F,G,H)
    #define  FE_TRACE_9(T,X,A,B,C,D,E,F,G,H,I) \
                          T=X(A,B,C,D,E,F,G,H,I)
    #define FE_TRACE_10(T,X,A,B,C,D,E,F,G,H,I,J) \
                          T=X(A,B,C,D,E,F,G,H,I,J)
    #define FE_TRACE_11(T,X,A,B,C,D,E,F,G,H,I,J,K) \
                          T=X(A,B,C,D,E,F,G,H,I,J,K)
    #define FE_TRACE_12(T,X,A,B,C,D,E,F,G,H,I,J,K,L) \
                          T=X(A,B,C,D,E,F,G,H,I,J,K,L)
    #define FE_TRACE_13(T,X,A,B,C,D,E,F,G,H,I,J,K,L,M) \
                          T=X(A,B,C,D,E,F,G,H,I,J,K,L,M)
    #define FE_TRACE_14(T,X,A,B,C,D,E,F,G,H,I,J,K,L,M,N) \
                          T=X(A,B,C,D,E,F,G,H,I,J,K,L,M,N)
    #define FE_TRACE_15(T,X,A,B,C,D,E,F,G,H,I,J,K,L,M,N,O) \
                          T=X(A,B,C,D,E,F,G,H,I,J,K,L,M,N,O)
    #define FE_TRACE_16(T,X,A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P) \
                          T=X(A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P)
    #define FE_TRACE_17(T,X,A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q) \
                          T=X(A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q)
    #define FE_TRACE_18(T,X,A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R) \
                          T=X(A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R)
    #define FE_TRACE_19(T,X,A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S) \
                          T=X(A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S)
    #define FE_TRACE_20(T,X,A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,U) \
                          T=X(A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,U)
    #define FE_TRACE_21(T,X,A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,U,V) \
                          T=X(A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,U,V)
    #define FE_TRACE_22(T,X,A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,U,V,W) \
                          T=X(A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,U,V,W)
    #define FE_TRACE_23(T,X,A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,U,V,W,Y) \
                          T=X(A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,U,V,W,Y)
    #define FE_TRACE_24(T,X,A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,U,V,W,Y,Z) \
                          T=X(A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,U,V,W,Y,Z)

#else /* FE_ENABLE_TRACING defined */

    #if __STDC_VERSION__ < 199901L
        #if defined(_MSC_VER) && !__INTEL_COMPILER
            #pragma message("Trace : __func__ is only available from C99.")
        #else
            #warning Trace : __func__ is only available from C99.
        #endif
    #endif

    #if __STDC_VERSION__ < 201112L
        #if defined(_MSC_VER) && !__INTEL_COMPILER
            #pragma message("Tracing needs _Generic, which is only available with C11. Parameters given to functions will NOT be displayed.")
        #else
            #warning Tracing needs _Generic, which is only available with C11. \
                Parameters given to functions will NOT be displayed.
        #endif
        #define FE_TRACE_GENERIC(X) fe_logf_trace("??")
    #else
/*
 * This macro lists all types in a paranoid way. As of today, it is sadly
 * necessary because of various reasons, including compiler quirks.
 */
        #define FE_TRACE_GENERIC(X) \
            _Generic((X), \
            signed char:              fe_trace_sc, \
            const signed char:        fe_trace_sc, \
            unsigned char:            fe_trace_sc, \
            const unsigned char:      fe_trace_sc, \
            \
            signed short:             fe_trace_ss, \
            const signed short:       fe_trace_ss, \
            unsigned short:           fe_trace_us, \
            const unsigned short:     fe_trace_us, \
            \
            signed int:               fe_trace_s, \
            const signed int:         fe_trace_s, \
            unsigned int:             fe_trace_u, \
            const unsigned int:       fe_trace_u, \
            \
            signed long:              fe_trace_sl, \
            const signed long:        fe_trace_sl, \
            unsigned long:            fe_trace_ul, \
            const unsigned long:      fe_trace_ul, \
            \
            signed long long:         fe_trace_sll, \
            const signed long long:   fe_trace_sll, \
            unsigned long long:       fe_trace_ull, \
            const unsigned long long: fe_trace_ull, \
            \
            float:                    fe_trace_f, \
            const float:              fe_trace_f, \
            double:                   fe_trace_d, \
            const double:             fe_trace_d, \
            long double:              fe_trace_ld, \
            const long double:        fe_trace_ld, \
            \
            const char*:              fe_trace_str, \
            char*:                    fe_trace_str, \
            const signed char*:       fe_trace_str, \
            signed char*:             fe_trace_str, \
            const unsigned char*:     fe_trace_str, \
            unsigned char*:           fe_trace_str, \
            \
            void*:                    fe_trace_voidp, \
            default:                  fe_trace_default \
            )(X)


void fe_trace_chr(const int c);
void fe_trace_sc(const signed char c);
void fe_trace_str(const signed char *str);
void fe_trace_ss(const signed short c);
void fe_trace_us(const unsigned short c);
void fe_trace_s(const signed int c);
void fe_trace_u(const unsigned int c);
void fe_trace_sl(const signed long c);
void fe_trace_ul(const unsigned long c);
void fe_trace_sll(const signed long long c);
void fe_trace_ull(const unsigned long long c);
void fe_trace_f(const float c);
void fe_trace_d(const double c);
void fe_trace_ld(const long double c);
void fe_trace_default(const void *p);
void fe_trace_voidp(const void *p);

    #endif /* _Generic support */

/* So that those which include tracer also include fe_logf_trace(). */
#include <fate/log.h> 

extern unsigned fe_tracer_level; 

    #define FE_TRACE_0(T,X) \
        ++fe_tracer_level; \
        fe_logf_trace( \
            "~> [%3u] %s:%5u: %s ==> %s(", \
            fe_tracer_level, __FILE__, __LINE__, __func__, XSTRINGIFY(X)); \
        fe_logf_trace(")\n"); \
        T = X(); \
        fe_logf_trace( \
            "~> [%3u] %s:%5u: %s <== ", \
            fe_tracer_level, __FILE__, __LINE__, __func__); \
        FE_TRACE_GENERIC(T); \
        fe_logf_trace("\n"); \
        --fe_tracer_level

    #define FE_TRACE_1(T,X,A) \
        ++fe_tracer_level; \
        fe_logf_trace( \
            "~> [%3u] %s:%5u: %s ==> %s(", \
            fe_tracer_level, __FILE__, __LINE__, __func__, XSTRINGIFY(X)); \
        FE_TRACE_GENERIC(A); \
        fe_logf_trace(")\n"); \
        T = X(A); \
        fe_logf_trace( \
            "~> [%3u] %s:%5u: %s <== ", \
            fe_tracer_level, __FILE__, __LINE__, __func__); \
        FE_TRACE_GENERIC(T); \
        fe_logf_trace("\n"); \
        --fe_tracer_level

    #define FE_TRACE_2(T,X,A,B) \
        ++fe_tracer_level; \
        fe_logf_trace( \
            "~> [%3u] %s:%5u: %s ==> %s(", \
            fe_tracer_level, __FILE__, __LINE__, __func__, XSTRINGIFY(X)); \
        FE_TRACE_GENERIC(A); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(B); \
        fe_logf_trace(")\n"); \
        T = X(A,B); \
        fe_logf_trace( \
            "~> [%3u] %s:%5u: %s <== ", \
            fe_tracer_level, __FILE__, __LINE__, __func__); \
        FE_TRACE_GENERIC(T); \
        fe_logf_trace("\n"); \
        --fe_tracer_level

    #define FE_TRACE_3(T,X,A,B,C) \
        ++fe_tracer_level; \
        fe_logf_trace( \
            "~> [%3u] %s:%5u: %s ==> %s(", \
            fe_tracer_level, __FILE__, __LINE__, __func__, XSTRINGIFY(X)); \
        FE_TRACE_GENERIC(A); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(B); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(C); \
        fe_logf_trace(")\n"); \
        T = X(A,B,C); \
        fe_logf_trace( \
            "~> [%3u] %s:%5u: %s <== ", \
            fe_tracer_level, __FILE__, __LINE__, __func__); \
        FE_TRACE_GENERIC(T); \
        fe_logf_trace("\n"); \
        --fe_tracer_level

    #define FE_TRACE_4(T,X,A,B,C,D) \
        ++fe_tracer_level; \
        fe_logf_trace( \
            "~> [%3u] %s:%5u: %s ==> %s(", \
            fe_tracer_level, __FILE__, __LINE__, __func__, XSTRINGIFY(X)); \
        FE_TRACE_GENERIC(A); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(B); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(C); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(D); \
        fe_logf_trace(")\n"); \
        T = X(A,B,C,D); \
        fe_logf_trace( \
            "~> [%3u] %s:%5u: %s <== ", \
            fe_tracer_level, __FILE__, __LINE__, __func__); \
        FE_TRACE_GENERIC(T); \
        fe_logf_trace("\n"); \
        --fe_tracer_level

    #define FE_TRACE_5(T,X,A,B,C,D,E) \
        ++fe_tracer_level; \
        fe_logf_trace( \
            "~> [%3u] %s:%5u: %s ==> %s(", \
            fe_tracer_level, __FILE__, __LINE__, __func__, XSTRINGIFY(X)); \
        FE_TRACE_GENERIC(A); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(B); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(C); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(D); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(E); \
        fe_logf_trace(")\n"); \
        T = X(A,B,C,D,E); \
        fe_logf_trace( \
            "~> [%3u] %s:%5u: %s <== ", \
            fe_tracer_level, __FILE__, __LINE__, __func__); \
        FE_TRACE_GENERIC(T); \
        fe_logf_trace("\n"); \
        --fe_tracer_level

    #define FE_TRACE_6(T,X,A,B,C,D,E,F) \
        ++fe_tracer_level; \
        fe_logf_trace( \
            "~> [%3u] %s:%5u: %s ==> %s(", \
            fe_tracer_level, __FILE__, __LINE__, __func__, XSTRINGIFY(X)); \
        FE_TRACE_GENERIC(A); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(B); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(C); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(D); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(E); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(F); \
        fe_logf_trace(")\n"); \
        T = X(A,B,C,D,E,F); \
        fe_logf_trace( \
            "~> [%3u] %s:%5u: %s <== ", \
            fe_tracer_level, __FILE__, __LINE__, __func__); \
        FE_TRACE_GENERIC(T); \
        fe_logf_trace("\n"); \
        --fe_tracer_level

    #define FE_TRACE_7(T,X,A,B,C,D,E,F,G) \
        ++fe_tracer_level; \
        fe_logf_trace( \
            "~> [%3u] %s:%5u: %s ==> %s(", \
            fe_tracer_level, __FILE__, __LINE__, __func__, XSTRINGIFY(X)); \
        FE_TRACE_GENERIC(A); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(B); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(C); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(D); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(E); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(F); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(G); \
        fe_logf_trace(")\n"); \
        T = X(A,B,C,D,E,F,G); \
        fe_logf_trace( \
            "~> [%3u] %s:%5u: %s <== ", \
            fe_tracer_level, __FILE__, __LINE__, __func__); \
        FE_TRACE_GENERIC(T); \
        fe_logf_trace("\n"); \
        --fe_tracer_level

    #define FE_TRACE_8(T,X,A,B,C,D,E,F,G,H) \
        ++fe_tracer_level; \
        fe_logf_trace( \
            "~> [%3u] %s:%5u: %s ==> %s(", \
            fe_tracer_level, __FILE__, __LINE__, __func__, XSTRINGIFY(X)); \
        FE_TRACE_GENERIC(A); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(B); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(C); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(D); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(E); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(F); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(G); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(H); \
        fe_logf_trace(")\n"); \
        T = X(A,B,C,D,E,F,G,H); \
        fe_logf_trace( \
            "~> [%3u] %s:%5u: %s <== ", \
            fe_tracer_level, __FILE__, __LINE__, __func__); \
        FE_TRACE_GENERIC(T); \
        fe_logf_trace("\n"); \
        --fe_tracer_level

    #define FE_TRACE_9(T,X,A,B,C,D,E,F,G,H,I) \
        ++fe_tracer_level; \
        fe_logf_trace( \
            "~> [%3u] %s:%5u: %s ==> %s(", \
            fe_tracer_level, __FILE__, __LINE__, __func__, XSTRINGIFY(X)); \
        FE_TRACE_GENERIC(A); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(B); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(C); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(D); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(E); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(F); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(G); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(H); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(I); \
        fe_logf_trace(")\n"); \
        T = X(A,B,C,D,E,F,G,H,I); \
        fe_logf_trace( \
            "~> [%3u] %s:%5u: %s <== ", \
            fe_tracer_level, __FILE__, __LINE__, __func__); \
        FE_TRACE_GENERIC(T); \
        fe_logf_trace("\n"); \
        --fe_tracer_level

    #define FE_TRACE_10(T,X,A,B,C,D,E,F,G,H,I,J) \
        ++fe_tracer_level; \
        fe_logf_trace( \
            "~> [%3u] %s:%5u: %s ==> %s(", \
            fe_tracer_level, __FILE__, __LINE__, __func__, XSTRINGIFY(X)); \
        FE_TRACE_GENERIC(A); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(B); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(C); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(D); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(E); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(F); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(G); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(H); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(I); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(J); \
        fe_logf_trace(")\n"); \
        T = X(A,B,C,D,E,F,G,H,I,J); \
        fe_logf_trace( \
            "~> [%3u] %s:%5u: %s <== ", \
            fe_tracer_level, __FILE__, __LINE__, __func__); \
        FE_TRACE_GENERIC(T); \
        fe_logf_trace("\n"); \
        --fe_tracer_level

    #define FE_TRACE_11(T,X,A,B,C,D,E,F,G,H,I,J,K) \
        ++fe_tracer_level; \
        fe_logf_trace( \
            "~> [%3u] %s:%5u: %s ==> %s(", \
            fe_tracer_level, __FILE__, __LINE__, __func__, XSTRINGIFY(X)); \
        FE_TRACE_GENERIC(A); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(B); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(C); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(D); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(E); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(F); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(G); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(H); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(I); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(J); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(K); \
        fe_logf_trace(")\n"); \
        T = X(A,B,C,D,E,F,G,H,I,J,K); \
        fe_logf_trace( \
            "~> [%3u] %s:%5u: %s <== ", \
            fe_tracer_level, __FILE__, __LINE__, __func__); \
        FE_TRACE_GENERIC(T); \
        fe_logf_trace("\n"); \
        --fe_tracer_level

    #define FE_TRACE_12(T,X,A,B,C,D,E,F,G,H,I,J,K,L) \
        ++fe_tracer_level; \
        fe_logf_trace( \
            "~> [%3u] %s:%5u: %s ==> %s(", \
            fe_tracer_level, __FILE__, __LINE__, __func__, XSTRINGIFY(X)); \
        FE_TRACE_GENERIC(A); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(B); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(C); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(D); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(E); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(F); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(G); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(H); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(I); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(J); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(K); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(L); \
        fe_logf_trace(")\n"); \
        T = X(A,B,C,D,E,F,G,H,I,J,K,L); \
        fe_logf_trace( \
            "~> [%3u] %s:%5u: %s <== ", \
            fe_tracer_level, __FILE__, __LINE__, __func__); \
        FE_TRACE_GENERIC(T); \
        fe_logf_trace("\n"); \
        --fe_tracer_level

    #define FE_TRACE_13(T,X,A,B,C,D,E,F,G,H,I,J,K,L,M) \
        ++fe_tracer_level; \
        fe_logf_trace( \
            "~> [%3u] %s:%5u: %s ==> %s(", \
            fe_tracer_level, __FILE__, __LINE__, __func__, XSTRINGIFY(X)); \
        FE_TRACE_GENERIC(A); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(B); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(C); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(D); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(E); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(F); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(G); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(H); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(I); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(J); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(K); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(L); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(M); \
        fe_logf_trace(")\n"); \
        T = X(A,B,C,D,E,F,G,H,I,J,K,L,M); \
        fe_logf_trace( \
            "~> [%3u] %s:%5u: %s <== ", \
            fe_tracer_level, __FILE__, __LINE__, __func__); \
        FE_TRACE_GENERIC(T); \
        fe_logf_trace("\n"); \
        --fe_tracer_level

    #define FE_TRACE_14(T,X,A,B,C,D,E,F,G,H,I,J,K,L,M,N) \
        ++fe_tracer_level; \
        fe_logf_trace( \
            "~> [%3u] %s:%5u: %s ==> %s(", \
            fe_tracer_level, __FILE__, __LINE__, __func__, XSTRINGIFY(X)); \
        FE_TRACE_GENERIC(A); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(B); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(C); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(D); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(E); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(F); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(G); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(H); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(I); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(J); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(K); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(L); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(M); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(N); \
        fe_logf_trace(")\n"); \
        T = X(A,B,C,D,E,F,G,H,I,J,K,L,M,N); \
        fe_logf_trace( \
            "~> [%3u] %s:%5u: %s <== ", \
            fe_tracer_level, __FILE__, __LINE__, __func__); \
        FE_TRACE_GENERIC(T); \
        fe_logf_trace("\n"); \
        --fe_tracer_level

    #define FE_TRACE_15(T,X,A,B,C,D,E,F,G,H,I,J,K,L,M,N,O) \
        ++fe_tracer_level; \
        fe_logf_trace( \
            "~> [%3u] %s:%5u: %s ==> %s(", \
            fe_tracer_level, __FILE__, __LINE__, __func__, XSTRINGIFY(X)); \
        FE_TRACE_GENERIC(A); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(B); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(C); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(D); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(E); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(F); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(G); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(H); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(I); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(J); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(K); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(L); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(M); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(N); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(O); \
        fe_logf_trace(")\n"); \
        T = X(A,B,C,D,E,F,G,H,I,J,K,L,M,N,O); \
        fe_logf_trace( \
            "~> [%3u] %s:%5u: %s <== ", \
            fe_tracer_level, __FILE__, __LINE__, __func__); \
        FE_TRACE_GENERIC(T); \
        fe_logf_trace("\n"); \
        --fe_tracer_level

    #define FE_TRACE_16(T,X,A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P) \
        ++fe_tracer_level; \
        fe_logf_trace( \
            "~> [%3u] %s:%5u: %s ==> %s(", \
            fe_tracer_level, __FILE__, __LINE__, __func__, XSTRINGIFY(X)); \
        FE_TRACE_GENERIC(A); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(B); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(C); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(D); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(E); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(F); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(G); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(H); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(I); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(J); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(K); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(L); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(M); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(N); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(O); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(P); \
        fe_logf_trace(")\n"); \
        T = X(A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P); \
        fe_logf_trace( \
            "~> [%3u] %s:%5u: %s <== ", \
            fe_tracer_level, __FILE__, __LINE__, __func__); \
        FE_TRACE_GENERIC(T); \
        fe_logf_trace("\n"); \
        --fe_tracer_level

    #define FE_TRACE_17(T,X,A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q) \
        ++fe_tracer_level; \
        fe_logf_trace( \
            "~> [%3u] %s:%5u: %s ==> %s(", \
            fe_tracer_level, __FILE__, __LINE__, __func__, XSTRINGIFY(X)); \
        FE_TRACE_GENERIC(A); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(B); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(C); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(D); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(E); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(F); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(G); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(H); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(I); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(J); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(K); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(L); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(M); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(N); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(O); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(P); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(Q); \
        fe_logf_trace(")\n"); \
        T = X(A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q); \
        fe_logf_trace( \
            "~> [%3u] %s:%5u: %s <== ", \
            fe_tracer_level, __FILE__, __LINE__, __func__); \
        FE_TRACE_GENERIC(T); \
        fe_logf_trace("\n"); \
        --fe_tracer_level

    #define FE_TRACE_18(T,X,A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R) \
        ++fe_tracer_level; \
        fe_logf_trace( \
            "~> [%3u] %s:%5u: %s ==> %s(", \
            fe_tracer_level, __FILE__, __LINE__, __func__, XSTRINGIFY(X)); \
        FE_TRACE_GENERIC(A); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(B); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(C); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(D); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(E); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(F); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(G); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(H); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(I); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(J); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(K); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(L); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(M); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(N); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(O); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(P); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(Q); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(R); \
        fe_logf_trace(")\n"); \
        T = X(A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R); \
        fe_logf_trace( \
            "~> [%3u] %s:%5u: %s <== ", \
            fe_tracer_level, __FILE__, __LINE__, __func__); \
        FE_TRACE_GENERIC(T); \
        fe_logf_trace("\n"); \
        --fe_tracer_level

    #define FE_TRACE_19(T,X,A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S) \
        ++fe_tracer_level; \
        fe_logf_trace( \
            "~> [%3u] %s:%5u: %s ==> %s(", \
            fe_tracer_level, __FILE__, __LINE__, __func__, XSTRINGIFY(X)); \
        FE_TRACE_GENERIC(A); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(B); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(C); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(D); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(E); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(F); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(G); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(H); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(I); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(J); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(K); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(L); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(M); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(N); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(O); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(P); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(Q); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(R); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(S); \
        fe_logf_trace(")\n"); \
        T = X(A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S); \
        fe_logf_trace( \
            "~> [%3u] %s:%5u: %s <== ", \
            fe_tracer_level, __FILE__, __LINE__, __func__); \
        FE_TRACE_GENERIC(T); \
        fe_logf_trace("\n"); \
        --fe_tracer_level

    #define FE_TRACE_20(T,X,A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,U) \
        ++fe_tracer_level; \
        fe_logf_trace( \
            "~> [%3u] %s:%5u: %s ==> %s(", \
            fe_tracer_level, __FILE__, __LINE__, __func__, XSTRINGIFY(X)); \
        FE_TRACE_GENERIC(A); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(B); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(C); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(D); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(E); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(F); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(G); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(H); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(I); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(J); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(K); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(L); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(M); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(N); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(O); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(P); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(Q); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(R); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(S); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(U); \
        fe_logf_trace(")\n"); \
        T = X(A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,U); \
        fe_logf_trace( \
            "~> [%3u] %s:%5u: %s <== ", \
            fe_tracer_level, __FILE__, __LINE__, __func__); \
        FE_TRACE_GENERIC(T); \
        fe_logf_trace("\n"); \
        --fe_tracer_level

    #define FE_TRACE_21(T,X,A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,U,V) \
        ++fe_tracer_level; \
        fe_logf_trace( \
            "~> [%3u] %s:%5u: %s ==> %s(", \
            fe_tracer_level, __FILE__, __LINE__, __func__, XSTRINGIFY(X)); \
        FE_TRACE_GENERIC(A); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(B); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(C); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(D); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(E); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(F); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(G); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(H); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(I); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(J); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(K); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(L); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(M); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(N); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(O); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(P); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(Q); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(R); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(S); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(U); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(V); \
        fe_logf_trace(")\n"); \
        T = X(A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,U,V); \
        fe_logf_trace( \
            "~> [%3u] %s:%5u: %s <== ", \
            fe_tracer_level, __FILE__, __LINE__, __func__); \
        FE_TRACE_GENERIC(T); \
        fe_logf_trace("\n"); \
        --fe_tracer_level

    #define FE_TRACE_22(T,X,A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,U,V,W) \
        ++fe_tracer_level; \
        fe_logf_trace( \
            "~> [%3u] %s:%5u: %s ==> %s(", \
            fe_tracer_level, __FILE__, __LINE__, __func__, XSTRINGIFY(X)); \
        FE_TRACE_GENERIC(A); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(B); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(C); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(D); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(E); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(F); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(G); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(H); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(I); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(J); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(K); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(L); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(M); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(N); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(O); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(P); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(Q); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(R); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(S); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(U); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(V); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(W); \
        fe_logf_trace(")\n"); \
        T = X(A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,U,V,W); \
        fe_logf_trace( \
            "~> [%3u] %s:%5u: %s <== ", \
            fe_tracer_level, __FILE__, __LINE__, __func__); \
        FE_TRACE_GENERIC(T); \
        fe_logf_trace("\n"); \
        --fe_tracer_level

    #define FE_TRACE_23(T,X,A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,U,V,W,Y) \
        ++fe_tracer_level; \
        fe_logf_trace( \
            "~> [%3u] %s:%5u: %s ==> %s(", \
            fe_tracer_level, __FILE__, __LINE__, __func__, XSTRINGIFY(X)); \
        FE_TRACE_GENERIC(A); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(B); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(C); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(D); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(E); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(F); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(G); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(H); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(I); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(J); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(K); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(L); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(M); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(N); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(O); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(P); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(Q); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(R); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(S); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(U); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(V); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(W); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(Y); \
        fe_logf_trace(")\n"); \
        T = X(A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,U,V,W,Y); \
        fe_logf_trace( \
            "~> [%3u] %s:%5u: %s <== ", \
            fe_tracer_level, __FILE__, __LINE__, __func__); \
        FE_TRACE_GENERIC(T); \
        fe_logf_trace("\n"); \
        --fe_tracer_level

    #define FE_TRACE_24(T,X,A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,U,V,W,Y,Z) \
        ++fe_tracer_level; \
        fe_logf_trace( \
            "~> [%3u] %s:%5u: %s ==> %s(", \
            fe_tracer_level, __FILE__, __LINE__, __func__, XSTRINGIFY(X)); \
        FE_TRACE_GENERIC(A); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(B); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(C); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(D); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(E); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(F); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(G); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(H); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(I); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(J); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(K); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(L); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(M); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(N); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(O); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(P); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(Q); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(R); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(S); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(U); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(V); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(W); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(Y); \
        fe_logf_trace(", "); \
        FE_TRACE_GENERIC(Z); \
        fe_logf_trace(")\n"); \
        T = X(A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,U,V,W,Y,Z); \
        fe_logf_trace( \
            "~> [%3u] %s:%5u: %s <== ", \
            fe_tracer_level, __FILE__, __LINE__, __func__); \
        FE_TRACE_GENERIC(T); \
        fe_logf_trace("\n"); \
        --fe_tracer_level


#endif /* FE_ENABLE_TRACING */

/*! @} */
#endif /* FE_TRACER_H */
