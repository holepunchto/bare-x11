declare const handle: unique symbol

/** A connection to an X server, as returned by `XOpenDisplay()`. */
declare class X11Display {
  /** Connect to `name`, or to `$DISPLAY` if `null`. Throws if the server cannot be reached. */
  constructor(name?: string | null)

  static open(name?: string | null): X11Display

  /** The connection, for other addons such as `bare-x11-xtest`. */
  readonly [handle]: unknown

  readonly closed: boolean

  /** `DefaultRootWindow()`. */
  readonly defaultRootWindow: number

  /** `XCloseDisplay()`. Closing twice does nothing. */
  close(): void

  /**
   * `XTranslateCoordinates()`: a point in `src` in the coordinates of `dest`, and the child of
   * `dest` that holds it or `0`, or `null` if the two are on different screens.
   */
  translateCoordinates(
    src: number,
    dest: number,
    x: number,
    y: number
  ): { x: number; y: number; child: number } | null

  /** `XQueryPointer()`, or `null` if the pointer is on another screen than `window`. */
  queryPointer(window: number): {
    root: number
    child: number
    rootX: number
    rootY: number
    x: number
    y: number
    mask: number
  } | null

  /** `XQueryTree()`, with the children from bottom to top, or `null` if it fails. */
  queryTree(window: number): { root: number; parent: number; children: number[] } | null

  /** `XCreateSimpleWindow()`. The window is unmapped until `mapWindow()`. */
  createSimpleWindow(
    parent: number,
    x: number,
    y: number,
    width: number,
    height: number,
    borderWidth: number,
    border: number,
    background: number
  ): number

  /** `XDestroyWindow()`. */
  destroyWindow(window: number): void

  /** `XMapWindow()`. */
  mapWindow(window: number): void

  /** `XRaiseWindow()`: put `window` on top of its siblings. */
  raiseWindow(window: number): void

  /** `XKeysymToKeycode()`, or `0` if no key produces `keysym`. */
  keysymToKeycode(keysym: number): number

  /** `XFlush()`: send the requests waiting in the output buffer. */
  flush(): void

  /** `XSync()`: send the requests and wait until the server has handled them. */
  sync(discard?: boolean): void
}

export = X11Display
