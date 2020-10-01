# OSADOCA real-time GNC algorithms
## Part of the OSADOCA Project
***O**rbital **S**imulator **a**nd **D**evelopment **o**f **C**ontrol **A**lgorithms*

Providing real-time algorithms in c, running on a RPI 3B+ using Xenomai 3 real-time kernel.

## Installation
```bash
git clone https://github.com/yves-marieRaiffaud/OSADOCA_Algorithms.git
```

## Building
In the root directory of the project, simply use the `make` command to create the Makefile:
```bash
make Makefile
```

## Running
* The first time, make sure that the built executable can be executed:
```bash
sudo chmod +x Makefile
```

* Run the file `main.c` with the command:
```bash
./main.c rpi_IP receivingPort sendingPort
```
where `rpi_IP` is the IP address of the Raspberry PI 3B+, `receivingPort` is the listenning port to receive telemetry from the Unity3D OSADOCA project, and `sendingPort` is the sending port to send telecommands to the Unity3D OSODOCA Project.