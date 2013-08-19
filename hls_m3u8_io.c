/* hls_m3u8_io.c */

#include <stdio.h>

#include "hls.h"


static jd_var *load_string(jd_var *out, FILE *f) {
  char buf[0x10000];
  size_t got;

  jd_set_empty_string(out, 1);
  while (got = fread(buf, 1, sizeof(buf), f), got)
    jd_append_bytes(out, buf, got);

  if (ferror(f)) jd_throw("I/O error: %m");

  return out;
}

static jd_var *load_file(jd_var *out, const char *fn) {
  FILE *fl = fopen(fn, "r");
  if (!fl) jd_throw("Can't read %s: %m\n", fn);
  jd_var *v = load_string(out, fl);
  fclose(fl);
  return v;
}

static jd_var *save_string(jd_var *s, FILE *f) {
  size_t len;
  const char *buf = jd_bytes(s, &len);
  len--;
  size_t put = fwrite(buf, 1, len, f);
  if (put != len || ferror(f)) jd_throw("I/O error: %m");
  return s;
}

static jd_var *save_file(jd_var *s, const char *fn) {
  FILE *fl = fopen(fn, "w");
  if (!fl) jd_throw("Can't write %s: %m\n", fn);
  save_string(s, fl);
  fclose(fl);
  return s;
}

jd_var *hls_m3u8_load(jd_var *m3u8, const char *filename) {
  scope hls_m3u8_parse(m3u8, load_file(jd_nv(), filename));
  return m3u8;
}

jd_var *hls_m3u8_save(jd_var *m3u8, const char *filename) {
  scope save_file(hls_m3u8_format(jd_nv(), m3u8), filename);
  return m3u8;
}

/* vim:ts=2:sw=2:sts=2:et:ft=c
 */
