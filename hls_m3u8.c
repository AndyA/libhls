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

jd_var *hls_m3u8_last_seg(jd_var *m3u8) {
  jd_var *seg = jd_get_ks(m3u8, "seg", 0);
  if (!seg) return NULL;
  unsigned pos = jd_count(seg);
  while (pos > 0) {
    jd_var *s = jd_get_idx(seg, --pos);
    if (s->type == HASH) return s;
  }
  return NULL;
}

/* vim:ts=2:sw=2:sts=2:et:ft=c
 */
