// Author -- James McGreivy
// Date -- Jun 30th 2021

// A user-defined hit

#include "myHit.hh"
#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "SystemOfUnits.hh"

myHit::myHit(G4int trackID, G4double eDep, G4String particle)
{
	hTrackID = trackID;
	hEdep = eDep;
	hParticle = particle;
	hEventNum = -1;
}

myHit::~myHit()
{
	delete hParticle;
}

G4String myHit::ToString() {
	G4String str = std::to_string(hEventNum) + "," 
				 + std::to_string(hTrackID)  + "," 
				 + std::to_string(hEdep / eV)	 + ","
				 + hParticle;
	return str;
}
