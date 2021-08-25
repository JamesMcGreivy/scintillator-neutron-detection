// Author - James McGreivy
// Date - Jun 26th 2021

// Includes the header files for the required user-defined geometry and physics :
#include "DetectorConstruction.hh"
#include "ActionInitialization.hh"
#include "PhysicsList.hh"

// Includes header files for important G4 claasses :
#include "G4RunManagerFactory.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
#include "G4ScoringManager.hh"
#include "G4VModularPhysicsList.hh"
#include "G4SDManager.hh"
#include "SensitiveDetector.hh"

int main(int argc, char *argv[]) 
{

	// Checks command-line args
	if ( argc != 2 && argc != 3 )
	{
		G4cout << "Please Provide Valid Command Line Args : " << G4endl;
		G4cout << "\t./EJ309 [# cores] [input file dir]" << G4endl;
		G4cout << "\t\tOR" << G4endl;
		G4cout << "\t./EJ309 vis (for debugging)" << G4endl;
		return 1;
	}

	// If vis is passed on the command line, opens up the UI
	// for debugging
	bool visualize = false;
	if ( argc == 2 and std::strcmp(argv[1], "vis") == 0 )
	{
		visualize = true;
	}

	// Constructs the run manager, checking if the G4 build
	// is multithreaded or not.
	G4MTRunManager* runManager = new G4MTRunManager;
	G4cout << "Multithreaded" << G4endl;

	// Gets the pointer to the UI manager
	G4UImanager* UImanager = G4UImanager::GetUIpointer();

	// ~~ Provides the mandatory initialization classes ~~ //
	
	// Detector construction :
	runManager->SetUserInitialization(new DetectorConstruction());
	
	// Physics List :
	runManager->SetUserInitialization(new PhysicsList());

	// User action initialization :
	runManager->SetUserInitialization(new ActionInitialization());

	// ~~ Initializes the visualization ~~ //
	G4VisManager* visManager = new G4VisExecutive();
	visManager->Initialize();

	// ~~ Sets up the Particle Source ~~ //

	// Isometric distribution, i.e. any angle is equally probable
	UImanager->ApplyCommand("/gps/ang/type iso");
	
	// Sets the particle distribution to contain only the detector. 
	// Prevents particles from being created which do not intersect the detector.
	// This was just determined visually
	UImanager->ApplyCommand("/gps/position 0 0 -10 cm");
	UImanager->ApplyCommand("/gps/ang/mintheta 180 deg");
	UImanager->ApplyCommand("/gps/ang/maxtheta 200 deg");
	UImanager->ApplyCommand("/gps/ene/type Mono");

	// For visualization, debugging
	if (visualize)
	{
		// Constructs the ui
		G4UIExecutive* ui = new G4UIExecutive(argc, argv);
	
		runManager->Initialize();	
		
		UImanager->ApplyCommand("/gps/number 1");
		UImanager->ApplyCommand("/gps/particle neutron");
		UImanager->ApplyCommand("/gps/energy 0.2 MeV");
		UImanager->ApplyCommand("/control/execute init_vis.mac");
		ui->SessionStart();
		delete runManager;
		SensitiveDetector::DumpToFile("debug.csv");
		return 0;
	}

	// Sets number of threads
	
	G4int numThreadsInt = std::stoi(argv[1]);
	G4String numThreadsString = argv[1];

	std::cout << "NUM THREADS : " << numThreadsString << std::endl;
	
	UImanager->ApplyCommand("/run/numberOfThreads " + numThreadsString);
	
	// Initializes the Run. Set the number of threads accordingly.
	runManager->Initialize();

	// ~~ Runs the simulation ~~ //
	G4String particle;
	G4String energy;
	G4String unit;
	G4String number;

	// The input file, containing all runs to perform
	G4String inputFile = argv[2];
	std::ifstream file(inputFile);

	// make a mutex for thread-safe file dumping
	std::mutex mtx;

	// Run a simulation for each line in the file
    while(file >> particle)
    {
	    file >> energy;
	    file >> unit;
	    file >> number;

	    G4cout 	<< "Particle : " << particle
	    		<< " , Energy : " << energy
	    		<< " , Unit : " << unit
	    		<< " , Number : " << number << "\n";

	    G4int ptclsPerThread = std::atoi(number) / numThreadsInt;

	    // User input
		UImanager->ApplyCommand("/gps/number " + std::to_string(ptclsPerThread));
		UImanager->ApplyCommand("/gps/particle " + particle);
		UImanager->ApplyCommand("/gps/energy " + energy + " " + unit);

		// Runs the simulation
		runManager->BeamOn(numThreadsInt);

		// Dump data to output file
		mtx.lock();
		SensitiveDetector::DumpToFile("data/" + particle 
					 						  + "_" + energy 
											  + "_" + unit + ".csv");
		mtx.unlock();
	}

	file.close();
	delete runManager;
	return 0;
}
