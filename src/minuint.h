/*
 * minuint.h
 *
 *  Created on: May 19, 2019
 *      Author: mariano
 */

#ifndef SRC_MINUINT_H_
#define SRC_MINUINT_H_

 /* file: minunit.h */
 #define mu_assert(message, test) do { if (!(test)) return message; } while (0)
 #define mu_run_test(test) do { char *message = test(); tests_run++; \
                                if (message) return message; } while (0)
 extern int tests_run;



#endif /* SRC_MINUINT_H_ */
