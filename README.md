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
npm install @logigator/logigator-simulation
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
const logicsim = require('@logigator/logigator-simulation').logicsim;
```
You can also import it using typescript:
```javascript
import {BoardObject, logicsim} from "@logigator/logigator-simulation";
```
### Functions
| Function | Description |
| --- | --- |
| `logicsim.init(board);` | Creates a new Board. A sample board object can be found below. |
| `logicsim.start(threads, ticks, ms);` | Runs the simulation until either the ticks were simulated or the simulation time was reached. |
| `logicsim.stop();` | Stops the simulation if running. |
| `logicsim.getStatus();` | Gets the current Status of a Board, like simulated ticks, current execution state or current simulation speed |
| `logicsim.getLinks();` | Gets states of all links |
| `logicsim.getBoard();` | Gets all components and links and their current state |
| `logicsim.triggerInput(/*index of component*/, /*0 = set, 1 = pulse*/, /*array of states for input*/);` | Triggers an input element on a board |
| `logicsim.destroy();` | Destroys current board. This is required before initializing a new board. |
### Sample Board Object:
```json
{
  "links": 4,
  "components": [
  	{
  		"type": 200,
  		"inputs": [
  		],
  		"outputs": [
  			0
  		]
  	},
  	{
  		"type": 4,
  		"inputs": [
  			0, 1
  		],
  		"outputs": [
  			2
  		]
  	},
  	{
  		"type": 2,
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
| `Module.start(ticks, ms);` | Runs the simulation until either the ticks were simulated or the simulation time was reached. |
| `Module.stop();` | Stops the simulation if running. (Probably useless in webAssembly as active simulation locks current thread anyway.) |
| `Module.getStatus();` | Returns object with data for current state of the simulation |
| `Module.getLinks();` | Returns pointer with the states of all links (1 byte per element, Array length equals number of links on board) |
| `Module.getComponents();` | Return pointer with the states of all inputs and outputs of all components. Format: (component\[0\] inputs)(component\[0\] outputs)(component\[1\] inputs)(component\[1\] outputs)...(component\[n\] inputs)(component\[n\] outputs)  |
| `Module.triggerInput(/*index of component*/, /*0 = set, 1 = pulse*/, /*pointer to array of states for inputs*/);` | Triggers a user input |
| `Module.destroy();` | Destroys current board. This is required before initializing a new board. |
## Component Types
| Type ID | Name | Inputs | Outputs | Ops |
| --- | --- | --- | --- | --- |
| 1 | NOT | 1 | 1 | x |
| 2 | AND | 2 - 2^32 | 1 | x |
| 3 | OR | 2 - 2^32 | 1 | x |
| 4 | XOR | 2 - 2^32 | 1 | x |
| 5 | DELAY | 1 | 1 | x | x |
| 6 | CLOCK | 1 | 1 | [0] => Ticks between clock pulses |
| 10 | Half Adder | 2 | 2 | x |
| 11 | Full Adder | 3 | 2 | x |
| 12 | ROM | 1 - 16 | 1 - 64 | [0..n] => Data for ROM (1 Byte per element) |
| 13 | D Flip-Flop | 2 | 2 | x |
| 14 | JK Flip-Flop | 3 | 2 | x |
| 15 | SR Flip-Flop | 3 | 2 | x |
| 200 | User Input | 0 | 1 - 2^32 | x |
## License
This Project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details
