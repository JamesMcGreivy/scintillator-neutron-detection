
// Author -- James McGreivy
// Date -- Jun 26th 2021


#include "DetectorConstruction.hh"

// NIST material manager, helps to define materials
#include "G4NistManager.hh"

// Some default shapes
#include "G4Box.hh"
#include "G4Orb.hh"

// Important Classes
#include "G4LogicalVolume.hh"
#include "G4SystemOfUnits.hh"
#include "G4PVPlacement.hh"
#include "G4SDManager.hh"
#include "SensitiveDetector.hh"

// Constructor, just calls parent constructor
DetectorConstruction::DetectorConstruction() 
  : G4VUserDetectorConstruction() { }


G4VPhysicalVolume* DetectorConstruction::Construct()
{

  // Check if volumes overlap
  G4bool checkOverlaps = true;

  
  // Instantiates the nist materials manager
  G4NistManager* nist = G4NistManager::Instance();


  // ~~ Defines the world ~~ //
  G4double worldXY = 100*cm;
  G4double worldZ = 100*cm;
  G4Material* worldMat = nist->FindOrBuildMaterial("G4_AIR");

  G4Box* worldBox =
    new G4Box("World", worldXY, worldXY, worldZ);

  G4LogicalVolume* logicWorld =
    new G4LogicalVolume(worldBox, worldMat, "World");

  G4VPhysicalVolume* world =
    new G4PVPlacement(0,                  // rotation
                      G4ThreeVector(),    // at (0,0,0)
                      logicWorld,         // logical volume
                      "World",            // name
                      0,                  // mother
                      false,              // ?, just make false
                      0,                  // copy number
                      checkOverlaps);     // overlaps checking


  // ~~ Puts a uranium box in the world ~~ //
  G4Material* detectorMat = nist->FindOrBuildMaterial("G4_GLASS_PLATE");

  // Defines the material of the walls
  G4Material* wallMat = nist->FindOrBuildMaterial("G4_BENZENE");

  G4Box* wall = 
    new G4Box("wall_solid", 60*cm, 60*cm, 10*cm);

  G4LogicalVolume* logicWall =
    new G4LogicalVolume(wall, wallMat, "wall_lv");

  G4VPhysicalVolume* physFirstWall =
    new G4PVPlacement(0,                  // rotation
                      G4ThreeVector(0*cm, 0*cm, 0*cm), // at (0,0,0)
                      logicWall,         // logical volume
                      "wall_pv",      // name
                      logicWorld,         // mother
                      false,              // ?, just make false
                      0,                  // copy number
                      checkOverlaps);     // overlaps checking

  G4Box* detector =
    new G4Box("detector_solid", 60*cm, 60*cm, 0.5*cm);

  G4LogicalVolume* logicDetector =
    new G4LogicalVolume(detector, detectorMat, "detector_lv");

  G4VPhysicalVolume* physDeteector =
    new G4PVPlacement(0,
                      G4ThreeVector(0*cm, 0*cm, 10.5*cm),
                      logicDetector,
                      "detector_pv",
                      logicWorld,
                      false,
                      0,
                      checkOverlaps);

  // Returns the physical world
  return world;

}


void DetectorConstruction::ConstructSDandField()
{

  SensitiveDetector* sensitiveBox = new SensitiveDetector("detector_sd");
    
  G4SDManager* SDManager = G4SDManager::GetSDMpointer();
  SDManager->AddNewDetector(sensitiveBox);
    
  SetSensitiveDetector("detector_lv", sensitiveBox, true);

}





