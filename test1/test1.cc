// Author - James McGreivy
// Date - Jun 26th 2021

// Includes the header files for the required user-defined geometry and physics :
#include "DetectorConstruction.hh"
#include "ActionInitialization.hh"

// Includes header files for important G4 claasses :
#include "G4RunManagerFactory.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
#include "G4ScoringManager.hh"

// The premade physics list to use
#include "QGSP_BERT_HP.hh"


int main(int argc, char** argv) 
{

	// Constructs the ui
	G4UIExecutive* ui = new G4UIExecutive(argc, argv);

	// Constructs the run manager (this should be multithreaded?)
	auto* runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::Default);

	// Gets the predifined physics list named "QBBC" -- Good for high energy 
	G4VModularPhysicsList* physicsList = new QGSP_BERT_HP;

	//?
	G4ScoringManager* sm = G4ScoringManager::GetScoringManager();

	// ~~ Provides the mandatory initialization classes ~~ //
	
	// Detector construction :
	runManager->SetUserInitialization(new DetectorConstruction());
	
	// Physics List :
	runManager->SetUserInitialization(physicsList);

	// User action initialization :
	runManager->SetUserInitialization(new ActionInitialization());

	// ~~ Initializes the visualization ~~ //

	G4VisManager* visManager = new G4VisExecutive();
	visManager->Initialize();

	// Gets the pointer to the UI manager
	G4UImanager* UImanager = G4UImanager::GetUIpointer();

	// Starts the interactive session
	UImanager->ApplyCommand("/control/macroPath /Users/jamesmcgreivy/Desktop/NSSC Research/example1");
	UImanager->ApplyCommand("/control/execute init_vis.mac");
	ui->SessionStart();
	

	// Deletes everything : )
	delete ui;
	delete visManager;
	delete runManager;


	return 0;
}