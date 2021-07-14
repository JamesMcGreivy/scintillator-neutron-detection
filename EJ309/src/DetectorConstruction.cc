// Author -- James McGreivy
// Date -- Jun 26th 2021

#include "DetectorConstruction.hh"

// NIST material manager, helps to define materials
#include "G4NistManager.hh"

// Some default shapes
#include "G4Box.hh"
#include "G4Tubs.hh"

// Important Classes
#include "G4LogicalVolume.hh"
#include "SystemOfUnits.hh"
#include "G4PVPlacement.hh"
#include "G4SDManager.hh"
#include "SensitiveDetector.hh"
#include "G4Element.hh"
#include "G4Material.hh"
#include "G4UserLimits.hh"

// Constructor just initializes the NIST material manager
DetectorConstruction::DetectorConstruction() 
  : G4VUserDetectorConstruction() 
{ 
  // NIST materials manager
  nist = G4NistManager::Instance();

}

// Required by G4 to construct the world
// Must return the world volume
G4VPhysicalVolume* DetectorConstruction::Construct()
{
  DefineMaterials();
  return DefineGeometry();
}

// Defines the materials to be used in the detector
void DetectorConstruction::DefineMaterials() 
{

  air = nist->FindOrBuildMaterial("G4_AIR");

  aluminum = nist->FindOrBuildMaterial("G4_Al");

  G4Element* elH = new G4Element("Hydrogen", "H", 1., 1.01*g/mole);
  G4Element* elC = new G4Element("Carbon", "C", 6., 12.01*g/mole);

  ej309 = new G4Material("ej309", 0.959*g/cm3, 2);
  ej309->AddElement(elH, 543);
  ej309->AddElement(elC, 435);


}

// Builds the geometry of the detector, returning the physical world volume
G4VPhysicalVolume* DetectorConstruction::DefineGeometry()
{

  // Check if volumes overlap
  G4bool checkOverlaps = true;

  // ~~ Defines the world volume ~~ //
  
  G4double worldXYZ = 10*inch;

  G4Box* solidWorld =
    new G4Box("world", worldXYZ, worldXYZ, worldXYZ);

  G4LogicalVolume* logicWorld =
    new G4LogicalVolume(solidWorld, air, "world");

  G4VPhysicalVolume* physWorld =
    new G4PVPlacement(0,                  // rotation
                      G4ThreeVector(),    // at (0,0,0)
                      logicWorld,         // logical volume
                      "world",            // name
                      0,                  // mother
                      false,              // ?, just make false
                      0,                  // copy number
                      checkOverlaps);     // overlaps checking

  // ~~ Defines the Scintillator (EJ 309) ~~ //

  G4Tubs* solidEJ309
    = new G4Tubs("ej309",     // Name
                 0.0*inch,    // Inner Radius 
                 1.0*inch,    // Outer Radius
                 1.0*inch,    // Height
                 0.0*deg,     // Starting Angle
                 360.0*deg);  // Spanning Angle

  G4LogicalVolume* logicEJ309 =
    new G4LogicalVolume(solidEJ309, ej309, "ej309");

  G4VPhysicalVolume* physEJ309 =
    new G4PVPlacement(0,
                      G4ThreeVector(0,0,0),
                      logicEJ309,
                      "ej309",
                      logicWorld,
                      false,
                      1,
                      checkOverlaps);

  G4UserLimits* stepLimit = new G4UserLimits(DBL_MAX, DBL_MAX, DBL_MAX, 0.0, 0.0);
  logicEJ309->SetUserLimits(stepLimit);

  // ~~ Defines an aluminum sleeve surrounding the scintillator ~~ //

  G4Tubs* solidAlSleeve
    = new G4Tubs("sleeve",    // Name
                 1.0*inch,    // Inner Radius 
                 1.05*inch,    // Outer Radius
                 1.0*inch,    // Half-Height
                 0.0*deg,     // Starting Angle
                 360.0*deg);  // Spanning Angle

  G4LogicalVolume* logicAlSleeve =
    new G4LogicalVolume(solidAlSleeve, aluminum, "sleeve");

  G4VPhysicalVolume* physAlSleeve =
    new G4PVPlacement(0,
                      G4ThreeVector(0,0,0),
                      logicAlSleeve,
                      "sleeve",
                      logicWorld,
                      false,
                      2,
                      checkOverlaps);

  // ~~ Defines two aluminum caps, so that the scintillator is fully enclosed ~~ //
  
  G4Tubs* solidAlCap
    = new G4Tubs("cap",    // Name
                 0.0*inch,    // Inner Radius 
                 1.05*inch,    // Outer Radius
                 0.025*inch,    // Half-Height
                 0.0*deg,     // Starting Angle
                 360.0*deg);  // Spanning Angle

  G4LogicalVolume* logicAlCap1=
    new G4LogicalVolume(solidAlCap, aluminum, "cap1");

  G4VPhysicalVolume* physAlCap1 =
    new G4PVPlacement(0,
                      G4ThreeVector(0, 0, -1.025*inch),
                      logicAlCap1,
                      "cap1",
                      logicWorld,
                      false,
                      2,
                      checkOverlaps);

  G4LogicalVolume* logicAlCap2=
    new G4LogicalVolume(solidAlCap, aluminum, "cap2");

  G4VPhysicalVolume* physAlCap2 =
    new G4PVPlacement(0,
                      G4ThreeVector(0, 0, 1.025*inch),
                      logicAlCap2,
                      "cap2",
                      logicWorld,
                      false,
                      2,
                      checkOverlaps);

  
  // Returns the world
  return physWorld;

}


// Constructs all sensitive detectors and assigns them to their logical volumes
void DetectorConstruction::ConstructSDandField()
{

  SensitiveDetector* ej309SD = new SensitiveDetector("ej309");

  G4SDManager::GetSDMpointer()->AddNewDetector(ej309SD);
  
  SetSensitiveDetector("ej309", ej309SD);

  SensitiveDetector* alSD = new SensitiveDetector("Al");

  G4SDManager::GetSDMpointer()->AddNewDetector(alSD);
  
  SetSensitiveDetector("cap1", alSD);
  SetSensitiveDetector("cap2", alSD);
  SetSensitiveDetector("sleeve", alSD);


}





