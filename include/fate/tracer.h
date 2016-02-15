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
 */


#ifndef FATE_TRACER_H
#define FATE_TRACER_H

#include <fate/defs.h>

#ifndef FATE_ENABLE_TRACING

    #define  FATE_TRACE_0(T,X) \
                          T=X()
    #define  FATE_TRACE_1(T,X,A) \
                          T=X(A)
    #define  FATE_TRACE_2(T,X,A,B) \
                          T=X(A,B)
    #define  FATE_TRACE_3(T,X,A,B,C) \
                          T=X(A,B,C)
    #define  FATE_TRACE_4(T,X,A,B,C,D) \
                          T=X(A,B,C,D)
    #define  FATE_TRACE_5(T,X,A,B,C,D,E) \
                          T=X(A,B,C,D,E)
    #define  FATE_TRACE_6(T,X,A,B,C,D,E,F) \
                          T=X(A,B,C,D,E,F)
    #define  FATE_TRACE_7(T,X,A,B,C,D,E,F,G) \
                          T=X(A,B,C,D,E,F,G)
    #define  FATE_TRACE_8(T,X,A,B,C,D,E,F,G,H) \
                          T=X(A,B,C,D,E,F,G,H)
    #define  FATE_TRACE_9(T,X,A,B,C,D,E,F,G,H,I) \
                          T=X(A,B,C,D,E,F,G,H,I)
    #define FATE_TRACE_10(T,X,A,B,C,D,E,F,G,H,I,J) \
                          T=X(A,B,C,D,E,F,G,H,I,J)
    #define FATE_TRACE_11(T,X,A,B,C,D,E,F,G,H,I,J,K) \
                          T=X(A,B,C,D,E,F,G,H,I,J,K)
    #define FATE_TRACE_12(T,X,A,B,C,D,E,F,G,H,I,J,K,L) \
                          T=X(A,B,C,D,E,F,G,H,I,J,K,L)
    #define FATE_TRACE_13(T,X,A,B,C,D,E,F,G,H,I,J,K,L,M) \
                          T=X(A,B,C,D,E,F,G,H,I,J,K,L,M)
    #define FATE_TRACE_14(T,X,A,B,C,D,E,F,G,H,I,J,K,L,M,N) \
                          T=X(A,B,C,D,E,F,G,H,I,J,K,L,M,N)
    #define FATE_TRACE_15(T,X,A,B,C,D,E,F,G,H,I,J,K,L,M,N,O) \
                          T=X(A,B,C,D,E,F,G,H,I,J,K,L,M,N,O)
    #define FATE_TRACE_16(T,X,A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P) \
                          T=X(A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P)
    #define FATE_TRACE_17(T,X,A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q) \
                          T=X(A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q)
    #define FATE_TRACE_18(T,X,A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R) \
                          T=X(A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R)
    #define FATE_TRACE_19(T,X,A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S) \
                          T=X(A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S)
    #define FATE_TRACE_20(T,X,A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,U) \
                          T=X(A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,U)
    #define FATE_TRACE_21(T,X,A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,U,V) \
                          T=X(A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,U,V)
    #define FATE_TRACE_22(T,X,A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,U,V,W) \
                          T=X(A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,U,V,W)
    #define FATE_TRACE_23(T,X,A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,U,V,W,Y) \
                          T=X(A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,U,V,W,Y)
    #define FATE_TRACE_24(T,X,A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,U,V,W,Y,Z) \
                          T=X(A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,U,V,W,Y,Z)

#else /* FATE_ENABLE_TRACING defined */

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
        #define FATE_TRACE_GENERIC(X) fate_logf_trace("??")
    #else
/*
 * This macro lists all types in a paranoid way. As of today, it is sadly
 * necessary because of various reasons, including compiler quirks.
 */
        #define FATE_TRACE_GENERIC(X) \
            _Generic((X), \
            signed char:              fate_trace_sc, \
            const signed char:        fate_trace_sc, \
            unsigned char:            fate_trace_sc, \
            const unsigned char:      fate_trace_sc, \
            \
            signed short:             fate_trace_ss, \
            const signed short:       fate_trace_ss, \
            unsigned short:           fate_trace_us, \
            const unsigned short:     fate_trace_us, \
            \
            signed int:               fate_trace_s, \
            const signed int:         fate_trace_s, \
            unsigned int:             fate_trace_u, \
            const unsigned int:       fate_trace_u, \
            \
            signed long:              fate_trace_sl, \
            const signed long:        fate_trace_sl, \
            unsigned long:            fate_trace_ul, \
            const unsigned long:      fate_trace_ul, \
            \
            signed long long:         fate_trace_sll, \
            const signed long long:   fate_trace_sll, \
            unsigned long long:       fate_trace_ull, \
            const unsigned long long: fate_trace_ull, \
            \
            float:                    fate_trace_f, \
            const float:              fate_trace_f, \
            double:                   fate_trace_d, \
            const double:             fate_trace_d, \
            long double:              fate_trace_ld, \
            const long double:        fate_trace_ld, \
            \
            const char*:              fate_trace_str, \
            char*:                    fate_trace_str, \
            const signed char*:       fate_trace_str, \
            signed char*:             fate_trace_str, \
            const unsigned char*:     fate_trace_str, \
            unsigned char*:           fate_trace_str, \
            \
            void*:                    fate_trace_voidp, \
            default:                  fate_trace_default \
            )(X)


void fate_trace_chr(const int c);
void fate_trace_sc(const signed char c);
void fate_trace_str(const signed char *str);
void fate_trace_ss(const signed short c);
void fate_trace_us(const unsigned short c);
void fate_trace_s(const signed int c);
void fate_trace_u(const unsigned int c);
void fate_trace_sl(const signed long c);
void fate_trace_ul(const unsigned long c);
void fate_trace_sll(const signed long long c);
void fate_trace_ull(const unsigned long long c);
void fate_trace_f(const float c);
void fate_trace_d(const double c);
void fate_trace_ld(const long double c);
void fate_trace_default(const void *p);
void fate_trace_voidp(const void *p);

    #endif /* _Generic support */

/* So that those which include tracer also include fate_logf_trace(). */
#include <fate/log.h> 

extern unsigned fate_tracer_level; 

    #define FATE_TRACE_0(T,X) \
        ++fate_tracer_level; \
        fate_logf_trace( \
            "~> [%3u] %s:%5u: %s ==> %s(", \
            fate_tracer_level, __FILE__, __LINE__, __func__, XSTRINGIFY(X)); \
        fate_logf_trace(")\n"); \
        T = X(); \
        fate_logf_trace( \
            "~> [%3u] %s:%5u: %s <== ", \
            fate_tracer_level, __FILE__, __LINE__, __func__); \
        FATE_TRACE_GENERIC(T); \
        fate_logf_trace("\n"); \
        --fate_tracer_level

    #define FATE_TRACE_1(T,X,A) \
        ++fate_tracer_level; \
        fate_logf_trace( \
            "~> [%3u] %s:%5u: %s ==> %s(", \
            fate_tracer_level, __FILE__, __LINE__, __func__, XSTRINGIFY(X)); \
        FATE_TRACE_GENERIC(A); \
        fate_logf_trace(")\n"); \
        T = X(A); \
        fate_logf_trace( \
            "~> [%3u] %s:%5u: %s <== ", \
            fate_tracer_level, __FILE__, __LINE__, __func__); \
        FATE_TRACE_GENERIC(T); \
        fate_logf_trace("\n"); \
        --fate_tracer_level

    #define FATE_TRACE_2(T,X,A,B) \
        ++fate_tracer_level; \
        fate_logf_trace( \
            "~> [%3u] %s:%5u: %s ==> %s(", \
            fate_tracer_level, __FILE__, __LINE__, __func__, XSTRINGIFY(X)); \
        FATE_TRACE_GENERIC(A); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(B); \
        fate_logf_trace(")\n"); \
        T = X(A,B); \
        fate_logf_trace( \
            "~> [%3u] %s:%5u: %s <== ", \
            fate_tracer_level, __FILE__, __LINE__, __func__); \
        FATE_TRACE_GENERIC(T); \
        fate_logf_trace("\n"); \
        --fate_tracer_level

    #define FATE_TRACE_3(T,X,A,B,C) \
        ++fate_tracer_level; \
        fate_logf_trace( \
            "~> [%3u] %s:%5u: %s ==> %s(", \
            fate_tracer_level, __FILE__, __LINE__, __func__, XSTRINGIFY(X)); \
        FATE_TRACE_GENERIC(A); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(B); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(C); \
        fate_logf_trace(")\n"); \
        T = X(A,B,C); \
        fate_logf_trace( \
            "~> [%3u] %s:%5u: %s <== ", \
            fate_tracer_level, __FILE__, __LINE__, __func__); \
        FATE_TRACE_GENERIC(T); \
        fate_logf_trace("\n"); \
        --fate_tracer_level

    #define FATE_TRACE_4(T,X,A,B,C,D) \
        ++fate_tracer_level; \
        fate_logf_trace( \
            "~> [%3u] %s:%5u: %s ==> %s(", \
            fate_tracer_level, __FILE__, __LINE__, __func__, XSTRINGIFY(X)); \
        FATE_TRACE_GENERIC(A); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(B); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(C); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(D); \
        fate_logf_trace(")\n"); \
        T = X(A,B,C,D); \
        fate_logf_trace( \
            "~> [%3u] %s:%5u: %s <== ", \
            fate_tracer_level, __FILE__, __LINE__, __func__); \
        FATE_TRACE_GENERIC(T); \
        fate_logf_trace("\n"); \
        --fate_tracer_level

    #define FATE_TRACE_5(T,X,A,B,C,D,E) \
        ++fate_tracer_level; \
        fate_logf_trace( \
            "~> [%3u] %s:%5u: %s ==> %s(", \
            fate_tracer_level, __FILE__, __LINE__, __func__, XSTRINGIFY(X)); \
        FATE_TRACE_GENERIC(A); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(B); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(C); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(D); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(E); \
        fate_logf_trace(")\n"); \
        T = X(A,B,C,D,E); \
        fate_logf_trace( \
            "~> [%3u] %s:%5u: %s <== ", \
            fate_tracer_level, __FILE__, __LINE__, __func__); \
        FATE_TRACE_GENERIC(T); \
        fate_logf_trace("\n"); \
        --fate_tracer_level

    #define FATE_TRACE_6(T,X,A,B,C,D,E,F) \
        ++fate_tracer_level; \
        fate_logf_trace( \
            "~> [%3u] %s:%5u: %s ==> %s(", \
            fate_tracer_level, __FILE__, __LINE__, __func__, XSTRINGIFY(X)); \
        FATE_TRACE_GENERIC(A); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(B); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(C); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(D); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(E); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(F); \
        fate_logf_trace(")\n"); \
        T = X(A,B,C,D,E,F); \
        fate_logf_trace( \
            "~> [%3u] %s:%5u: %s <== ", \
            fate_tracer_level, __FILE__, __LINE__, __func__); \
        FATE_TRACE_GENERIC(T); \
        fate_logf_trace("\n"); \
        --fate_tracer_level

    #define FATE_TRACE_7(T,X,A,B,C,D,E,F,G) \
        ++fate_tracer_level; \
        fate_logf_trace( \
            "~> [%3u] %s:%5u: %s ==> %s(", \
            fate_tracer_level, __FILE__, __LINE__, __func__, XSTRINGIFY(X)); \
        FATE_TRACE_GENERIC(A); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(B); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(C); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(D); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(E); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(F); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(G); \
        fate_logf_trace(")\n"); \
        T = X(A,B,C,D,E,F,G); \
        fate_logf_trace( \
            "~> [%3u] %s:%5u: %s <== ", \
            fate_tracer_level, __FILE__, __LINE__, __func__); \
        FATE_TRACE_GENERIC(T); \
        fate_logf_trace("\n"); \
        --fate_tracer_level

    #define FATE_TRACE_8(T,X,A,B,C,D,E,F,G,H) \
        ++fate_tracer_level; \
        fate_logf_trace( \
            "~> [%3u] %s:%5u: %s ==> %s(", \
            fate_tracer_level, __FILE__, __LINE__, __func__, XSTRINGIFY(X)); \
        FATE_TRACE_GENERIC(A); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(B); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(C); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(D); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(E); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(F); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(G); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(H); \
        fate_logf_trace(")\n"); \
        T = X(A,B,C,D,E,F,G,H); \
        fate_logf_trace( \
            "~> [%3u] %s:%5u: %s <== ", \
            fate_tracer_level, __FILE__, __LINE__, __func__); \
        FATE_TRACE_GENERIC(T); \
        fate_logf_trace("\n"); \
        --fate_tracer_level

    #define FATE_TRACE_9(T,X,A,B,C,D,E,F,G,H,I) \
        ++fate_tracer_level; \
        fate_logf_trace( \
            "~> [%3u] %s:%5u: %s ==> %s(", \
            fate_tracer_level, __FILE__, __LINE__, __func__, XSTRINGIFY(X)); \
        FATE_TRACE_GENERIC(A); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(B); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(C); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(D); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(E); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(F); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(G); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(H); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(I); \
        fate_logf_trace(")\n"); \
        T = X(A,B,C,D,E,F,G,H,I); \
        fate_logf_trace( \
            "~> [%3u] %s:%5u: %s <== ", \
            fate_tracer_level, __FILE__, __LINE__, __func__); \
        FATE_TRACE_GENERIC(T); \
        fate_logf_trace("\n"); \
        --fate_tracer_level

    #define FATE_TRACE_10(T,X,A,B,C,D,E,F,G,H,I,J) \
        ++fate_tracer_level; \
        fate_logf_trace( \
            "~> [%3u] %s:%5u: %s ==> %s(", \
            fate_tracer_level, __FILE__, __LINE__, __func__, XSTRINGIFY(X)); \
        FATE_TRACE_GENERIC(A); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(B); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(C); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(D); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(E); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(F); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(G); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(H); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(I); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(J); \
        fate_logf_trace(")\n"); \
        T = X(A,B,C,D,E,F,G,H,I,J); \
        fate_logf_trace( \
            "~> [%3u] %s:%5u: %s <== ", \
            fate_tracer_level, __FILE__, __LINE__, __func__); \
        FATE_TRACE_GENERIC(T); \
        fate_logf_trace("\n"); \
        --fate_tracer_level

    #define FATE_TRACE_11(T,X,A,B,C,D,E,F,G,H,I,J,K) \
        ++fate_tracer_level; \
        fate_logf_trace( \
            "~> [%3u] %s:%5u: %s ==> %s(", \
            fate_tracer_level, __FILE__, __LINE__, __func__, XSTRINGIFY(X)); \
        FATE_TRACE_GENERIC(A); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(B); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(C); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(D); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(E); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(F); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(G); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(H); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(I); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(J); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(K); \
        fate_logf_trace(")\n"); \
        T = X(A,B,C,D,E,F,G,H,I,J,K); \
        fate_logf_trace( \
            "~> [%3u] %s:%5u: %s <== ", \
            fate_tracer_level, __FILE__, __LINE__, __func__); \
        FATE_TRACE_GENERIC(T); \
        fate_logf_trace("\n"); \
        --fate_tracer_level

    #define FATE_TRACE_12(T,X,A,B,C,D,E,F,G,H,I,J,K,L) \
        ++fate_tracer_level; \
        fate_logf_trace( \
            "~> [%3u] %s:%5u: %s ==> %s(", \
            fate_tracer_level, __FILE__, __LINE__, __func__, XSTRINGIFY(X)); \
        FATE_TRACE_GENERIC(A); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(B); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(C); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(D); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(E); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(F); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(G); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(H); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(I); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(J); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(K); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(L); \
        fate_logf_trace(")\n"); \
        T = X(A,B,C,D,E,F,G,H,I,J,K,L); \
        fate_logf_trace( \
            "~> [%3u] %s:%5u: %s <== ", \
            fate_tracer_level, __FILE__, __LINE__, __func__); \
        FATE_TRACE_GENERIC(T); \
        fate_logf_trace("\n"); \
        --fate_tracer_level

    #define FATE_TRACE_13(T,X,A,B,C,D,E,F,G,H,I,J,K,L,M) \
        ++fate_tracer_level; \
        fate_logf_trace( \
            "~> [%3u] %s:%5u: %s ==> %s(", \
            fate_tracer_level, __FILE__, __LINE__, __func__, XSTRINGIFY(X)); \
        FATE_TRACE_GENERIC(A); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(B); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(C); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(D); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(E); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(F); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(G); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(H); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(I); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(J); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(K); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(L); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(M); \
        fate_logf_trace(")\n"); \
        T = X(A,B,C,D,E,F,G,H,I,J,K,L,M); \
        fate_logf_trace( \
            "~> [%3u] %s:%5u: %s <== ", \
            fate_tracer_level, __FILE__, __LINE__, __func__); \
        FATE_TRACE_GENERIC(T); \
        fate_logf_trace("\n"); \
        --fate_tracer_level

    #define FATE_TRACE_14(T,X,A,B,C,D,E,F,G,H,I,J,K,L,M,N) \
        ++fate_tracer_level; \
        fate_logf_trace( \
            "~> [%3u] %s:%5u: %s ==> %s(", \
            fate_tracer_level, __FILE__, __LINE__, __func__, XSTRINGIFY(X)); \
        FATE_TRACE_GENERIC(A); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(B); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(C); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(D); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(E); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(F); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(G); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(H); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(I); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(J); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(K); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(L); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(M); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(N); \
        fate_logf_trace(")\n"); \
        T = X(A,B,C,D,E,F,G,H,I,J,K,L,M,N); \
        fate_logf_trace( \
            "~> [%3u] %s:%5u: %s <== ", \
            fate_tracer_level, __FILE__, __LINE__, __func__); \
        FATE_TRACE_GENERIC(T); \
        fate_logf_trace("\n"); \
        --fate_tracer_level

    #define FATE_TRACE_15(T,X,A,B,C,D,E,F,G,H,I,J,K,L,M,N,O) \
        ++fate_tracer_level; \
        fate_logf_trace( \
            "~> [%3u] %s:%5u: %s ==> %s(", \
            fate_tracer_level, __FILE__, __LINE__, __func__, XSTRINGIFY(X)); \
        FATE_TRACE_GENERIC(A); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(B); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(C); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(D); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(E); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(F); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(G); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(H); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(I); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(J); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(K); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(L); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(M); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(N); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(O); \
        fate_logf_trace(")\n"); \
        T = X(A,B,C,D,E,F,G,H,I,J,K,L,M,N,O); \
        fate_logf_trace( \
            "~> [%3u] %s:%5u: %s <== ", \
            fate_tracer_level, __FILE__, __LINE__, __func__); \
        FATE_TRACE_GENERIC(T); \
        fate_logf_trace("\n"); \
        --fate_tracer_level

    #define FATE_TRACE_16(T,X,A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P) \
        ++fate_tracer_level; \
        fate_logf_trace( \
            "~> [%3u] %s:%5u: %s ==> %s(", \
            fate_tracer_level, __FILE__, __LINE__, __func__, XSTRINGIFY(X)); \
        FATE_TRACE_GENERIC(A); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(B); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(C); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(D); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(E); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(F); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(G); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(H); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(I); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(J); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(K); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(L); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(M); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(N); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(O); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(P); \
        fate_logf_trace(")\n"); \
        T = X(A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P); \
        fate_logf_trace( \
            "~> [%3u] %s:%5u: %s <== ", \
            fate_tracer_level, __FILE__, __LINE__, __func__); \
        FATE_TRACE_GENERIC(T); \
        fate_logf_trace("\n"); \
        --fate_tracer_level

    #define FATE_TRACE_17(T,X,A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q) \
        ++fate_tracer_level; \
        fate_logf_trace( \
            "~> [%3u] %s:%5u: %s ==> %s(", \
            fate_tracer_level, __FILE__, __LINE__, __func__, XSTRINGIFY(X)); \
        FATE_TRACE_GENERIC(A); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(B); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(C); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(D); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(E); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(F); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(G); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(H); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(I); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(J); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(K); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(L); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(M); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(N); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(O); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(P); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(Q); \
        fate_logf_trace(")\n"); \
        T = X(A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q); \
        fate_logf_trace( \
            "~> [%3u] %s:%5u: %s <== ", \
            fate_tracer_level, __FILE__, __LINE__, __func__); \
        FATE_TRACE_GENERIC(T); \
        fate_logf_trace("\n"); \
        --fate_tracer_level

    #define FATE_TRACE_18(T,X,A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R) \
        ++fate_tracer_level; \
        fate_logf_trace( \
            "~> [%3u] %s:%5u: %s ==> %s(", \
            fate_tracer_level, __FILE__, __LINE__, __func__, XSTRINGIFY(X)); \
        FATE_TRACE_GENERIC(A); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(B); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(C); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(D); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(E); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(F); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(G); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(H); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(I); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(J); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(K); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(L); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(M); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(N); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(O); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(P); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(Q); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(R); \
        fate_logf_trace(")\n"); \
        T = X(A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R); \
        fate_logf_trace( \
            "~> [%3u] %s:%5u: %s <== ", \
            fate_tracer_level, __FILE__, __LINE__, __func__); \
        FATE_TRACE_GENERIC(T); \
        fate_logf_trace("\n"); \
        --fate_tracer_level

    #define FATE_TRACE_19(T,X,A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S) \
        ++fate_tracer_level; \
        fate_logf_trace( \
            "~> [%3u] %s:%5u: %s ==> %s(", \
            fate_tracer_level, __FILE__, __LINE__, __func__, XSTRINGIFY(X)); \
        FATE_TRACE_GENERIC(A); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(B); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(C); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(D); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(E); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(F); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(G); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(H); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(I); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(J); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(K); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(L); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(M); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(N); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(O); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(P); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(Q); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(R); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(S); \
        fate_logf_trace(")\n"); \
        T = X(A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S); \
        fate_logf_trace( \
            "~> [%3u] %s:%5u: %s <== ", \
            fate_tracer_level, __FILE__, __LINE__, __func__); \
        FATE_TRACE_GENERIC(T); \
        fate_logf_trace("\n"); \
        --fate_tracer_level

    #define FATE_TRACE_20(T,X,A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,U) \
        ++fate_tracer_level; \
        fate_logf_trace( \
            "~> [%3u] %s:%5u: %s ==> %s(", \
            fate_tracer_level, __FILE__, __LINE__, __func__, XSTRINGIFY(X)); \
        FATE_TRACE_GENERIC(A); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(B); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(C); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(D); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(E); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(F); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(G); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(H); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(I); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(J); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(K); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(L); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(M); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(N); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(O); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(P); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(Q); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(R); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(S); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(U); \
        fate_logf_trace(")\n"); \
        T = X(A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,U); \
        fate_logf_trace( \
            "~> [%3u] %s:%5u: %s <== ", \
            fate_tracer_level, __FILE__, __LINE__, __func__); \
        FATE_TRACE_GENERIC(T); \
        fate_logf_trace("\n"); \
        --fate_tracer_level

    #define FATE_TRACE_21(T,X,A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,U,V) \
        ++fate_tracer_level; \
        fate_logf_trace( \
            "~> [%3u] %s:%5u: %s ==> %s(", \
            fate_tracer_level, __FILE__, __LINE__, __func__, XSTRINGIFY(X)); \
        FATE_TRACE_GENERIC(A); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(B); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(C); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(D); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(E); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(F); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(G); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(H); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(I); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(J); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(K); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(L); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(M); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(N); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(O); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(P); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(Q); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(R); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(S); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(U); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(V); \
        fate_logf_trace(")\n"); \
        T = X(A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,U,V); \
        fate_logf_trace( \
            "~> [%3u] %s:%5u: %s <== ", \
            fate_tracer_level, __FILE__, __LINE__, __func__); \
        FATE_TRACE_GENERIC(T); \
        fate_logf_trace("\n"); \
        --fate_tracer_level

    #define FATE_TRACE_22(T,X,A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,U,V,W) \
        ++fate_tracer_level; \
        fate_logf_trace( \
            "~> [%3u] %s:%5u: %s ==> %s(", \
            fate_tracer_level, __FILE__, __LINE__, __func__, XSTRINGIFY(X)); \
        FATE_TRACE_GENERIC(A); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(B); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(C); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(D); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(E); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(F); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(G); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(H); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(I); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(J); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(K); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(L); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(M); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(N); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(O); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(P); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(Q); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(R); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(S); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(U); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(V); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(W); \
        fate_logf_trace(")\n"); \
        T = X(A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,U,V,W); \
        fate_logf_trace( \
            "~> [%3u] %s:%5u: %s <== ", \
            fate_tracer_level, __FILE__, __LINE__, __func__); \
        FATE_TRACE_GENERIC(T); \
        fate_logf_trace("\n"); \
        --fate_tracer_level

    #define FATE_TRACE_23(T,X,A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,U,V,W,Y) \
        ++fate_tracer_level; \
        fate_logf_trace( \
            "~> [%3u] %s:%5u: %s ==> %s(", \
            fate_tracer_level, __FILE__, __LINE__, __func__, XSTRINGIFY(X)); \
        FATE_TRACE_GENERIC(A); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(B); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(C); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(D); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(E); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(F); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(G); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(H); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(I); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(J); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(K); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(L); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(M); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(N); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(O); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(P); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(Q); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(R); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(S); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(U); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(V); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(W); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(Y); \
        fate_logf_trace(")\n"); \
        T = X(A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,U,V,W,Y); \
        fate_logf_trace( \
            "~> [%3u] %s:%5u: %s <== ", \
            fate_tracer_level, __FILE__, __LINE__, __func__); \
        FATE_TRACE_GENERIC(T); \
        fate_logf_trace("\n"); \
        --fate_tracer_level

    #define FATE_TRACE_24(T,X,A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,U,V,W,Y,Z) \
        ++fate_tracer_level; \
        fate_logf_trace( \
            "~> [%3u] %s:%5u: %s ==> %s(", \
            fate_tracer_level, __FILE__, __LINE__, __func__, XSTRINGIFY(X)); \
        FATE_TRACE_GENERIC(A); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(B); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(C); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(D); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(E); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(F); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(G); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(H); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(I); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(J); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(K); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(L); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(M); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(N); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(O); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(P); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(Q); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(R); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(S); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(U); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(V); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(W); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(Y); \
        fate_logf_trace(", "); \
        FATE_TRACE_GENERIC(Z); \
        fate_logf_trace(")\n"); \
        T = X(A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,U,V,W,Y,Z); \
        fate_logf_trace( \
            "~> [%3u] %s:%5u: %s <== ", \
            fate_tracer_level, __FILE__, __LINE__, __func__); \
        FATE_TRACE_GENERIC(T); \
        fate_logf_trace("\n"); \
        --fate_tracer_level


#endif /* FATE_ENABLE_TRACING */

#endif /* FATE_TRACER_H */
