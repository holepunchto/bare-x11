const binding = require('../binding')

const handle = Symbol.for('bare.x11.display')

module.exports = class X11Display {
  constructor(name = null) {
    this._handle = binding.openDisplay(name)

    if (this._handle === null) {
      throw new Error(
        name === null ? 'Cannot open the default display' : `Cannot open display '${name}'`
      )
    }

    this._closed = false
  }

  static open(name) {
    return new this(name)
  }

  // What another addon reads the connection from.
  get [handle]() {
    return this._handle
  }

  get closed() {
    return this._closed
  }

  get defaultRootWindow() {
    return binding.defaultRootWindow(this._handle)
  }

  close() {
    if (this._closed) return

    this._closed = true

    binding.closeDisplay(this._handle)
  }

  translateCoordinates(src, dest, x, y) {
    return binding.translateCoordinates(this._handle, src, dest, x, y)
  }

  queryPointer(window) {
    return binding.queryPointer(this._handle, window)
  }

  queryTree(window) {
    return binding.queryTree(this._handle, window)
  }

  createSimpleWindow(parent, x, y, width, height, borderWidth, border, background) {
    return binding.createSimpleWindow(
      this._handle,
      parent,
      x,
      y,
      width,
      height,
      borderWidth,
      border,
      background
    )
  }

  destroyWindow(window) {
    binding.destroyWindow(this._handle, window)
  }

  mapWindow(window) {
    binding.mapWindow(this._handle, window)
  }

  raiseWindow(window) {
    binding.raiseWindow(this._handle, window)
  }

  keysymToKeycode(keysym) {
    return binding.keysymToKeycode(this._handle, keysym)
  }

  flush() {
    binding.flush(this._handle)
  }

  sync(discard = false) {
    binding.sync(this._handle, discard)
  }

  [Symbol.for('bare.inspect')]() {
    return {
      __proto__: { constructor: X11Display },
      closed: this.closed
    }
  }
}
