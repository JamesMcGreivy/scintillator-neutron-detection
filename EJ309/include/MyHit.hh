// Author -- James McGreivy
// Date -- Jun 30th 2021

#include "SensitiveDetector.hh"

#ifndef MyHit_h
#define MyHit_h 1

class MyHit
{

public:
	MyHit(G4Step* step, G4int sdID);
    
    ~MyHit();

    G4String getOutput();

private:
    G4String output;

};

#endif