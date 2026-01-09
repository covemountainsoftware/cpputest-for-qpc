
#ifndef CPPUTEST_FOR_QPC_QEP_PORT_H
#define CPPUTEST_FOR_QPC_QEP_PORT_H

#ifdef __GNUC__
    #define Q_NORETURN   __attribute__ ((noreturn))  void
#endif

#include <stdint.h>  /* Exact-width types. WG14/N843 C99 Standard */
#include <stdbool.h> /* Boolean type.      WG14/N843 C99 Standard */
#include <stddef.h>  /* for size_t */
#include "qp.h"     /* QEP platform-independent public interface */

#endif   // CPPUTEST_FOR_QPC_QEP_PORT_H
