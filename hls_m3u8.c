/* hls_m3u8.c */

#include <math.h>

#include "hls.h"

extern const char *hls__m3u8_syntax;;

jd_var *hls__get_syntax(jd_var *m3u8) {
  jd_var *syntax = jd_get_ks(m3u8, "syntax", 1);
  if (syntax->type == VOID)
    jd_from_jsons(syntax, hls__m3u8_syntax);
  return syntax;
}

static jd_var *get_array(jd_var *m3u8, const char *name) {
  jd_var *ar = jd_get_ks(m3u8, name, 1);
  if (ar->type != ARRAY) jd_set_array(ar, 100);
  return ar;
}

jd_var *hls_m3u8_seg(jd_var *m3u8) {
  return get_array(m3u8, "seg");
}

jd_var *hls_m3u8_vpl(jd_var *m3u8) {
  return get_array(m3u8, "vpl");
}

jd_var *hls_m3u8_init(jd_var *out) {
  jd_set_hash(out, 4);
  jd_set_hash(jd_lv(out, "$.meta"), 10);
  jd_set_array(jd_lv(out, "$.seg"), 1000);
  jd_set_array(jd_lv(out, "$.vpl"), 20);
  jd_set_bool(jd_lv(out, "$.closed"), 0);
  return out;
}

#define IS_DISCONTINUITY(s) ((s)->type == STRING)
#define IS_SEGMENT(s)       ((s)->type == HASH)

jd_var *hls_m3u8_last_seg(jd_var *m3u8) {
  jd_var *seg = hls_m3u8_seg(m3u8);
  unsigned pos = jd_count(seg);
  while (pos > 0) {
    jd_var *s = jd_get_idx(seg, --pos);
    if (IS_SEGMENT(s)) return s;
  }
  return NULL;
}

unsigned hls_m3u8_retire(jd_var *m3u8, unsigned count) {
  jd_var *seg = hls_m3u8_seg(m3u8);
  unsigned done = 0;

  while (done != count && jd_count(seg)) {
    if (IS_DISCONTINUITY(jd_get_idx(seg, 0))) {
      jd_shift(seg, 1, NULL);
      continue;
    }
    jd_shift(seg, 1, NULL);
    done++;
    if (jd_count(seg) && IS_DISCONTINUITY(jd_get_idx(seg, 0)))
      jd_shift(seg, 1, NULL);
  }

  jd_var *meta = jd_get_ks(m3u8, "meta", 0);
  jd_var *seq = jd_get_ks(meta, "EXT-X-MEDIA-SEQUENCE", 1);
  jd_set_int(seq, jd_get_int(seq) + done);

  return done;
}

static unsigned count_to(jd_var *m3u8, unsigned count) {
  jd_var *seg = hls_m3u8_seg(m3u8);
  unsigned got = 0;
  for (unsigned i = 0; i < count; i++)
    if (jd_get_idx(seg, i)->type == HASH) got++;
  return got;
}

unsigned hls_m3u8_count(jd_var *m3u8) {
  return count_to(m3u8, jd_count(hls_m3u8_seg(m3u8)));
}

unsigned hls_m3u8_rotate(jd_var *m3u8, unsigned max_seg) {
  int excess = hls_m3u8_count(m3u8) - max_seg;
  if (excess > 0) return hls_m3u8_retire(m3u8, excess);
  return 0;
}

int hls_m3u8_push_playlist(jd_var *m3u8, jd_var *pl) {
  jd_assign(jd_push(hls_m3u8_vpl(m3u8), 1), pl);
  return 1;
}

int hls_m3u8_push_segment(jd_var *m3u8, jd_var *frag) {
  jd_assign(jd_push(hls_m3u8_seg(m3u8), 1), frag);
  return 1;
}

int hls_m3u8_push_discontinuity(jd_var *m3u8) {
  jd_var *seg = hls_m3u8_seg(m3u8);

  if (jd_count(seg) == 0 || IS_DISCONTINUITY(jd_get_idx(seg, -1)))
    return 0;

  jd_set_string(jd_push(seg, 1), "EXT-X-DISCONTINUITY");
  return 1;
}

unsigned duration_span(jd_var *m3u8, double *limit) {
  jd_var *seg = hls_m3u8_seg(m3u8);
  unsigned pos = jd_count(seg);
  double elapsed = 0;
  while (pos != 0) {
    jd_var *s = jd_get_idx(seg, --pos);
    if (IS_DISCONTINUITY(s)) continue;
    double duration = jd_get_real(jd_rv(s, "$.EXTINF.duration"));
    elapsed += duration;
    if (*limit && !isnan(*limit) && elapsed >= *limit) break;
  }
  if (*limit) *limit = elapsed;
  return pos;
}

double hls_m3u8_duration(jd_var *m3u8) {
  double limit = NAN;
  duration_span(m3u8, &limit);
  return limit;
}

unsigned hls_m3u8_expire(jd_var *m3u8, double min_duration) {
  unsigned pos = duration_span(m3u8, &min_duration);
  unsigned count = count_to(m3u8, pos);
  return hls_m3u8_retire(m3u8, count);
}

/* vim:ts=2:sw=2:sts=2:et:ft=c
 */
