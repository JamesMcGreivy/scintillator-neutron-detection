// Author -- James McGreivy
// Date -- Jun 30th 2021

#ifndef SensitiveDetector_h
#define SensitiveDetector_h 1

#include "G4VSensitiveDetector.hh"

class G4Step;
class G4HCofThisEvent;

class SensitiveDetector : public G4VSensitiveDetector
{

public:
	SensitiveDetector(const G4String& name);
    ~SensitiveDetector();

    // Called at the end of every event
    virtual void EndOfEvent(G4HCofThisEvent* HCE);
    // Called at the beginning of every event 
    virtual void Initialize(G4HCofThisEvent* HCE);
    // Called any time a hit is created within the SD
    virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* history);

    // Unique identifier for each sensitive detector
    G4int sdID;

    // For thread-safe writing to file
    static std::mutex mtx;
    
    // Tracks the number of existing sensitive detectors
    static int numberSDs;
    
    // For writing hits to a file
    static std::ofstream* outputFile;
    
    // Sets the file for all sensitive detectors to write to
    // and creates the column headers
    // NOTE : This method MUST be called before using the sensitive detector
    // beause otherwise the outputFile pointer points to nothing
    static void OpenFile(G4String filePath)
    {
        outputFile = new std::ofstream(filePath, std::ofstream::out | std::ofstream::trunc);
        (*outputFile) << "trackID,parentID,pType,eDep(eV),materialName,currKE(eV),currTime\n";
    }

};

#endif