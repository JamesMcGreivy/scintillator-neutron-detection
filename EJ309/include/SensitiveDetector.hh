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
    ~SensitiveDetector();

    virtual void EndOfEvent(G4HCofThisEvent* HCE);
    virtual void Initialize(G4HCofThisEvent* HCE);
    virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* history);

    // Unique identifier for each sensitive detector
    G4int sdID;

    // For thread-safe locking
    static std::mutex mtx;
    // Tracks the number of existing sensitive detectors
    static int numberSDs;
    // For writing hits to a file
    static std::ofstream* outputFile;
    // Sets the file for all sensitive detectors to write to
    static void OpenFile(G4String filePath)
    {
        outputFile = new std::ofstream(filePath, std::ofstream::out | std::ofstream::trunc);
        (*outputFile) << "trackID,parentID,pType,eDep(eV),materialName,currKE(eV),currTime\n";
    }

};

#endif