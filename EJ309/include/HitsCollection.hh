// Author -- James McGreivy
// Date -- July 2nd 2021

#ifndef HitsCollection_h
#define HitsCollection_h 1

#include "myHit.hh"

class HitsCollection
{

private:	
	static G4int eventNumber;

	static std::vector<myHit*> allHits;

public:
	static std::vector<myHit*> GetAllHits() { return allHits; };

	static void AddHit(myHit* hit);

	static void EndEvent();

	static G4int GetEventNumber() { return eventNumber; };

	static void ToFile(G4String path);

	static std::mutex mtx;

};

#endif

