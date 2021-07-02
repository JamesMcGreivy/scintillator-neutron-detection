// Author -- James McGreivy
// Date -- Jun 30th 2021

// A user-defined sensitive detector class for detecting protons (?)

#ifndef SensitiveDetector_h
#define SensitiveDetector_h 1

#include "G4VSensitiveDetector.hh"
#include "myHit.hh"

class G4Step;
class G4HCofThisEvent;

class SensitiveDetector : public G4VSensitiveDetector
{

public:
	SensitiveDetector(const G4String& name);

    virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* history);

};

#endif