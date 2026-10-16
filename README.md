# bare-x11

Xlib bindings for Bare. It covers what a test needs to find its way around an X server: the connection, the root window, the pointer and keysyms.

```
npm i bare-x11
```

## Usage

```js
const { Display } = require('bare-x11')

const display = Display.open()

const { rootX, rootY } = display.queryPointer(display.defaultRootWindow)

display.close()
```

## Interop

A display carries its connection under `Symbol.for('bare.x11.display')`. An addon reads it with `bare_x11_read_display()` from `lib/x11.h`, which throws if the value is not a display or the display is closed.

## License

Apache-2.0
