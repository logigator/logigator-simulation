const fs = require('fs');
const logicsim = require('./index').logicsim;

async function run() {
	let threads = 1;
	let files = [];
	for (let i = 2; i < process.argv.length; i++) {
		switch (process.argv[i]) {
			case '--threads':
			case '-t':
				threads = Number(process.argv[++i]);
				if (!threads)
					threads = 1;
				break;
			default:
				files.push(process.argv[i]);
		}
	}

	for (const file of files) {
		process.stdout.write(`[${file}] Initialising..`);
		const config = JSON.parse(await fs.promises.readFile(file, {encoding: 'utf8'}));

		let time = process.hrtime();
		logicsim.init(config.board);
		console.log(` \x1b[33m${process.hrtime(time)[1] * 1e-6}ms\x1b[0m`);

		process.stdout.write(`[${file}] Benchmarking`);
		logicsim.start(threads);
		for (let i = 0; i < 3; i++) {
			process.stdout.write('.');
			await new Promise(resolve => setTimeout(resolve, 1000));
		}
		logicsim.stop();
		console.log(` \x1b[33m${humanise(logicsim.getStatus().currentSpeed)}Hz\x1b[0m`);
	}
}

function humanise(value) {
	const si = ['k', 'M', 'G', 'T', 'P', 'E', 'Z', 'Y'];

	for (let i = 0; i < 8; i++) {
		if ((value /= 1000) < 1000)
			return value + si[i];
	}
}

run();
