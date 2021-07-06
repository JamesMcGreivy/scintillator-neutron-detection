
#include "SensitiveDetector.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"
#include "myHit.hh"
#include "SystemOfUnits.hh"
#include "HitsCollection.hh"

SensitiveDetector::SensitiveDetector(const G4String& name)
  : G4VSensitiveDetector(name) { }

SensitiveDetector::~SensitiveDetector()
{

}

G4bool SensitiveDetector::ProcessHits(G4Step* step, G4TouchableHistory* history)
{
  
  G4String particle = step->GetTrack()->GetParticleDefinition()->GetParticleName();
  G4int trackID = step->GetTrack()->GetTrackID();
  G4double eDep = step->GetTotalEnergyDeposit();

  myHit* hit = new myHit(trackID, eDep, particle);
  HitsCollection::AddHit(hit);
  return true;

}