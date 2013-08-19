/* parser.t */

#include "framework.h"
#include "tap.h"

#include "jd_test.h"
#include "jd_pretty.h"

struct {
  const char *in, *out;
} tests[] = {
  {"data/ref/byterange.m3u8", "data/byterange.json"},
  {"data/ref/complex.m3u8", "data/complex.json"},
  {"data/ref/datetime.m3u8", "data/datetime.json"},
  {"data/ref/discontinuity.m3u8", "data/discontinuity.json"},
  {"data/ref/endlist.m3u8", "data/endlist.json"},
  {"data/ref/iframe_index.m3u8", "data/iframe_index.json"},
  {"data/ref/simple_root.m3u8", "data/simple_root.json"},
  {"data/ref/simple_var.m3u8", "data/simple_var.json"}
};

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

static void check_parser(const char *in, const char *out) {
  scope {
    jd_var *m3u8 = load_file(jd_nv(), in);
    jd_var *json = load_file(jd_nv(), out);
    jd_var *got = hls_m3u8_parse(jd_nv(), m3u8);
    jd_var *want = jd_from_json(jd_nv(), json);
    jdt_is(got, want, "%s -> %s", in, out);
  }
}

static void test_parser(void) {
  for (unsigned i = 0; i < sizeof(tests) / sizeof(tests[0]); i++)
    check_parser(tests[i].in, tests[i].out);
}

void test_main(void) {
  scope {
    test_parser();
  }
}

/* vim:ts=2:sw=2:sts=2:et:ft=c
 */
