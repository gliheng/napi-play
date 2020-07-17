var addon = require('bindings')('addon');
addon.setTimeout(function() {
    console.log('timeout!');
}, 1000);