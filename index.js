var addon = require('bindings')('addon');
let c = new addon.Counter(10);
c.inc();
c.inc();
console.log('After inc:', c.value);
c.dec();
console.log('After dec:', c.value);
