/* jd_test.c */

#include "jd_pretty.h"
#include "jd_test.h"
#include "tap.h"

static int _is(jd_var *got, jd_var *want, const char *msg, va_list ap) {
  int rc;
  rc = test(jd_compare(got, want) == 0, msg, ap);
  if (!rc) {
    jdt_diag("wanted %V", want);
    jdt_diag("got %V", got);
  }
  return rc;
}

static int _isj(jd_var *got, jd_var *want, const char *msg, va_list ap) {
  jd_var vgot = JD_INIT;
  int rc;
  jd_to_json(&vgot, got);
  rc = test(jd_compare(&vgot, want) == 0, msg, ap);
  if (!rc) {
    jd_var pwant = JD_INIT;
    jd_from_json(&pwant, want);
    jdt_diag("wanted %lJ", &pwant);
    jdt_diag("got %lJ", got);
    jd_release(&pwant);
  }
  jd_release(&vgot);
  return rc;
}

int jdt_is(jd_var *got, jd_var *want, const char *msg, ...) {
  va_list ap;
  jd_var vwant = JD_INIT;
  int rc;

  jd_to_json(&vwant, want);
  va_start(ap, msg);
  rc = _isj(got, &vwant, msg, ap);
  va_end(ap);
  jd_release(&vwant);
  return rc;
}

int jdt_is_json(jd_var *got, const char *want, const char *msg, ...) {
  va_list ap;
  jd_var vwant = JD_INIT;
  int rc;

  jd_set_string(&vwant, want);
  va_start(ap, msg);
  rc = _isj(got, &vwant, msg, ap);
  va_end(ap);
  jd_release(&vwant);
  return rc;
}

int jdt_is_string(jd_var *got, const char *want, const char *msg, ...) {
  va_list ap;
  jd_var vwant = JD_INIT;
  int rc;

  jd_set_string(&vwant, want);
  va_start(ap, msg);
  rc = _is(got, &vwant, msg, ap);
  va_end(ap);
  jd_release(&vwant);
  return rc;
}

int jdt_throws(void (*func)(void *), void *ctx, const char *want, const char *msg, ...) {
  va_list ap;
  int rc = 0;

  scope {
    jd_var *caught = jd_nv();
    jd_var *vwant = jd_nsv(want);

    try {
      func(ctx);
    }
    catch (e) {
      jd_assign(caught, e);
    }

    va_start(ap, msg);
    rc = _is(jd_get_ks(caught, "message", 0), vwant, msg, ap);
    va_end(ap);
  }
  return rc;
}

void jdt_diag(const char *msg, ...) {
  scope {
    jd_var *vmsg = jd_nv();
    va_list ap;

    va_start(ap, msg);
    jd_vsprintf(vmsg, msg, ap);
    va_end(ap);
    diag(jd_bytes(vmsg, NULL));
  }
}

/* vim:ts=2:sw=2:sts=2:et:ft=c
 */
