// Author -- James McGreivy
// Date -- Jun 30th 2021

#include "HitsCollection.hh"
#include "SensitiveDetector.hh"

HitsCollection::HitsCollection()
{
    hitsContainer = new std::vector<MyHit*>();
}
    
HitsCollection::~HitsCollection()
{
    for (MyHit* hit : *hitsContainer)
    {
        delete hit;
    }

    delete hitsContainer;
}

void HitsCollection::addHit(MyHit* hit)
{
    hitsContainer->push_back(hit);
}


