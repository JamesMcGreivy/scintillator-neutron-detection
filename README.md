# Scintillator-Neutron-Detection

This README file is organized as follows:

2. Discussion of the design descisions that went into this simulation.

3. Ideas for future improvement of simulation code and verification of results.


## 1 - Compiling and Running the Geant4 Code

This project uses cmake in order to compile the Geant4 simulation:

$ source <GEANT4_INSTALL_DIR>/bin/geant4.sh
$ cd EJ309-build
$ cmake ../EJ309
$ make

In order to run simulations, create an input .txt file with the following structure:

\[ ptcl type ] \[ energy ] \[ unit ] \[ # of ptcls ]
\[ ptcl type ] \[ energy ] \[ unit ] \[ # of ptcls ]
...

where each row will correspond to a separate simulation. To run these simulations, type

$ ./EJ309 \[# Cores] \[input_file.txt]

The output data for each simulation specified in the input file will be written to the "/EJ309-build/data/" directory.

## 2 - Design Decisions in the Geant4 Code

The detector geometry, specified in the "DetectorConstruction.cc", defines a 2 inch diameter by 2 inch height cylinder of EJ309, surrounded by a 0.05 inch thick aluminum sleeve. The EJ309 volume also has the funcitonality of a sensitive detector, so that any hits generated within the volume will be stored in a HitsCollection object.

When written from the HitsCollection to a file, the simulation data will have the following CSV format:

\[ ptcl ID ],\[ parent ID ],\[ ptcl type ],\[ energy deposited along step (in eV)],\[ material which hit was generated in ]
\[ ptcl ID ],\[ parent ID ],\[ ptcl type ],\[ energy deposited along step (in eV)],\[ material which hit was generated in ]
...

The ID of the ptcl and its parent has the format \[ run # - ptcl # ]. This is because in order to get full multithreading functionality in Geant4, a single simulation must be broken into multiple runs. For this reason, the actual total number of particles simulated will always be rounded down to a multiple of the number of threads used.


