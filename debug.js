const logicsim = require('./build/Debug/logigatorSimulation.node');
const fs = require('fs');

const file = 'debug.json';

const board = JSON.parse(fs.readFileSync(file, {encoding: 'utf8'}));
logicsim.init(board);

process.stdout.write('> ');
process.openStdin().addListener('data', (data) => {
	data = data.toString().trim().split(' ');
	const arguments = processArgs(data);

	switch (data[0].toLowerCase()) {
		case 'tick':
			logicsim.start(1, 1, Number.MAX_SAFE_INTEGER, true);
			break;
		case 'start':
			logicsim.start(Number(arguments.threads) || 1, Number(arguments.ticks) || Number.MAX_SAFE_INTEGER);
			break;
		case 'stop':
			logicsim.stop();
			break;
		case 'status':
			console.log(logicsim.getStatus());
			break;
		case 'triggerinput':
			const inputIndex = Number(arguments['0'] || arguments['index']);
			if (!inputIndex || !board.components[inputIndex] || board.components[inputIndex].type !== 200) {
				console.error('No component index supplied or component index invalid.');
				break;
			}
			const inputStates = (arguments['2'] || arguments['state'] || '')
				.split(',')
				.map(x => x !== 'false' && Number(x) !== 0)
				.slice(0, board.components[inputIndex].outputs.length);
			logicsim.triggerInput
			(
				inputIndex,
				Number(arguments['1']) || Number(arguments['action']) || 0,
				inputStates
			);
			break;
		case 'state':
			console.log(logicsim.getBoard());
			break;
		default:
			console.error('Unknown command.');
	}

	process.stdout.write('> ');
});

function processArgs(command) {
	let arguments = { };
	let index = 0;
	for (let i = 1; i < command.length; i++) {
		if (command[i].startsWith('--') && command[i + 1]) {
			arguments[command[i].substring(2)] = command[++i];
		} else {
			arguments[index++] = command[i];
		}
	}
	return arguments;
}
