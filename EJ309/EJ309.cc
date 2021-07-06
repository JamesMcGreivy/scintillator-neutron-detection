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

int main(int argc, char** argv) 
{

	// Constructs the ui
	G4UIExecutive* ui = new G4UIExecutive(argc, argv);

	// Constructs the run manager (this should be multithreaded?)
	auto* runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::Default);

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

	// Gets the pointer to the UI manager
	G4UImanager* UImanager = G4UImanager::GetUIpointer();

	// Starts the interactive session
	//UImanager->ApplyCommand("/run/initialize");
	//UImanager->ApplyCommand("/control/execute vis.mac");
	//ui->SessionStart();

	// Initializes the Run
	UImanager->ApplyCommand("/cuts/setLowEdge 1 eV");
	UImanager->ApplyCommand("/cuts/setHighEdge 10 TeV");
	runManager->Initialize();

	// Sets up the Particle Source
	UImanager->ApplyCommand("/gps/ang/type iso");
	UImanager->ApplyCommand("/gps/position 0 0 -10 cm");
	UImanager->ApplyCommand("/gps/ang/mintheta 160 deg");
	UImanager->ApplyCommand("/gps/ang/mintheta 200 deg");
	UImanager->ApplyCommand("/gps/ene/type Mono");
	UImanager->ApplyCommand("/gps/number 1000000");

	// Changes the gun to Gamma
	UImanager->ApplyCommand("/gps/particle gamma");
	UImanager->ApplyCommand("/gps/energy 662 keV");

	// Runs the first simulation (Gammas)
	runManager->BeamOn(1);

	// Changes the gun to Neutrons
	HitsCollection::EndEvent();
	UImanager->ApplyCommand("/gps/particle neutron");
	UImanager->ApplyCommand("/gps/energy 2 MeV");

	// Runs the second simulation (Neutrons)
	runManager->BeamOn(1);

	HitsCollection::ToFile("EnergyDeposited.csv");

	delete runManager;
	return 0;
}