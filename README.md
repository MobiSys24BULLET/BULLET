# BULLET

## Introduction

This repository contains the NS-3 simulator used in the paper "BULLET: Boosting 5G Uplink with Demand-Identfiying Scheduling for Real-time Communications" submitted to Mobisys 24. This simulator is specifically designed to explore the performance of real-time communication applications on 5G NR. We've implemented features not present in previous 5G NR simulators, focusing on the difference in uplink scheduling techniques, and have fixed critical bugs.

## How to Use

To get started with BULLET, follow these steps:

### 1. Installation
- Please refer to the NS-3 NR installation guide at [CTTC LENA NR](https://gitlab.com/cttc-lena/nr). Note that our NS-3 code is based on NS-3.37.

### 2. Configuration
- Run `./ns3 run "scratch/bullet-simulation.cc --PrintHelp"` to view the available simulation configurations.

### 3. Running BULLET
- Execute `./ns3 --run scratch/bullet-simulation.cc` to run the BULLET simulation according to the configurations.

### 4. Performance Evaluation
- `LatencyAnalyzer.ipynb` is a Python code to evaluate both application and link layer performance.

## Update Plan

We'll be refactoring the code to make it more user-friendly soon!

## Reference

We used the following projects as references:
- [NS3](https://github.com/nsnam/ns-3-dev-git)
- [NS3-NR](https://gitlab.com/cttc-lena/nr)
- [SparkRTC](https://github.com/hkust-spark/ns3-sparkrtc)
