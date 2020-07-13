var addon = require('bindings')('addon');
addon.connect('127.0.0.1:3456', {
  connect() {
    console.log('Connected!');
  },
  data(data) {
    console.log('Got data:', data);
  },
});