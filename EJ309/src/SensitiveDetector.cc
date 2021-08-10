
#include "SensitiveDetector.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"
#include "SystemOfUnits.hh"
#include "MyHit.hh"

std::mutex SensitiveDetector::mtx;
std::map<G4int, HitsCollection*> SensitiveDetector::sdIDtoHC;

// Keeps track of the number of sensitive detectors created,
// which is essentially the number of events which have been ran.
// This is done as a hack to keep track of the current run number.
G4int SensitiveDetector::numberSDs = 0;

SensitiveDetector::SensitiveDetector(const G4String& name)
  : G4VSensitiveDetector(name) { }

SensitiveDetector::~SensitiveDetector() { }

// Updates the sensitive detector ID, as well as the number of
// sensitive detectors in the world.
void SensitiveDetector::Initialize(G4HCofThisEvent* HCE)
{
  mtx.lock();
  numberSDs += 1;
  sdID = numberSDs;
  hitsCollection = new HitsCollection();
  sdIDtoHC[sdID] = hitsCollection;
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

  MyHit* hit = new MyHit(step, sdID);

  sdIDtoHC[sdID]->addHit(hit);

  return true;

}
