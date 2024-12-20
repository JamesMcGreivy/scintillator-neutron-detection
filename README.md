## This is the repository for the paper "[Performance of Simulated Detector Responses in Training Neural Networks for Neutron Spectrum Unfolding](https://www.osti.gov/servlets/purl/1863681)". 

This research leverages machine learning to unfold neutron energy spectra from EJ-309 organic scintillator detector responses. Accurate neutron energy spectrum measurements are critical for applications such as radiation monitoring, medical imaging, nuclear fusion / fission tracking, and detecting illicit nuclear materials. Below is a demonstration of the unfolding neural network, trained only on Monte Carlo simulated detector responses, applied to the unfolding of two real world detector responses. These detector responses were obtained from measurements made of AmBe241 and Cf252 sources. The known neutron energy spectra are plotted below:

<img width="400" alt="spectra" src="https://github.com/JamesMcGreivy/neutron-scintillator-simulation/blob/master/unfolding-neural-network/plots/Spectra.png">

The experimentally determined detector responses are compared to the Monte Carlo simulated detector responses below:

<img width="200" alt="spectra" src="https://github.com/JamesMcGreivy/neutron-scintillator-simulation/blob/master/unfolding-neural-network/plots/AmBeResponse.png"><img width="200" alt="spectra" src="https://github.com/JamesMcGreivy/neutron-scintillator-simulation/blob/master/unfolding-neural-network/plots/CFResponse.png">

The accuracy of the neutron spectrum unfolding neural network on real-world experimental detector response data is demonstrated below:

<img width="200" alt="spectra" src="https://github.com/JamesMcGreivy/neutron-scintillator-simulation/blob/master/unfolding-neural-network/plots/AmBeUnfolded.png"><img width="200" alt="spectra" src="https://github.com/JamesMcGreivy/neutron-scintillator-simulation/blob/master/unfolding-neural-network/plots/CFUnfolded.png">

## 1 - Compiling and Running Monte Carlo Simulations

This project uses cmake in order to compile the Geant4 simulation:

    $ source <GEANT4_INSTALL_DIR>/bin/geant4.sh
    $ cd EJ309-build
    $ cmake ../EJ309
    $ make
To run simulations you must provide an input text file, where each row provides parameters to a separate simulation. An example file, example_input.txt, is provided for reference. To run these simulations, type

    $ ./EJ309 [# Cores] [example_input.txt]

The output data for each simulation specified in the input file will be written to the "EJ309-build/data/" directory. The detector geometry, specified in the "DetectorConstruction.cc", defines a 2 inch diameter by 2 inch height cylinder of EJ309, surrounded by a 0.05 inch thick aluminum sleeve. The EJ309 volume also has the funcitonality of a sensitive detector, so that any hits generated within the volume will be stored in a HitsCollection object.

When written from the HitsCollection to a file, the simulation data will have the following CSV format:

    [ ptcl ID ],[ parent ID ],[ ptcl type ],[ energy deposited along step (in eV)],[ material in which hit was generated ]
    [ ptcl ID ],[ parent ID ],[ ptcl type ],[ energy deposited (in eV)],[ material in which hit was generated ]
    ...

## 3 - Light Yield Analysis

To run the light yield analysis, just navigate to the simulate-light-yield directory and run SimulateLightYield.py. The constants DATA_DIR OUTPUT_DIR BINS and NUM_CORES must be set accordingly.

In a real-world scintillator detector setup the energy deposited into the scintillator by recoiling protons and carbon ions will generate light. Scintillation light entering a photomultiplier tube is converted into an electrical pulse, which is then placed into buckets based upon the electrical pulse height or integral.

One decision I had to make in analyzing the G4 data is how light producing events should be counted in the pulse height spectrum. A back of the envelope calculation for a 20 MeV neutron in a scintillator with a mean free path of ~ 10 cm should yield a time of 1 or 2 ns between collision events. As the neutron slows due to multiple collisions this collision time will increase. According to https://eljentechnology.com/, the decay constant for a light pulse from EJ309 is ~ 3.5 ns. While analyzing the data I make the assumption that the time resolution of the photomultiplier tube and the light decay time of EJ309 will cause multiple light production events from one single neutron to be smeared into one large electrical pulse.

In order to keep track of which recoiling protons and carbon ions are the result of a specific incoming neutron, I use a tree structure. The recursive process used to calculate a single entry in the detector response spectrum is:

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

## Training Data for the Neural Network

The code is written to be modular, and so the specifics of the number of output or input neurons can be changed very easily, as well as the minimum and maximum detector response cutoff. For my testing I used an input layer of 50 neurons, an output layer of 100 neurons, a minimum detector response cutoff of 0.3 477-keVee, and a maximum detector response cutoff of 30 477-keVee. 

The data which the neural network is trained on takes the form:

-> x-data: The detector response from a neutron source with a given energy spectra (from the y-data), in the range of 0.3 477-keVee to 30 477-keVee, normalized so the integral over this range is one. 50 evenly spaced points are taken in this range and used as the input data.
    
-> y-data: The energy spectra of the neutron source, ranging from 1 keV to 20,000 keV, normalized so the integraal over this range is one. 100 evenly spaced points are taken in this range and used as the expected output data.
    
There are many ways to generate this training data, however the method I have chosen is:

-> Perform 400 simulations of monoenergetic neutron sources, with energies spaced linearly in the range of 1 keV to 20,000 keV. All of these simulations should have the exact same number of primary particles. Run the light yield analysis script on these simulations in order to get a list of light yield values for the primary particles in each of these 400 simulations.
    
-> For each simulation, place the light yield values into 50 linearly-spaced buckets in the range 0.3 477-keVee to 30 477-keVee (ignoring light yields outside of this range) so that we have a 50-bucket un-normalized detector response function. Each bucket in this response function is the raw number of counts of primary neutrons which generated light in that eVee range. I have not proven this, however testing seems to imply that linear combinations of these monoenergetic detector response functions can be used to infer the detector response of a neutron source with the same linear combination of energies (i.e. I can calculate 0.3 * { 2 keV DR } + 0.7 * { 10 keV DR } to get the detector response of a neutron source with a 30% chance of generating a 2 keV neutron and a 70% chance of generating a 10 keV neutron). I perform this linear combination BEFORE normalization, because normalization would remove any information about the proportion of neutrons at that energy which generate no light in the detector.
    
-> Generate a random discrete energy spectra with 400 buckets in the range of 1 keV to 20,000 keV. The mid-points of these buckets should have the same values as the 400 mono-energetic neutron sources simulated. The spectra is normalized so that the values of the buckets sum to one. This way, the value in a specific bucket is the proportion of particles generated in this hypothetical neutron source which leave with an energy value somewhere in that bucket.

-> Use this discrete energy spectra to perform a linear combination of the 400 mono-energetic detector responses, weighted by the proportion given in the energy spectra. This should give a good approximation of the detector response that a neutron source with this energy spectra would yield.

-> Interpolate the 400-bucket energy spectra down to 100-buckets, and normalize it so it integrates to one. This becomes the y-data. Normalize the calculated detector response function so that it also integrates to one. This becomes the x-data.
