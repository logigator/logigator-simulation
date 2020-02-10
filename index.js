const fs = require('fs');
const path = require('path');

let logicsim;
if (fs.existsSync(path.join(__dirname, './build/Debug/logigatorSimulation.node')))
	logicsim = require('./build/Debug/logigatorSimulation.node');
else
	logicsim = require('./build/Release/logigatorSimulation.node');

module.exports = {logicsim};
