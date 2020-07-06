var addon = require('bindings')('addon');
console.log(addon.hello(function() {
    console.log('callback function called');
    return 'hello';
}));