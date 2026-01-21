# Selfish Mining Simulator

## Overview
This repository contains a **discrete-event simulation** that replicates the findings of Eyal and Sirer's 2013 paper, *"Majority is not enough: Bitcoin mining is vulnerable."*

The project validates the theoretical claim that a Bitcoin mining pool can earn revenue disproportionate to its size by selectively withholding blocks ("Selfish Mining").



## Features
* Simulates block discovery without computing cryptopuzzle.
* Validates the critical threshold of **$\alpha = 0.33$** (pool size).
* Accounts for the propagation parameter ($\gamma$) to simulate honest node splits.

## Installation

1. **Clone the repository:**
```bash
   $ git clone https://github.com/khonmaksim/selfish-mining-sim.git
   $ cd selfish-mining-sim
```

## Usage

1. **Compile**

```bash
   $ make
```

2. **Optional Parameters (can be skipped)**

* gamma parameter: split ratio; float from 0 to 1 (both included).
* data size: number of samples in x-axis (the bigger, the more fine plot will be); integer from 0 to 1000000 (0 excluded).
* blocks: number of block discoveries to simulate; positive integer. 

3. **Run a Simulation with default parameters (Reproduce the Paper's Graph)**

Note: running this may a take a minute (see how to modify default parameters below)

```bash
   $ ./main
```
This generates data.txt and plot.png in the current directory

Defaults parameters are:
* gamma parameter = 0.0, 0.5, 1 (three plots on one canvas)
* data size = 1000
* blocks = 1000000

You can modify default parameters by changing numerical values of the following constant in the source code 
```
   #define DATA_SIZE 1000
   #define BLOCKS 1000000
```

4. **Run a Simulation with Specific parameters**

```bash
   $ ./main {gamma parameter} {data size} {blocks}
``` 

