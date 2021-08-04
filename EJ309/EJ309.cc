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
	if ( argc != 5 && argc != 2 )
	{
		G4cout << "Please Provide Valid Command Line Args : " << G4endl;
		G4cout << "\t./EJ309 [ptcl] [energy] [unit] [#]" << G4endl;
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
	#ifdef G4MULTITHREADED
	G4MTRunManager* runManager = new G4MTRunManager;
	G4cout << "Multithreaded" << G4endl;
	#else
	G4RunManager* runManager = new G4RunManager;
	G4cout << "Single threaded" << G4endl;
	#endif

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

	// Constructs the ui
	G4UIExecutive* ui = new G4UIExecutive(argc, argv);

	// Initializes the Run. Set the number of threads accordingly.
	UImanager->ApplyCommand("/run/numberOfThreads 4");
	runManager->Initialize();

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
		SensitiveDetector::OpenFile("debug.csv");
		UImanager->ApplyCommand("/gps/number 1");
		UImanager->ApplyCommand("/gps/particle neutron");
		UImanager->ApplyCommand("/gps/energy 0.2 MeV");
		UImanager->ApplyCommand("/control/execute init_vis.mac");
		ui->SessionStart();
		delete runManager;
		return 0;
	}

	// ~~ Runs the simulation ~~ //
	G4String particle = argv[1];
    G4String energy = argv[2];
    G4String unit = argv[3];
    G4String number = argv[4];

    G4cout << "Particle : " << particle
    	   << " , Energy : " << energy
    	   << " , Unit : " << unit
    	   << " , Number : " << number << "\n";

    // Sets the particle source according to user input
    // Performs 500 particles per run, since multithreading
    // only works across seperate events
	UImanager->ApplyCommand("/gps/number 500");
	UImanager->ApplyCommand("/gps/particle " + particle);
	UImanager->ApplyCommand("/gps/energy " + energy + " " + unit);

	// Sets the path for the sensitive detector output
	// NEED TO CALL somewhere before run because otherwise nullptr
	SensitiveDetector::OpenFile("EJ309-build/data/" + particle 
								 		+ "_" + energy 
								 		+ "_" + unit + ".csv");
		
	// Runs the simulation
	runManager->BeamOn( std::atoi(number) / 500 );

	// Properly closes the output file
	SensitiveDetector::outputFile->close();

	delete runManager;
	return 0;
}