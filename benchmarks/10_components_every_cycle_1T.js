const logicsim = require('../index').logicsim;

var components = [];

components.push({
    "type": 6,
    "ops": [1],
    "inputs": [
        1
    ],
    "outputs": [
        0
    ]
});

for (let i = 0; i < 10; i++) {
    components.push({
        "type": 2,
        "inputs": [
            0, 1
        ],
        "outputs": [
            2
        ]
    });
}

logicsim.init({
	"links" : 3,
    "components": components
});
logicsim.start(1);

run();

async function run() {
    await new Promise(resolve => setTimeout(resolve, 3000));
    logicsim.stop();
    console.log(logicsim.getStatus().currentSpeed);
}
