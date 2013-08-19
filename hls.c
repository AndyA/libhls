/* hls.c */

#include <getopt.h>
#include <jd_pretty.h>
#include <stdlib.h>
#include <string.h>

#include "hls.h"

#define PROG "hls"

static int to_json = 0;
static int to_m3u8 = 0;

static jd_var *load_string(jd_var *out, FILE *f) {
  char buf[0x10000];
  size_t got;

  jd_set_empty_string(out, 1);
  while (got = fread(buf, 1, sizeof(buf), f), got)
    jd_append_bytes(out, buf, got);
  return out;
}

static jd_var *load_file(jd_var *out, const char *fn) {
  if (!strcmp(fn, "-")) {
    return load_string(out, stdin);
  }
  else {
    FILE *fl = fopen(fn, "r");
    if (!fl) jd_throw("Can't read %s: %m\n", fn);
    jd_var *v = load_string(out, fl);
    fclose(fl);
    return v;
  }
}

static void usage() {
  fprintf(stderr, "Usage: " PROG " [options] <file>...\n\n"
          "Options:\n"
          "  -j, --json       Convert M3U8 to JSON\n"
          "  -m, --m3u8       Convert JSON to M3U8\n");
  exit(1);
}

static void parse_options(int *argc, char ***argv) {
  int ch, oidx;

  static struct option opts[] = {
    {"help", no_argument, NULL, 'h'},
    {"json", no_argument, NULL, 'j'},
    {"m3u8", no_argument, NULL, 'm'},
    {NULL, 0, NULL, 0}
  };

  while (ch = getopt_long(*argc, *argv, "hjm", opts, &oidx), ch != -1) {
    switch (ch) {
    case 'j':
      to_json++;
      break;
    case 'm':
      to_m3u8++;
      break;
    case 'h':
    default:
      usage();
    }
  }

  if (to_json && to_m3u8) {
    fprintf(stderr, "Only one conversion allowed\n");
    exit(1);
  }

  *argc -= optind;
  *argv += optind;
}

static void m3u8_to_json(const char *file) {
  scope {
    jd_var *m3u8 = hls_m3u8_parse(jd_nv(), load_file(jd_nv(), file));
    jd_printf("%lJ\n", m3u8);
  }
}

static void json_to_m3u8(const char *file) {
  scope {
    jd_var *m3u8 = hls_m3u8_init(jd_nv());
    jd_var *json = jd_from_json(jd_nv(), load_file(jd_nv(), file));
    jd_merge(m3u8, json, 0);
    jd_var *rep = hls_m3u8_format(jd_nv(), m3u8);
    jd_printf("%V", rep);
  }
}

static void process(const char *file) {
  if (to_json) m3u8_to_json(file);
  else if (to_m3u8) json_to_m3u8(file);
  else {
    char *dot = strchr(file, '.');
    if (dot && !strcmp(dot, ".json"))
      json_to_m3u8(file);
    else if (dot && !strcmp(dot, ".m3u8"))
      m3u8_to_json(file);
    else {
      fprintf(stderr, "No conversion specified and can't guess from %s\n", file);
      exit(1);
    }
  }
}

int main(int argc, char *argv[]) {
  parse_options(&argc, &argv);
  for (int i = 0; i < argc; i++) {
    process(argv[i]);
  }
  return 0;
}

/* vim:ts=2:sw=2:sts=2:et:ft=c
 */
