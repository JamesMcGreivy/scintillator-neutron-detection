// Author -- James McGreivy
// Date -- Jun 26th 2021

#include "PrimaryGeneratorAction.hh"

#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4RunManager.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "SystemOfUnits.hh"


// Sets up the source which will generate
// the particles for the simulation
PrimaryGeneratorAction::PrimaryGeneratorAction()
: G4VUserPrimaryGeneratorAction(),
  fParticleSource(0)
{
  fParticleSource = new G4GeneralParticleSource();
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  delete fParticleSource;
}

// This gets called at the beginning of each event
void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  fParticleSource->SetParticlePosition(G4ThreeVector(0*inch, 0*inch, -5.0*inch));
  fParticleSource->GeneratePrimaryVertex(anEvent);
} 