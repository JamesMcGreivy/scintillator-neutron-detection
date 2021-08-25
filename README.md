# Scintillator-Neutron-Detection

This README file is organized as follows:

1. Description of how to compile and run the simulation code, 
   and how train the Neural Network.

2. Discussion of the design descisions that went into this simulation.

3. Ideas for future improvement of simulation code and verification of results.


## 1 - Compiling the Code

This project uses cmake in order to compile the Geant4 simulation:

$ source <GEANT4_INSTALL_DIR>/bin/geant4.sh
$ cd EJ309-build
$ cmake ../EJ309
$ make

In order to run simulations, create a .txt file with the following structure:

\[ ptcl type ] \[ energy ] \[ unit ] \[ # of ptcls ]
