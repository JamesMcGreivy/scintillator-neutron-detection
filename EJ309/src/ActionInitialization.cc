// Author -- James McGreivy
// Date -- Jun 26th 2021

#include "ActionInitialization.hh"

// User Defined Actions
#include "PrimaryGeneratorAction.hh"

ActionInitialization::ActionInitialization() : G4VUserActionInitialization() {}

void ActionInitialization::BuildForMaster() const
{

}

void ActionInitialization::Build() const
{

  SetUserAction(new PrimaryGeneratorAction);

}