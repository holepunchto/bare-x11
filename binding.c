#include <assert.h>
#include <bare.h>
#include <js.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <utf.h>

#include <X11/Xlib.h>

#include "lib/x11.h"

static void
bare_x11__on_display_finalize(js_env_t *env, void *data, void *finalize_hint) {
  bare_x11_display_t *handle = data;

  if (handle->display) XCloseDisplay(handle->display);

  free(handle);
}

static js_value_t *
bare_x11__uint32(js_env_t *env, uint32_t value) {
  int err;

  js_value_t *result;
  err = js_create_uint32(env, value, &result);
  assert(err == 0);

  return result;
}

static js_value_t *
bare_x11__int32(js_env_t *env, int32_t value) {
  int err;

  js_value_t *result;
  err = js_create_int32(env, value, &result);
  assert(err == 0);

  return result;
}

static js_value_t *
bare_x11__null(js_env_t *env) {
  int err;

  js_value_t *result;
  err = js_get_null(env, &result);
  assert(err == 0);

  return result;
}

static js_value_t *
bare_x11_open_display(js_env_t *env, js_callback_info_t *info) {
  int err;

  size_t argc = 1;
  js_value_t *argv[1];

  err = js_get_callback_info(env, info, &argc, argv, NULL, NULL);
  assert(err == 0);

  assert(argc == 1);

  js_value_type_t type;
  err = js_typeof(env, argv[0], &type);
  assert(err == 0);

  utf8_t name[256] = {0};

  if (type == js_string) {
    err = js_get_value_string_utf8(env, argv[0], name, sizeof(name) - 1, NULL);
    assert(err == 0);
  }

  Display *display = XOpenDisplay(type == js_string ? (char *) name : NULL);

  if (display == NULL) return bare_x11__null(env);

  bare_x11_display_t *handle = malloc(sizeof(bare_x11_display_t));

  handle->display = display;

  js_value_t *result;
  err = js_create_object(env, &result);
  assert(err == 0);

  err = js_wrap(env, result, handle, bare_x11__on_display_finalize, NULL, NULL);
  assert(err == 0);

  err = js_add_type_tag(env, result, &bare_x11__display_tag);
  assert(err == 0);

  return result;
}

static js_value_t *
bare_x11_close_display(js_env_t *env, js_callback_info_t *info) {
  int err;

  size_t argc = 1;
  js_value_t *argv[1];

  err = js_get_callback_info(env, info, &argc, argv, NULL, NULL);
  assert(err == 0);

  assert(argc == 1);

  Display *display;
  if (bare_x11_read_display(env, argv[0], &display) < 0) return NULL;

  bare_x11_display_t *handle;
  err = js_unwrap(env, argv[0], (void **) &handle);
  assert(err == 0);

  XCloseDisplay(display);

  handle->display = NULL;

  return NULL;
}

#define DISPLAY(argc_expected) \
  int err; \
  size_t argc = argc_expected; \
  js_value_t *argv[argc_expected]; \
  err = js_get_callback_info(env, info, &argc, argv, NULL, NULL); \
  assert(err == 0); \
  assert(argc == argc_expected); \
  Display *display; \
  if (bare_x11_read_display(env, argv[0], &display) < 0) return NULL;

static int32_t
bare_x11__get_int32(js_env_t *env, js_value_t *value) {
  int err;

  int32_t result;
  err = js_get_value_int32(env, value, &result);
  assert(err == 0);

  return result;
}

static uint32_t
bare_x11__get_uint32(js_env_t *env, js_value_t *value) {
  int err;

  uint32_t result;
  err = js_get_value_uint32(env, value, &result);
  assert(err == 0);

  return result;
}

static bool
bare_x11__get_bool(js_env_t *env, js_value_t *value) {
  int err;

  bool result;
  err = js_get_value_bool(env, value, &result);
  assert(err == 0);

  return result;
}

static js_value_t *
bare_x11_default_root_window(js_env_t *env, js_callback_info_t *info) {
  DISPLAY(1)

  return bare_x11__uint32(env, (uint32_t) DefaultRootWindow(display));
}

static js_value_t *
bare_x11_translate_coordinates(js_env_t *env, js_callback_info_t *info) {
  DISPLAY(5)

  Window src = bare_x11__get_uint32(env, argv[1]);
  Window dest = bare_x11__get_uint32(env, argv[2]);
  int src_x = bare_x11__get_int32(env, argv[3]);
  int src_y = bare_x11__get_int32(env, argv[4]);

  int x, y;
  Window child;

  if (!XTranslateCoordinates(display, src, dest, src_x, src_y, &x, &y, &child)) {
    return bare_x11__null(env);
  }

  js_value_t *result;
  err = js_create_object(env, &result);
  assert(err == 0);

  err = js_set_named_property(env, result, "x", bare_x11__int32(env, x));
  assert(err == 0);

  err = js_set_named_property(env, result, "y", bare_x11__int32(env, y));
  assert(err == 0);

  return result;
}

static js_value_t *
bare_x11_query_pointer(js_env_t *env, js_callback_info_t *info) {
  DISPLAY(2)

  Window window = bare_x11__get_uint32(env, argv[1]);

  Window root, child;
  int root_x, root_y, x, y;
  unsigned int mask;

  if (!XQueryPointer(display, window, &root, &child, &root_x, &root_y, &x, &y, &mask)) {
    return bare_x11__null(env);
  }

  js_value_t *result;
  err = js_create_object(env, &result);
  assert(err == 0);

#define V(name, value) \
  err = js_set_named_property(env, result, name, value); \
  assert(err == 0);

  V("root", bare_x11__uint32(env, (uint32_t) root))
  V("child", bare_x11__uint32(env, (uint32_t) child))
  V("rootX", bare_x11__int32(env, root_x))
  V("rootY", bare_x11__int32(env, root_y))
  V("x", bare_x11__int32(env, x))
  V("y", bare_x11__int32(env, y))
  V("mask", bare_x11__uint32(env, mask))
#undef V

  return result;
}

static js_value_t *
bare_x11_keysym_to_keycode(js_env_t *env, js_callback_info_t *info) {
  DISPLAY(2)

  KeySym keysym = bare_x11__get_uint32(env, argv[1]);

  return bare_x11__uint32(env, XKeysymToKeycode(display, keysym));
}

static js_value_t *
bare_x11_flush(js_env_t *env, js_callback_info_t *info) {
  DISPLAY(1)

  XFlush(display);

  return NULL;
}

static js_value_t *
bare_x11_sync(js_env_t *env, js_callback_info_t *info) {
  DISPLAY(2)

  XSync(display, bare_x11__get_bool(env, argv[1]));

  return NULL;
}

#undef DISPLAY

static js_value_t *
bare_x11_string_to_keysym(js_env_t *env, js_callback_info_t *info) {
  int err;

  size_t argc = 1;
  js_value_t *argv[1];

  err = js_get_callback_info(env, info, &argc, argv, NULL, NULL);
  assert(err == 0);

  assert(argc == 1);

  utf8_t name[64] = {0};
  err = js_get_value_string_utf8(env, argv[0], name, sizeof(name) - 1, NULL);
  assert(err == 0);

  return bare_x11__uint32(env, (uint32_t) XStringToKeysym((char *) name));
}

static js_value_t *
bare_x11_exports(js_env_t *env, js_value_t *exports) {
  int err;

#define V(name, fn) \
  { \
    js_value_t *val; \
    err = js_create_function(env, name, -1, fn, NULL, &val); \
    assert(err == 0); \
    err = js_set_named_property(env, exports, name, val); \
    assert(err == 0); \
  }

  V("openDisplay", bare_x11_open_display)
  V("closeDisplay", bare_x11_close_display)
  V("defaultRootWindow", bare_x11_default_root_window)
  V("translateCoordinates", bare_x11_translate_coordinates)
  V("queryPointer", bare_x11_query_pointer)
  V("keysymToKeycode", bare_x11_keysym_to_keycode)
  V("stringToKeysym", bare_x11_string_to_keysym)
  V("flush", bare_x11_flush)
  V("sync", bare_x11_sync)
#undef V

#define V(name, n) \
  { \
    js_value_t *val; \
    err = js_create_uint32(env, n, &val); \
    assert(err == 0); \
    err = js_set_named_property(env, exports, name, val); \
    assert(err == 0); \
  }

  V("BUTTON1_MASK", Button1Mask)
  V("BUTTON2_MASK", Button2Mask)
  V("BUTTON3_MASK", Button3Mask)
  V("BUTTON4_MASK", Button4Mask)
  V("BUTTON5_MASK", Button5Mask)
  V("SHIFT_MASK", ShiftMask)
  V("CONTROL_MASK", ControlMask)
#undef V

  return exports;
}

BARE_MODULE(bare_x11, bare_x11_exports)
