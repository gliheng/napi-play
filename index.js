var addon = require('bindings')('addon');
let obj = addon.getObject();
console.log('obj:', obj);
console.log('obj.rand:', obj.rand);