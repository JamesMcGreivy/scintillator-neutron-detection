
#include "SensitiveDetector.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"
#include "myHit.hh"


SensitiveDetector::SensitiveDetector(const G4String& name)
  : G4VSensitiveDetector(name) { }

G4bool SensitiveDetector::ProcessHits(G4Step* step, G4TouchableHistory* history)
{  

  G4cout << "hit" << G4endl;

  return true;
}