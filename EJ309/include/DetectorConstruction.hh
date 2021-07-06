// Author -- James McGreivy
// Date -- Jun 26th 2021

#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

// The parent class for this user-defined detector geometry
#include "G4VUserDetectorConstruction.hh"

// Needed G4 Classes
#include "globals.hh"
#include "SystemOfUnits.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;
class G4NistManager;
class G4Material;



// Detector construction class which defines materials and geometry
class DetectorConstruction : public G4VUserDetectorConstruction
{

public:

  DetectorConstruction();

  virtual G4VPhysicalVolume* Construct();

  virtual void ConstructSDandField();

  void DefineMaterials();

  G4VPhysicalVolume* DefineGeometry();

private:

  G4Material* ej309;
  G4Material* aluminum;
  G4Material* air;

  G4NistManager* nist;

};

#endif