
#include "SensitiveDetector.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"
#include "SystemOfUnits.hh"

std::ofstream* SensitiveDetector::outputFile;
std::mutex SensitiveDetector::mtx;

// Keeps track of the number of sensitive detectors created,
// which is essentially the number of events which have been ran.
// This is done as a hack to keep track of the current run number.
int SensitiveDetector::numberSDs = 0;

SensitiveDetector::SensitiveDetector(const G4String& name)
  : G4VSensitiveDetector(name) { }

SensitiveDetector::~SensitiveDetector() { }

// Updates the sensitive detector ID, as well as the number of
// sensitive detectors in the world.
void SensitiveDetector::Initialize(G4HCofThisEvent* HCE)
{
  G4cout << "INITIALIZE CALLED ??????" << G4endl;
  mtx.lock();
  sdID = numberSDs + 1;
  numberSDs += 1;
  mtx.unlock();
}

// Sets the sdID to an unused value, mainly to help with
// debugging. If sdID = -1 in the output, there is an issue.
void SensitiveDetector::EndOfEvent(G4HCofThisEvent* HCE)
{
  sdID = -1;
}

// Called every single time a hit occurs within this volume.
// Extracts relevant information and writes it to outputFile
G4bool SensitiveDetector::ProcessHits(G4Step* step, G4TouchableHistory* history)
{
  // Relevant information
  G4int parentID = step->GetTrack()->GetParentID();
  G4String particle = step->GetTrack()->GetParticleDefinition()->GetParticleName();
  G4String material = step->GetTrack()->GetMaterial()->GetName();
  G4int trackID = step->GetTrack()->GetTrackID();
  G4double eDep = step->GetTotalEnergyDeposit() / eV;
  G4double currKE = step->GetPreStepPoint()->GetKineticEnergy() / eV;
  G4double currTime = step->GetPreStepPoint()->GetGlobalTime();

  // Formats the output data
  G4String output = std::to_string(sdID) + "-" + std::to_string(trackID)  + "," 
                  + std::to_string(sdID) + "-" + std::to_string(parentID) + ","
                  + particle         + ","
                  + std::to_string(eDep)     + ","
                  + material         + ","
                  + std::to_string(currKE)   + ","
                  + std::to_string(currTime) + "\n";

  // Must lock with mutex so that multiple threads cannot write at the same time
  mtx.lock();
  (*outputFile) << output;
  mtx.unlock();

  return true;

}
