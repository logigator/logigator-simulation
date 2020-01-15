const fs = require('fs');
const logicsim = require('./index').logicsim;

let args = process.argv;
let failed = false;

for (let fileIndex = 2; fileIndex < args.length; fileIndex++) {
	process.stdout.write("Running test '" + args[fileIndex] + "'.. ");
	let config = JSON.parse(fs.readFileSync(args[fileIndex]));
	
	logicsim.init(config.board);
	for (let i = 0; i < config.inputTriggers.length; i++) {
		logicsim.triggerInput(config.inputTriggers[i], 0, config.board.components[config.inputTriggers[i]].outputs.map(() => true));
	}
	
	logicsim.start(config.threads || 1, config.ticks || Number.MAX_SAFE_INTEGER, config.ms || Number.MAX_SAFE_INTEGER);
	
	while (logicsim.getStatus().currentState !== 1) {}
	let boardState = logicsim.getBoard();

	let errorOccurred = false;
	for (let i = 0; i < config.expected.components.length; i++) {
		for (let j = 0; j < config.expected.components[i].length; j++) {
			if (boardState.components[i][j] !== config.expected.components[i][j]) {
				if (!errorOccurred)
					console.log('\x1b[31m%s\x1b[0m', "failed!");
				console.error('\x1b[31m%s\x1b[0m', 'Component[' + i + '][' + j + '] failed (expected: ' + config.expected.components[i][j] + ', actual: ' + boardState.components[i][j] + ')');
				errorOccurred = true;
			}
		}
	}
	
	for (let i = 0; i < config.expected.links.length; i++) {
		if (boardState.links[i] !== config.expected.links[i]) {
			if (!errorOccurred)
				console.log('\x1b[31m%s\x1b[0m', "failed!");
			console.error('\x1b[31m%s\x1b[0m', 'Link[' + i + '] failed (expected: ' + config.expected.links[i] + ', actual: ' + boardState.links[i] + ')');
			errorOccurred = true;
		}
	}
	
	if(!errorOccurred)
		console.log('\x1b[32m%s\x1b[0m', 'passed!');
	else
		failed = true;
}

process.exit(failed ? 1 : 0);
