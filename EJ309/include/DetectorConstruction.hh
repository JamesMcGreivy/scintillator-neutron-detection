// Author -- James McGreivy
// Date -- Jun 26th 2021

#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

// The parent class for this user-defined detector geometry
#include "G4VUserDetectorConstruction.hh"

// Needed G4 Classes
#include "globals.hh"
#include "SystemOfUnits.hh"
#include "G4VPhysicalVolume.hh";
#include "G4LogicalVolume.hh";
#include "G4NistManager.hh";
#include "G4Material.hh";



// Detector construction class which defines materials and geometry
class DetectorConstruction : public G4VUserDetectorConstruction
{

public:

  DetectorConstruction();

  // All the static parts of the geometry should be defined here
  virtual G4VPhysicalVolume* Construct();
  
  // Helper methods for Construct
  void DefineMaterials();
  G4VPhysicalVolume* DefineGeometry();

  // All the dynamic parts of the geometry, such as magnetic fields 
  // or sensitive detectors must be defined in here. 
  virtual void ConstructSDandField();

private:
  // Materials used in detector
  G4Material* ej309;
  G4Material* aluminum;
  G4Material* air;

  // Helpful database of premade real-world materials
  G4NistManager* nist;

};

#endif