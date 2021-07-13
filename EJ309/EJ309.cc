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
#include "myHit.hh"
#include "HitsCollection.hh"

int main(int argc, char *argv[]) 
{

	// Constructs the run manager (this should be multithreaded?)
	//auto* runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::MT);

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

	// Initializes the Run
	UImanager->ApplyCommand("/run/numberOfThreads 4");
	runManager->Initialize();

	// Sets up the Particle Source
	UImanager->ApplyCommand("/gps/ang/type iso");
	UImanager->ApplyCommand("/gps/position 0 0 -10 cm");
	UImanager->ApplyCommand("/gps/ang/mintheta 180 deg");
	UImanager->ApplyCommand("/gps/ang/maxtheta 202 deg");
	UImanager->ApplyCommand("/gps/ene/type Mono");

/*
	// For visualization
	UImanager->ApplyCommand("/gps/number 1");
	UImanager->ApplyCommand("/gps/particle neutron");
	UImanager->ApplyCommand("/gps/energy 0.2 MeV");
	ui->SessionStart();
*/

	G4String particle;
	G4String energy;
	G4String unit;
	G4String number;

	// The input file, containing all runs to perform
	std::ifstream file("input.txt");

	// Run a simulation for each line in the file
    while(file >> particle)
    {
    	file >> energy;
    	file >> unit;
    	file >> number;

    	G4cout << "Particle : " << particle
    		   << " , Energy : " << energy
    		   << " , Unit : " << unit
    		   << " , Number : " << number << "\n";

        // User input
		UImanager->ApplyCommand("/gps/number 100");
		UImanager->ApplyCommand("/gps/particle " + particle);
		UImanager->ApplyCommand("/gps/energy " + energy + " " + unit);

		// Sets the path for the sensitive detector output
		// Need to call before run because otherwise nullptr
		SensitiveDetector::OpenFile("data/" + particle 
								 		+ "_" + energy 
								 		+ "_" + unit + ".csv");
		
		// Runs the simulation
		runManager->BeamOn( std::atoi(number) / 100.0 );

		// Properly closes the output file
		SensitiveDetector::outputFile->close();

	}

	file.close();

	// W/out str opts -> 23 s
	// W str opts -> 

	delete runManager;
	return 0;
}