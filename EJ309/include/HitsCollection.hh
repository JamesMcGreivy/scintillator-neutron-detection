// Author -- James McGreivy
// Date -- Jun 30th 2021

#include "MyHit.hh"
#include "SensitiveDetector.hh"

#ifndef HitsCollection_h
#define HitsCollection_h 1

class HitsCollection
{

public:
    HitsCollection();
    
    ~HitsCollection();

    void addHit(MyHit* hit);

    std::vector<MyHit*>::iterator begin() { return hitsContainer->begin(); }
    std::vector<MyHit*>::iterator end() { return hitsContainer->end(); }
    
private:
    std::vector<MyHit*>* hitsContainer;

};

#endif