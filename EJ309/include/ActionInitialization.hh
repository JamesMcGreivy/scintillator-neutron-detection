// Author -- James McGreivy
// Date -- Jun 26th 2021

#ifndef ActionInitialization_h
#define ActionInitialization_h 1

#include "G4VUserActionInitialization.hh"

// Action initialization class

class ActionInitialization : public G4VUserActionInitialization
{
public:
  ActionInitialization();

  virtual void BuildForMaster() const;
  virtual void Build() const;
};

#endif

