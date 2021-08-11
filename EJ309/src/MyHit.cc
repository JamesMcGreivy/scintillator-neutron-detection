// Author -- James McGreivy
// Date -- Jun 30th 2021

#include "MyHit.hh"
#include "SensitiveDetector.hh"
#include "SystemOfUnits.hh"

MyHit::MyHit(G4Step* step, G4int sdIDint)
{
	// Relevant information
  	G4String parentID = std::to_string( step->GetTrack()->GetParentID() );
  	G4String particle = step->GetTrack()->GetParticleDefinition()->GetParticleName();
  	G4String material = step->GetTrack()->GetMaterial()->GetName();
  	G4String trackID = std::to_string( step->GetTrack()->GetTrackID() );
  	G4String eDep = std::to_string( step->GetTotalEnergyDeposit() / eV );
  	G4String currKE = std::to_string( step->GetPreStepPoint()->GetKineticEnergy() / eV );
  	G4String currTime = std::to_string( step->GetPreStepPoint()->GetGlobalTime() );
  	G4String sdID = std::to_string( sdIDint );
	
	// Formats for output string
	output = sdID 	+ "-" + trackID  + "," + sdID + "-" + parentID + ","
                  	+ particle + "," + eDep + "," + material + "\n";
}

MyHit::~MyHit()
{

}

G4String MyHit::getOutput()
{
	return output;
}