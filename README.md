## 1 - Compiling and Running the Geant4 Code

This project uses cmake in order to compile the Geant4 simulation:

    $ source <GEANT4_INSTALL_DIR>/bin/geant4.sh
    $ cd EJ309-build
    $ cmake ../EJ309
    $ make

In order to run simulations, create an input .txt file with the following structure:

    [ ptcl type ] [ energy ] [ unit ] [ # of ptcls ]
    [ ptcl type ] [ energy ] [ unit ] [ # of ptcls ]
    ...

where each row will correspond to a separate simulation. To run these simulations, type

    $ ./EJ309 [# Cores] [input_file.txt]

The output data for each simulation specified in the input file will be written to the "EJ309-build/data/" directory.

## 2 - Design of the Geant4 Code

The detector geometry, specified in the "DetectorConstruction.cc", defines a 2 inch diameter by 2 inch height cylinder of EJ309, surrounded by a 0.05 inch thick aluminum sleeve. The EJ309 volume also has the funcitonality of a sensitive detector, so that any hits generated within the volume will be stored in a HitsCollection object.

When written from the HitsCollection to a file, the simulation data will have the following CSV format:

    [ ptcl ID ],[ parent ID ],[ ptcl type ],[ energy deposited along step (in eV)],[ material in which hit was generated ]
    [ ptcl ID ],[ parent ID ],[ ptcl type ],[ energy deposited (in eV)],[ material in which hit was generated ]
    ...

The ID of the ptcl and its parent has the format \[ run # - ptcl # ]. This is because in order to get full multithreading functionality in Geant4, a single simulation must be broken into multiple runs. For this reason, the actual number of particles simulated will always be rounded down to a multiple of the number of threads used.

## 3 - Running the Light Yield Analysis

To run the light yield analysis, just navigate to the G4DataToLY directory and run G4DataToLY.py. The constants DATADIR OUTPUTDIR BINS and NUMCORES, described in the python file, should be set accordingly.

## 4 - Design of the Light Yield Analysis

In a real-world scintillator detector setup the energy deposited into the scintillator by recoiling protons and carbon ions will generate light. Scintillation light entering a photomultiplier tube is converted into an electrical pulse, which is then placed into buckets based upon the electrical pulse height or integral.

One decision I had to make in analyzing the G4 data is how light producing events should be counted in the pulse height spectrum. A back of the envelope calculation for a 20 MeV neutron in a scintillator with a mean free path of ~ 10 cm should yield a time of 1 or 2 ns between collision events. As the neutron slows due to multiple collisions this collision time will increase. According to https://eljentechnology.com/, the decay constant for a light pulse from EJ309 is ~ 3.5 ns. While analyzing the data I make the assumption that the time resolution of the photomultiplier tube and the light decay time of EJ309 will cause multiple light production events from one single neutron to be smeared into one large electrical pulse.

In order to keep track of which recoiling protons and carbon ions are the result of a specific incoming neutron, I use a tree structure. The process used to calculate a single entry in the detector response spectrum is:

    protonLightYieldFunction = # Interpolated data from Laplace 2021
    carbonLightYieldFunction = # Interpolated data from Laplace 2021
    uncertaintyFunction = # From Dietze 1981
    
    getLY(ptcl):
     
      if ptcl is proton:
        lightYield = protonLightYieldFunction( total energy deposited )
      if ptcl is carbon:
        lightYield = carbonLightYieldFunction( total energy deposited )

      uncertainty = uncertaintyFunction( lightYield )
      lightYield += random.normal( mean = 0, stddev = uncertainty )

      for child of ptcl:
        lightYield += getLY(child)

This function is called on every primary neutron in the simulation, and the distribution of these calculated light yields is used as the detector response.

## Getting Training Data for the Neural Network

The neural network takes in 
