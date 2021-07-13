
#include "SensitiveDetector.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"
#include "myHit.hh"
#include "SystemOfUnits.hh"
#include "HitsCollection.hh"

std::ofstream* SensitiveDetector::outputFile;
std::mutex SensitiveDetector::mtx;
int SensitiveDetector::numberSDs = 0;

SensitiveDetector::SensitiveDetector(const G4String& name)
  : G4VSensitiveDetector(name) { }

SensitiveDetector::~SensitiveDetector() { }

void SensitiveDetector::Initialize(G4HCofThisEvent* HCE)
{
  mtx.lock();
  sdID = numberSDs + 1;
  numberSDs += 1;
  mtx.unlock();
}

void SensitiveDetector::EndOfEvent(G4HCofThisEvent* HCE)
{
  sdID = -1;
}

G4bool SensitiveDetector::ProcessHits(G4Step* step, G4TouchableHistory* history)
{

  G4int parentID = step->GetTrack()->GetParentID();
  G4String particle = step->GetTrack()->GetParticleDefinition()->GetParticleName();
  G4String material = step->GetTrack()->GetMaterial()->GetName();
  G4int trackID = step->GetTrack()->GetTrackID();
  G4double eDep = step->GetTotalEnergyDeposit() / eV;
  G4double currKE = step->GetPreStepPoint()->GetKineticEnergy() / eV;
  G4double currTime = step->GetPreStepPoint()->GetGlobalTime();

  G4String output = std::to_string(sdID) + "-" + std::to_string(trackID)  + "," 
                  + std::to_string(sdID) + "-" + std::to_string(parentID) + ","
                  + particle         + ","
                  + std::to_string(eDep)     + ","
                  + material         + ","
                  + std::to_string(currKE)   + ","
                  + std::to_string(currTime) + "\n";

  mtx.lock();
  (*outputFile) << output;
  mtx.unlock();

  return true;

}
