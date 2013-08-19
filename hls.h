/* hls.h */

#ifndef HLS_H_
#define HLS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <jd_pretty.h>

  jd_var *hls__get_syntax(jd_var *m3u8);

  jd_var *hls_m3u8_init(jd_var *out);

  jd_var *hls_m3u8_seg(jd_var *m3u8);
  jd_var *hls_m3u8_vpl(jd_var *m3u8);
  jd_var *hls_m3u8_meta(jd_var *m3u8);
  jd_var *hls_m3u8_retired(jd_var *m3u8);

  jd_var *hls_m3u8_last_seg(jd_var *m3u8);
  unsigned hls_m3u8_retire(jd_var *m3u8, unsigned count);
  unsigned hls_m3u8_count(jd_var *m3u8);
  unsigned hls_m3u8_rotate(jd_var *m3u8, unsigned max_seg);
  unsigned hls_m3u8_expire(jd_var *m3u8, double min_duration);

  int hls_m3u8_set_closed(jd_var *m3u8, int closed);

  int hls_m3u8_push_playlist(jd_var *m3u8, jd_var *pl);
  int hls_m3u8_push_segment(jd_var *m3u8, jd_var *frag);
  int hls_m3u8_push_discontinuity(jd_var *m3u8);

  double hls_m3u8_duration(jd_var *m3u8);
  unsigned hls_m3u8_expire(jd_var *m3u8, double min_duration);

  jd_var *hls_m3u8_parse(jd_var *out, jd_var *m3u8);
  jd_var *hls_m3u8_format(jd_var *out, jd_var *m3u8);

  jd_var *hls_m3u8_load(jd_var *m3u8, const char *filename);
  jd_var *hls_m3u8_save(jd_var *m3u8, const char *filename);

#ifdef __cplusplus
}
#endif

#endif

/* vim:ts=2:sw=2:sts=2:et:ft=c
 */
