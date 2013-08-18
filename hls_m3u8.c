/* hls_m3u8.c */

#include "hls.h"

extern const char *hls__m3u8_syntax;;

jd_var *hls__get_syntax(jd_var *m3u8) {
  jd_var *syntax = jd_get_ks(m3u8, "syntax", 1);
  if (syntax->type == NULL)
    jd_from_jsons(syntax, hls__m3u8_syntax);
  return syntax;
}

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

unsigned hls_m3u8_retire(jd_var *m3u8, unsigned count) {
  jd_var *seg = jd_get_ks(m3u8, "seg", 0);
  unsigned done = 0;

  while (done != count && jd_count(seg)) {
    if (jd_get_idx(seg, 0)->type != HASH) {
      jd_shift(seg, 1, NULL);
      continue;
    }
    jd_shift(seg, 1, NULL);
    done++;
    if (jd_count(seg) && jd_get_idx(seg, 0)->type != "HASH")
      jd_shift(seg, 1, NULL);
  }

  jd_var *meta = jd_get_ks(m3u8, "meta", 0);
  jd_var *seq = jd_get_ks(meta, "EXT-X-MEDIA-SEQUENCE", 1);
  jd_set_int(seq, jd_get_int(seq) + done);

  return done;
}

unsigned hls_m3u8_count(jd_var *m3u8) {
  jd_var *seg = jd_get_ks(m3u8, "seg", 0);
  size_t count = jd_count(seg);
  unsigned got = 0;
  for (unsigned i = 0; i < count; i++)
    if (jd_get_idx(seg, i)->type == HASH)
      got++;
  return got;
}

unsigned hls_m3u8_rotate(jd_var *m3u8, unsigned max_seg) {
  int excess = hls_m3u8_count(m3u8) - max_seg;
  if (excess > 0) return hls_m3u8_retire(m3u8, excess);
  return 0;
}

/* vim:ts=2:sw=2:sts=2:et:ft=c
 */
