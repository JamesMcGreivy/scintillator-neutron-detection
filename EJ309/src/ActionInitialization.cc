// Author -- James McGreivy
// Date -- Jun 26th 2021

#include "ActionInitialization.hh"

// User Defined Action
#include "PrimaryGeneratorAction.hh"

// Just calls parent constructor
ActionInitialization::ActionInitialization() : G4VUserActionInitialization() {}

void ActionInitialization::Build() const
{
  // Sets the primary generator action, to be used when the user 
  // calls run->beamOn();
  SetUserAction(new PrimaryGeneratorAction);
}