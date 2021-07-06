// Author -- James McGreivy
// Date -- Jun 30th 2021

// A user-defined hit

#ifndef myHit_h
#define myHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "G4THitsCollection.hh"


class myHit : public G4VHit
{
public:
  // constructor
  myHit (G4int trackID, G4double eDep, G4String particle);
  ~myHit();
  
  // methods from base class
  G4String ToString();

  // Set methods
  void SetEventNum(G4int eventNum) { hEventNum = eventNum; };

  // Get methods
  G4int GetTrackID() const     { return hTrackID; };
  G4double GetEdep() const     { return hEdep; };

  private:
      G4int hEventNum;
      G4int hTrackID;
      G4double hEdep;
      G4String hParticle;
};

#endif