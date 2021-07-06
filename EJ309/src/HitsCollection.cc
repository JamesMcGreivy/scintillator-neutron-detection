// Author -- James McGreivy
// Date -- Jun 30th 2021

#include "HitsCollection.hh"
#include <fstream>


G4int HitsCollection::eventNumber = 0;
std::vector<myHit*> HitsCollection::allHits = std::vector<myHit*>();
std::mutex HitsCollection::mtx;

void HitsCollection::AddHit(myHit* hit)
{
	hit->SetEventNum(GetEventNumber());
	HitsCollection::mtx.lock();
	allHits.push_back(hit);
	HitsCollection::mtx.unlock();
}

void HitsCollection::EndEvent()
{
	eventNumber ++;
}

void HitsCollection::ToFile(G4String path)
{

	std::ofstream file;
	file.open(path);

	for (myHit* hit : HitsCollection::allHits)
	{
		file << hit->ToString();
		file << "\n";
	}

	file.close();

}