/* hls.c */

#include <jd_pretty.h>

#include "hls.h"

static jd_var *load_string(jd_var *out, FILE *f) {
  char buf[0x10000];
  size_t got;

  jd_set_empty_string(out, 1);
  while (got = fread(buf, 1, sizeof(buf), f), got)
    jd_append_bytes(out, buf, got);
  return out;
}

static jd_var *load_file(jd_var *out, const char *fn) {
  FILE *fl = fopen(fn, "r");
  if (!fl) jd_throw("Can't read %s: %m\n", fn);
  jd_var *v = load_string(out, fl);
  fclose(fl);
  return v;
}

int main(int argc, char *argv[]) {
  for (int i = 1; i < argc; i++) {
    scope {
      jd_printf("file: %s\n", argv[i]);
      jd_var *m3u8 = hls_m3u8_parse(jd_nv(), load_file(jd_nv(), argv[i]));
      jd_printf("m3u8: %lJ\n", m3u8);
    }
  }
  return 0;
}

/* vim:ts=2:sw=2:sts=2:et:ft=c
 */
