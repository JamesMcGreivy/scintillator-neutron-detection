// Author -- James McGreivy
// Date -- Jun 30th 2021

// A user-defined hit

#include "myHit.hh"
#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "SystemOfUnits.hh"

myHit::myHit(G4int trackID, G4double eDep, G4String particle, 
			 G4int parentID, G4String material, G4double currKE, 
			 G4double currTime)
{
	hTrackID = trackID;
	hEdep = eDep;
	hParticle = particle;
	hParentID = parentID;
	hMaterial = material;
	hCurrKE = currKE;
	hCurrTime = currTime;
}

myHit::~myHit() { }

G4String myHit::ToString() {
	G4String str = std::to_string(hTrackID)  + "," 
				 + std::to_string(hParentID) + ","
				 + hParticle 				 + ","
				 + std::to_string(hEdep)     + ","
				 + hMaterial 				 + ","
				 + std::to_string(hCurrKE)   + ","
				 + std::to_string(hCurrTime);
				 

	return str;
}
