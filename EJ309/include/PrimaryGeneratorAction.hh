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

// The primary generator action class with particle gun.
class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
  PrimaryGeneratorAction();
  virtual  ~PrimaryGeneratorAction();

  // method from the base class
  virtual void GeneratePrimaries(G4Event*);

  // method to access particle gun
  const G4GeneralParticleSource* GetParticleSource() const { return fParticleSource; }

private:
  G4GeneralParticleSource* fParticleSource;
};

#endif