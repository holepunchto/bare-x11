#pragma once

#include <assert.h>
#include <js.h>
#include <stdbool.h>

#include <X11/Xlib.h>

static const js_type_tag_t bare_x11__display_tag = {0x5d3a1f7c9e2b4a60, 0xa47c2e91d05b3f18};

typedef struct {
  Display *display;
} bare_x11_display_t;

/**
 * Read the display of a `bare-x11` display handle into `result` and return 0.
 * If `value` is not a handle or the display is closed, throw a JavaScript error
 * and return -1.
 */
static int
bare_x11_read_display(js_env_t *env, js_value_t *value, Display **result) {
  int err;

  js_value_type_t type;
  err = js_typeof(env, value, &type);
  assert(err == 0);

  bool is_display = false;

  if (type == js_object) {
    err = js_check_type_tag(env, value, &bare_x11__display_tag, &is_display);
    assert(err == 0);
  }

  if (!is_display) {
    err = js_throw_type_error(env, NULL, "Expected a display");
    assert(err == 0);

    return -1;
  }

  bare_x11_display_t *handle;
  err = js_unwrap(env, value, (void **) &handle);
  assert(err == 0);

  if (handle->display == NULL) {
    err = js_throw_error(env, NULL, "Display is closed");
    assert(err == 0);

    return -1;
  }

  *result = handle->display;

  return 0;
}
