// Author -- James McGreivy
// Date -- Jun 30th 2021

#ifndef SensitiveDetector_h
#define SensitiveDetector_h 1

#include "G4VSensitiveDetector.hh"
#include "HitsCollection.hh"
#include "MyHit.hh"

class G4Step;
class G4HCofThisEvent;

class SensitiveDetector : public G4VSensitiveDetector
{

public:
	SensitiveDetector(const G4String& name);

    // Called at the end of every event
    virtual void EndOfEvent(G4HCofThisEvent* HCE);
    // Called at the beginning of every event 
    virtual void Initialize(G4HCofThisEvent* HCE);
    // Called any time a hit is created within the SD
    virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* history);

    // Unique identifier for each sensitive detector
    G4int sdID;

    // Sensitive-detector specific hits collection
    HitsCollection* hitsCollection;

    // For thread-safe functions
    static std::mutex mtx;
    
    // Tracks the number of existing sensitive detectors
    static G4int numberSDs;

    // For storing the data from each thread's sensitive detector
    // maps a sensitive detector ID to a hit collection
    static std::map<G4int, HitsCollection*> sdIDtoHC;
    
    // Writes all sensitive detector data to a file
    // and clears the logged data
    static void DumpToFile(G4String filePath)
    {
        mtx.lock();
        std::ofstream outputFile;
        outputFile.open(filePath, std::ofstream::out | std::ofstream::trunc);
        outputFile << "trackID,parentID,pType,eDep(eV),materialName\n";

        for (const auto& kv : sdIDtoHC)
        {
            HitsCollection* HC = kv.second;
            for (MyHit* hit : *HC)
            {
                outputFile << hit->getOutput();
            }
        }


        sdIDtoHC.clear();
        numberSDs = 0;
        mtx.unlock();
    }
};

#endif