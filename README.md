# Logigator - Simulation Code Base
[![Build Status](https://travis-ci.org/logigator/logigator-simulation.svg?branch=master)](https://travis-ci.org/logigator/logigator-simulation)
[![Build Status](https://travis-ci.org/logigator/logigator-simulation.svg?branch=development)](https://travis-ci.org/logigator/logigator-simulation)

Multi-Threaded Simulator for Logic Circuits as Node.JS Native C++ Module, alternatively compilable to WebAssembly.
## Getting Started
### Prerequisites
You need to have node and npm installed to use this module. We recommend installing it via [nvm](https://github.com/nvm-sh/nvm):
#### Debian
```shell
curl -o- https://raw.githubusercontent.com/nvm-sh/nvm/v0.35.1/install.sh | bash
export NVM_DIR="$([ -z "${XDG_CONFIG_HOME-}" ] && printf %s "${HOME}/.nvm" || printf %s "${XDG_CONFIG_HOME}/nvm")"
[ -s "$NVM_DIR/nvm.sh" ] && \. "$NVM_DIR/nvm.sh" # This loads nvm
nvm install lts/*
```
Furthermore, python2 is required for compiling:
```shell
apt install python2
```
### Installation
Download the contents of the repository, open the command line in that directory and install it using ```npm install```.
#### Linux / Unix
```shell
git clone 'http://github.com/logigator/logigator-simulation.git' && cd ./logigator-simulation && npm install
```
alternatively, you can install it using npm:
```shell
npm install logicsim
```

To compile to WASM, use this command:
```shell
npm run wasm-install
```
this will download emscripten and compile into ```webAssembly/dist/```
## Usage (Node.JS)
Import the module in your Node.JS Script:
```javascript
const logicsim = require('./index').logicsim;
```
Alternatively, if you installed the package using npm, import it with:
```javascript
const logicsim = require('logicsim').logicsim;
```
You can also import it using typescript:
```javascript
import {BoardObject, logicsim} from "logicsim";
```
### Functions
| Function | Description |
| --- | --- |
| `logicsim.newBoard(/*unique identifier for your board*/, /*object with the contents of the board, see below for an example*/);`  | Creates a new Board |
| `logicsim.startBoard(/*identifier*/ [, /*amount of ticks the simulation will run, leave out for endless simulation*/ ]);` | Starts a Board |
| `logicsim.stopBoard(/*identifier*/);` | Stops a Board, if simulation is running |
| `logicsim.getBoardStatus(/*identifier*/);` | Gets the current Status of a Board, like passed ticks, current execution state or current simulation speed |
| `logicsim.getBoard(/*identifier*/);` | Gets the components and their current state of a board |
| `logicsim.triggerInput(/*identifier*/, /*index of component to trigger*/, /*index of input on component*/, /*input event (0 = down, 1 = up)*/)` | Triggers an input element on a board |
### Sample Board Object:
```json
{
  "links": 4,
  "threads": 1,
  "components": [
  	{
  		"type": "SWITCH",
  		"inputs": [
  		],
  		"outputs": [
  			0
  		]
  	},
  	{
  		"type": "XOR",
  		"inputs": [
  			0, 1
  		],
  		"outputs": [
  			2
  		]
  	},
  	{
  		"type": "AND",
  		"inputs": [
  			0, 2
  		],
  		"outputs": [
  			3
  		]
  	}
  ]
}
```
## Usage (WebAssembly)
Copy the .wasm and .js file to an appropriate location on your web server. Then, import the JS file and initialize WebAssembly:
```HTML
<script src="/path/to/this/file/logigator-simulation.js"></script>
```
```javascript
Module.onRuntimeInitialized = () => {
	console.log('Yay, it worked!');
};
```
To initialize a board, you can do as follows:
```javascript
Module.initLinks(2); // Number of links on that board.
Module.initComponents(4); // Number of components on that board.
for (let i = 0; i < 4; i++) {
  // Allocate arrays for inputs and outputs in heap - further described below
  // ...
  
  Module.initComponent
  (
    i, // index of component to initialize
    1, // type id, determines which component to initialize - a list of all type ids can be found further below
    0x00000001, // pointer to array in heap holding indexes of the links going into the component - how to get an array into the heap is further documented down below
    0x00000010, // pointer to array in heap holding indexes of the links going out of the component
    1, // length of array located in 0x00000001
    1, // length of array located in 0x00000010
    0, // op1 - used for certain component types, ignored otherwise
    0 // op2 - used for certain component types, ignored otherwise
  );
  
  // release memory from allocated arrays to avoid memory leaks
  Module._free(0x00000001);
  Module._free(0x00000010);
}

Module.initBoard(); // finally, finish initialisation
```

To copy an array to heap, you can do:
```javascript
function _arrayToHeap(typedArray) {
	const numBytes = typedArray.length * typedArray.BYTES_PER_ELEMENT;
	const ptr = Module._malloc(numBytes);
	const heapBytes = new Uint8Array(Module.HEAPU8.buffer, ptr, numBytes);
	heapBytes.set(new Uint8Array(typedArray.buffer));
	return ptr;
}
```
to retrieve data from heap (e.g. current state of links from board):
```javascript
Module.HEAP8.slice(0x00000001 /*address of first byte*/, 0x00000010 /*address of last byte*/);
```
### Functions
| Function | Description |
| --- | --- |
| `Module.start();` | Starts the simulation indefinetly. (Keep in mind that this operation locks the current thread.) |
| `Module.startTimeout(ms);` | Runs the simulation for x ms. |
| `Module.startManual(ticks);` | Runs the simulation for x ticks. |
| `Module.stop();` | Stops the simulation if running. (Probably useless in webAssembly as operations lock current thread anyway.) |
| `Module.getStatus();` | Returns object with data for current state of the simulation |
| `Module.getLinks();` | Returns pointer with the states of all links (1 byte per element, Array length equals number of links on board) |
| `Module.getComponents();` | Return pointer with the states of all inputs and outputs of all components. Format: (component\[0\] inputs)(component\[0\] outputs)(component\[1\] inputs)(component\[1\] outputs)...(component\[n\] inputs)(component\[n\] outputs)  |
| `Module.triggerInput(/*index of component*/, /*0 = set, 1 = pulse*/, /*pointer to array of states of components*/);` | Triggers a user input |
| `Module.destroy();` | Destroys current board. This is required before being able to initialize a new board. |
### Component Types
| Type ID | Name | Inputs | Outputs | Op1 | Op2 |
| --- | --- | --- | --- | --- | --- |
| 1 | NOT | 1 | 1 | x | x |
| 2 | AND | 2 - 2^32 | 1 | x | x |
| 3 | OR | 2 - 2^32 | 1 | x | x |
| 4 | XOR | 2 - 2^32 | 1 | x | x |
| 5 | DELAY | 1 | 1 | x | x |
| 6 | CLOCK | 1 | 1 | Ticks between clock pulses | x |
| 200 | User Input | 0 | 1 - 2^32 | x | x |
## License
This Project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details
