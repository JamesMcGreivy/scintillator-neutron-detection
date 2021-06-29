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
  G4Material* uranium = nist->FindOrBuildMaterial("G4_URANIUM_OXIDE");

  // Defines the material tungsten
  G4Material* tungsten = nist->FindOrBuildMaterial("G4_C");

  G4Box* firstWall = 
    new G4Box("first", 60*cm, 60*cm, 4*cm);

  G4LogicalVolume* logicFirstWall =
    new G4LogicalVolume(firstWall, tungsten, "first");

  G4VPhysicalVolume* physFirstWall =
    new G4PVPlacement(0,                  // rotation
                      G4ThreeVector(0*cm, 0*cm, -60*cm), // at (0,0,30cm)
                      logicFirstWall,         // logical volume
                      "first",      // name
                      logicWorld,         // mother
                      false,              // ?, just make false
                      0,                  // copy number
                      checkOverlaps);     // overlaps checking

  
  

  // ~~ Puts a tungsten reflector in the backside of the uranium box ~~ //
  G4Box* secondWall =
    new G4Box("second", 60*cm, 60*cm, 4*cm);

  G4LogicalVolume* logicSecondWall =
    new G4LogicalVolume(secondWall, tungsten, "second");

  G4VPhysicalVolume* physSecondWall =
    new G4PVPlacement(0,
                      G4ThreeVector(0, 0, 0*cm),
                      logicSecondWall,         // logical volume
                      "second",      // name
                      logicWorld,         // mother
                      false,              // ?, just make false
                      0,                  // copy number
                      checkOverlaps);     // overlaps checking

  // ~~ Puts a tungsten reflector in the bottom side of the uranium box ~~ //
  G4Box* thirdWall =
    new G4Box("third", 60*cm, 60*cm, 4*cm);

  G4LogicalVolume* logicThirdWall =
    new G4LogicalVolume(thirdWall, tungsten, "third");

  G4VPhysicalVolume* physThirdWall =
    new G4PVPlacement(0,
                      G4ThreeVector(0, 0, 60*cm),
                      logicThirdWall,         // logical volume
                      "third",      // name
                      logicWorld,         // mother
                      false,              // ?, just make false
                      0,                  // copy number
                      checkOverlaps);     // overlaps checking

  // Returns the physical world
  return world;

}