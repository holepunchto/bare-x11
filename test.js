const test = require('brittle')
const { Display, stringToKeysym } = require('.')

function open(t) {
  const display = Display.open()

  t.teardown(() => display.close())

  return display
}

test('connects to the display', (t) => {
  const display = open(t)

  t.ok(display.defaultRootWindow > 0, 'a root window')
})

test('cannot connect to a display that is not there', (t) => {
  t.exception(() => Display.open(':4242'), /Cannot open display ':4242'/)
})

test('finds the pointer', (t) => {
  const display = open(t)
  const root = display.defaultRootWindow

  const pointer = display.queryPointer(root)

  t.is(pointer.root, root, 'on the root window')
  t.is(pointer.x, pointer.rootX, 'in its coordinates')
})

test('finds the key for a keysym', (t) => {
  const display = open(t)

  t.ok(display.keysymToKeycode(stringToKeysym('a')) > 0)
})

test('looks up keysyms', (t) => {
  t.is(stringToKeysym('a'), 0x61)
  t.is(stringToKeysym('Return'), 0xff0d)
  t.is(stringToKeysym('NotAKey'), 0)
})

test('translates between windows', (t) => {
  const display = open(t)
  const root = display.defaultRootWindow

  t.alike(display.translateCoordinates(root, root, 5, 6), { x: 5, y: 6 })
})

test('sends what it has buffered', (t) => {
  const display = open(t)

  display.flush()
  display.sync(true)

  t.pass()
})

test('gives other addons the connection', (t) => {
  const display = open(t)

  t.is(typeof display[Symbol.for('bare.x11.display')], 'object')
})

test('cannot be used once closed', (t) => {
  const display = Display.open()

  display.close()
  display.close()

  t.ok(display.closed, 'closed')
  t.exception(() => display.defaultRootWindow, /Display is closed/)
  t.exception(() => display.sync(), /Display is closed/)
})
