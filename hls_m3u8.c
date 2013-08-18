/* hls_m3u8.c */

#include "hls.h"

jd_var *hls_m3u8_init(jd_var *out) {
  jd_set_hash(out, 4);
  jd_set_hash(jd_lv(out, "$.meta"), 10);
  jd_set_array(jd_lv(out, "$.seg"), 1000);
  jd_set_array(jd_lv(out, "$.vpl"), 20);
  jd_set_bool(jd_lv(out, "$.closed"), 0);
  return out;
}

/* vim:ts=2:sw=2:sts=2:et:ft=c
 */
