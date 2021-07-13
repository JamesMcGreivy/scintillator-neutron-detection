// Author -- James McGreivy
// Date -- Jun 30th 2021

#include "HitsCollection.hh"
#include <fstream>

std::vector<myHit*> HitsCollection::allHits = std::vector<myHit*>();
std::mutex HitsCollection::mtx;

void HitsCollection::AddHit(myHit* hit)
{
	HitsCollection::mtx.lock();
	allHits.push_back(hit);
	HitsCollection::mtx.unlock();
}

void HitsCollection::Clear()
{
	HitsCollection::mtx.lock();
	for (myHit* hit : HitsCollection::allHits)
		delete hit;
	allHits.clear();
	HitsCollection::mtx.unlock();
}

void HitsCollection::ToFile(G4String path)
{

	std::ofstream file;
	file.open(path);

	file << "trackID,parentID,pType,eDep(eV),materialName,currKE(eV),currTime\n";

	for (myHit* hit : HitsCollection::allHits)
	{
		file << hit->ToString();
		file << "\n";
	}

	file.close();

}