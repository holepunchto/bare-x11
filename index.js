const binding = require('./binding')

exports.Display = require('./lib/display')
exports.constants = require('./lib/constants')

exports.stringToKeysym = function stringToKeysym(name) {
  return binding.stringToKeysym(name)
}
