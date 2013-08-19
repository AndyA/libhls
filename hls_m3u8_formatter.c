/* hls_m3u8_formatter.c */

#include <jd_pretty.h>
#include <string.h>

#include "hls.h"

static int is(jd_var *v, const char *s) {
  return 0 == strcmp(jd_bytes(v, NULL), s);
}

static int is_in(jd_var *ar, jd_var *v) {
  size_t count = jd_count(ar);
  for (unsigned i = 0; i < count; i++)
    if (jd_compare(jd_get_idx(ar, i), v) == 0)
      return 1;
  return 0;
}

static jd_var *format_value(jd_var *out, jd_var *spec, jd_var *val);

static jd_var *format_attr(jd_var *out, jd_var *spec, jd_var *val) {
  scope {
    /* setup template structures */
    jd_var *allow = jd_get_ks(spec, "allow", 0);
    jd_var *require = jd_get_ks(spec, "require", 0);
    jd_var *need = jd_nhv(20);
    jd_var *all = jd_nhv(20);
    if (allow) jd_merge(all, allow, 0);
    if (require) {
      jd_merge(all, require, 0);
      jd_merge(need, require, 0);
    }

    /* format value */
    jd_var *keys = jd_sort(jd_keys(jd_nv(), val));
    size_t count = jd_count(keys);
    jd_var *part = jd_nav(count);

    for (unsigned i = 0; i < jd_count(keys); i++) {
      jd_var *key = jd_get_idx(keys, i);
      jd_var *sp = jd_get_key(all, key, 0);
      if (!sp) jd_throw("Unknown attribute %V", key);
      jd_var *rep = format_value(jd_nv(), sp, jd_get_key(val, key, 0));
      jd_sprintf(jd_push(part, 1), "%V=%V", key, rep);
      jd_delete_key(need, key, NULL);
    }

    /* report missing keys */
    jd_var *missing = jd_sort(jd_keys(jd_nv(), need));
    if (jd_count(missing)) {
      jd_var *list = jd_join(jd_nv(), jd_nsv(", "), missing);
      jd_throw("Missing mandatory attributes: %V", list);
    }

    /* build output */
    jd_join(out, jd_nsv(","), part);
  }
  return out;
}

static jd_var *format_value(jd_var *out, jd_var *spec, jd_var *val) {
  if (spec->type == STRING) {
    if (is(spec, "br")) {
      jd_sprintf(out, "%V@%V",
                 jd_get_ks(val, "length", 0),
                 jd_get_ks(val, "offset", 0));
    }
    else if (is(spec, "extinf")) {
      jd_sprintf(out, "%V,%V",
                 jd_get_ks(val, "duration", 0),
                 jd_get_ks(val, "title", 0));
    }
    else if (is(spec, "bs") || is(spec, "f") || is(spec, "i") || is(spec, "res")) {
      jd_assign(out, val);
    }
    else if (is(spec, "zqs")) {
      jd_to_json(out, val);
    }
    else jd_throw("Unknown type: %V", spec);
  }
  else if (spec->type == ARRAY) {
    if (!is_in(spec, val)) jd_throw("Illegal value: %V", val);
    jd_assign(out, val);
  }
  else if (spec->type == HASH) {
    format_attr(out, spec, val);
  }
  else {
    jd_throw("Bad spec type!");
  }

  return out;
}

static jd_var *format_tag(jd_var *out, jd_var *syn, jd_var *tag, jd_var *val) {
  scope {
    jd_var *spec = jd_get_key(syn, tag, 0);
    if (!spec) jd_throw("Unknown tag: %V", tag);
    if (spec->type == ARRAY && jd_count(spec) == 0) {
      jd_sprintf(out, "#%V", tag);
    }
    else {
      jd_var *rep = format_value(jd_nv(), spec, val);
      jd_sprintf(out, "#%V:%V", tag, rep);
    }
  }
  return out;
};

static void format_record(jd_var *lb, jd_var *syn, jd_var *rec, jd_var *order) {
  if (!rec) return;

  if (rec->type == STRING) {
    jd_sprintf(jd_push(lb, 1), "#%V", rec);
    return;
  }

  scope {
    jd_var *rv = jd_clone(jd_nv(), rec, 0);
    jd_var *uri = jd_nv();
    jd_delete_ks(rv, "uri", uri);

    jd_var *keys = jd_sort(jd_keys(jd_nv(), rv));
    if (order) keys = jd_append(order, keys);
    size_t count = jd_count(keys);

    for (unsigned i = 0; i < count; i++) {
      jd_var *key = jd_get_idx(keys, i);
      jd_var *val = jd_get_key(rv, key, 0);
      if (!val) continue;
      if (val->type == ARRAY) {
        for (unsigned j = 0; j < jd_count(val); j++)
          format_tag(jd_push(lb, 1), syn, key, jd_get_idx(val, j));
      }
      else {
        format_tag(jd_push(lb, 1), syn, key, val);
      }
      jd_delete_key(rv, key, NULL);
    }

    if (uri->type != VOID)
      jd_assign(jd_push(lb, 1), uri);
  }
}

static void format_list(jd_var *lb, jd_var *syn, jd_var *list, jd_var *order) {
  for (unsigned i = 0; i < jd_count(list); i++)
    format_record(lb, syn, jd_get_idx(list, i), order);
}

jd_var *hls_m3u8_format(jd_var *out, jd_var *m3u8) {
  scope {
    jd_var *syn = hls__get_syntax(m3u8);
    jd_var *lb = jd_nav(4000);
    jd_set_string(jd_push(lb, 1), "#EXTM3U");
    jd_var *seg_order = jd_set_array_with(jd_nv(), jd_nsv("EXTINF"), NULL);

    format_record(lb, syn, jd_get_ks(m3u8, "meta", 0), NULL);
    format_list(lb, syn, jd_get_ks(m3u8, "vpl", 0), NULL);
    format_list(lb, syn, jd_get_ks(m3u8, "seg", 0), seg_order);

    if (jd_get_int(jd_get_ks(m3u8, "closed", 0)))
      jd_set_string(jd_push(lb, 1), "#EXT-X-ENDLIST");

    jd_set_empty_string(jd_push(lb, 1), 0);
    jd_join(out, jd_nsv("\n"), lb);
  }
  return out;
}

/* vim:ts=2:sw=2:sts=2:et:ft=c
 */
