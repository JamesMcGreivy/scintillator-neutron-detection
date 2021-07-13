// Author -- James McGreivy
// Date -- July 2nd 2021

#ifndef HitsCollection_h
#define HitsCollection_h 1

#include "myHit.hh"

class HitsCollection
{

private:
	static std::vector<myHit*> allHits;

public:
	static std::vector<myHit*> GetAllHits() { return allHits; };

	static void AddHit(myHit* hit);

	static void Clear();

	static void ToFile(G4String path);

	static std::mutex mtx;

};

#endif

