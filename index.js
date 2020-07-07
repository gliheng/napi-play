var addon = require('bindings')('addon');
addon.readFile('package.json').then(function(ret) {
    console.log('file content:', ret);
});