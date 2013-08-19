/* misc.t */

#include <math.h>
#include <stdlib.h>

#include "framework.h"
#include "tap.h"

#include "jd_test.h"
#include "jd_pretty.h"

#include "../hls.h"

static jd_var *make_segment(jd_var *out,
                            const char *uri,
                            double duration,
                            const char *title) {
  jd_set_hash(out, 4);
  jd_set_string(jd_lv(out, "$.uri"), uri);
  jd_set_real(jd_lv(out, "$.EXTINF.duration"), duration);
  jd_set_string(jd_lv(out, "$.EXTINF.title"), title ? title : "");
  return out;
}

static jd_int get_sequence(jd_var *m3u8) {
  jd_var *meta = jd_get_ks(m3u8, "meta", 0);
  jd_var *seq = jd_get_ks(meta, "EXT-X-MEDIA-SEQUENCE", 1);
  return jd_get_int(seq);
}

void test_push(void) {
  scope {
    jd_var *m3u8 = hls_m3u8_init(jd_nv());
    unsigned count = 0;
    for (unsigned i = 0; i < 10; i++) {
      for (unsigned j = 0; j < 10; j++) {
        char uri[32];
        sprintf(uri, "%01d%01d.ts", i, j);
        hls_m3u8_push_segment(m3u8, make_segment(jd_nv(), uri, 4, ""));
        ok(jd_get_idx(hls_m3u8_seg(m3u8), -1)->type == HASH, "got segment");
        count++;
        ok(hls_m3u8_count(m3u8) == count, "%u items", count);
      }
      hls_m3u8_push_discontinuity(m3u8);
      ok(jd_get_idx(hls_m3u8_seg(m3u8), 0)->type == HASH, "no initial discontinuity");
      ok(jd_get_idx(hls_m3u8_seg(m3u8), -1)->type == STRING, "got discontinuity");
      ok(hls_m3u8_count(m3u8) == count, "%u items after discontinuity", count);
    }
    for (unsigned i = 1; i <= 37; i++) {
      hls_m3u8_retire(m3u8, 1);
      count--;
      ok(jd_get_idx(hls_m3u8_seg(m3u8), 0)->type == HASH, "no initial discontinuity");
      ok(hls_m3u8_count(m3u8) == count, "%u items after retire", count);
      ok(get_sequence(m3u8) == i, "sequence is %u", i);
    }

    hls_m3u8_rotate(m3u8, 50);
    ok(jd_get_idx(hls_m3u8_seg(m3u8), 0)->type == HASH, "no initial discontinuity");
    ok(hls_m3u8_count(m3u8) == 50, "50 items after rotate");
    jd_int retired = jd_count(hls_m3u8_retired(m3u8));
    if (!ok(retired == 15, "retired 15"))
      diag("retired = %ld", retired);
    ok(get_sequence(m3u8) == 50, "sequence is 50");
    /*    jd_fprintf(stderr, "%V", hls_m3u8_format(jd_nv(), m3u8));*/
  }
}

static int at_least(double *dur, int ndur, double need) {
  double total = 0;
  for (int pos = 0; pos < ndur; pos++) {
    if (total >= need) return pos;
    total += dur[ndur - pos - 1];
  }
  return -1;
}

void test_time(void) {
  scope {
    char uri[32];
    double dur[237], total = 0;
    jd_var *m3u8 = hls_m3u8_init(jd_nv());

    for (unsigned i = 0; i < sizeof(dur) / sizeof(dur[0]); i++) {
      dur[i] = 4 + (double) rand() * 16 / RAND_MAX;
      sprintf(uri, "%08d.ts", i);
      hls_m3u8_push_segment(m3u8, make_segment(jd_nv(), uri, dur[i], ""));
      total += dur[i];
      if ((i & 0x0f) == 0x00)
        hls_m3u8_push_discontinuity(m3u8);
      ok(fabs(total - hls_m3u8_duration(m3u8)) < 0.01, "duration %f", total);
    }
    /*    jd_fprintf(stderr, "%V", hls_m3u8_format(jd_nv(), m3u8));*/
    for (;;) {
      total -= 1 + (double) rand() * 38 / RAND_MAX;
      if (total < 10) break;
      int need = at_least(dur, sizeof(dur) / sizeof(dur[0]), total);
      hls_m3u8_expire(m3u8, total);
      int got = hls_m3u8_count(m3u8);
      if (!ok(got == need, "%f seconds -> %d segments", total, need))
        diag("%f seconds, wanted %d, got %d", total, need, got);
      double d = hls_m3u8_duration(m3u8);
      if (!ok(d >= total, "duration %f > total %f", d, total))
        diag("duration %f < total %f", d, total);
    }
  }
}

void test_main(void) {
  scope {
    test_push();
    test_time();
  }
}

/* vim:ts=2:sw=2:sts=2:et:ft=c
 */
