// Author -- James McGreivy
// Date -- Jun 26th 2021

#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4GeneralParticleSource.hh"
#include "globals.hh"

class G4ParticleGun;
class G4Event;
class G4Box;

// The primary generator action class, which defines a particle source.
class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
  PrimaryGeneratorAction();
  virtual  ~PrimaryGeneratorAction();

  // Method from the base class
  virtual void GeneratePrimaries(G4Event*);

  // Method to access particle gun
  const G4GeneralParticleSource* GetParticleSource() const { return fParticleSource; }

private:
  // The particle source class which allows the user to
  // set the type of ptcl, energy, trajectories, etc.
  G4GeneralParticleSource* fParticleSource;
};

#endif