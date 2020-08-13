var addon = require('bindings')('addon');
addon.setInterval(function() {
    console.log('timeout!');
}, 1000);