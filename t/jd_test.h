/* jd_test.h */

#ifndef __JD_TEST_H
#define __JD_TEST_H

#include "jsondata.h"

int jdt_is(jd_var *got, jd_var *want, const char *msg, ...);
int jdt_is_json(jd_var *got, const char *want, const char *msg, ...);
int jdt_is_string(jd_var *got, const char *want, const char *msg, ...);
int jdt_throws(void (*func)(void *), void *ctx, const char *want, const char *msg, ...);
void jdt_diag(const char *msg, ...);

#endif

/* vim:ts=2:sw=2:sts=2:et:ft=c
 */
