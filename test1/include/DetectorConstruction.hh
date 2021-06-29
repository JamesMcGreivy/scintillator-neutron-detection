// Author -- James McGreivy
// Date -- Jun 26th 2021


// The parent class for this user-defined detector geometry
#include "G4VUserDetectorConstruction.hh"

// Global variables
#include "globals.hh"


// Needed for making volumes (?)
class G4VPhysicalVolume;
class G4LogicalVolume;


// Detector construction class which defines materials and geometry
class DetectorConstruction : public G4VUserDetectorConstruction
{

public:

  DetectorConstruction();

  virtual G4VPhysicalVolume* Construct();

};