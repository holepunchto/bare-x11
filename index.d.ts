import Display = require('./lib/display')
import constants = require('./lib/constants')

/** `XStringToKeysym()`: the keysym named `name`, such as `a` or `Return`, or `0` if there is none. */
export function stringToKeysym(name: string): number

export { Display, constants }
