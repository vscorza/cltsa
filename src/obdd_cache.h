/*
 * obdd_cache.h
 *
 *  Created on: Mar 28, 2019
 *      Author: mariano
 */

#ifndef SRC_OBDD_CACHE_H_
#define SRC_OBDD_CACHE_H_

#include "obdd.h"

typedef uintptr_t ptruint;
/* Primes for cache hash functions. */
#define DD_P1			12582917
#define DD_P2			4256249
#define DD_P3			741457
#define DD_P4			1618033999

#if SIZEOF_VOID_P == 8 && SIZEOF_INT == 4
#define ddHash(f,g,s) \
((((unsigned)(ptruint)(f) * DD_P1 + \
   (unsigned)(ptruint)(g)) * DD_P2) >> (s))
#else
#define ddHash(f,g,s) \
((((unsigned)(f) * DD_P1 + (unsigned)(g)) * DD_P2) >> (s))
#endif


/**
  @brief Hash function for the cache.

  @sideeffect none

  @see ddHash ddCHash2

*/
#if SIZEOF_VOID_P == 8 && SIZEOF_INT == 4
#define ddCHash(o,f,g,h,s) \
((((((unsigned)(ptruint)(f) + (unsigned)(ptruint)(o)) * DD_P1 + \
    (unsigned)(ptruint)(g)) * DD_P2 + \
   (unsigned)(ptruint)(h)) * DD_P3) >> (s))
#else
#define ddCHash(o,f,g,h,s) \
((((((unsigned)(f) + (unsigned)(o)) * DD_P1 + (unsigned)(g)) * DD_P2 + \
   (unsigned)(h)) * DD_P3) >> (s))
#endif


/**
  @brief Hash function for the cache for functions with two operands.

  @sideeffect none

  @see ddHash ddCHash

*/
#if SIZEOF_VOID_P == 8 && SIZEOF_INT == 4
#define ddCHash2(o,f,g,s) \
(((((unsigned)(ptruint)(f) + (unsigned)(ptruint)(o)) * DD_P1 + \
   (unsigned)(ptruint)(g)) * DD_P2) >> (s))
#else
#define ddCHash2(o,f,g,s) \
(((((unsigned)(f) + (unsigned)(o)) * DD_P1 + (unsigned)(g)) * DD_P2) >> (s))
#endif



#endif /* SRC_OBDD_CACHE_H_ */
