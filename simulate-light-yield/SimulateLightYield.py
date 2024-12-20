import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from scipy import interpolate
from scipy import optimize
import os
from multiprocessing import Process
import pickle
import sys

DATA_DIR = "../EJ309-build/data/"
OUTPUT_DIR = "./output/"

PROTON_LY_PATH = "./experimental-data/ProtonResponseEJ309.txt"
CARBON_LY_PATH = "./experimental-data/CarbonResponseEJ309.txt"

NUMCORES = 4

# Creates a "particle tree" which is a dictionary containing
# { event0 : {ptcls} , event1 : {ptcls} , ... }
# "ptcls" contains:
# { ptclID : { "allEvents" : [] , "children" : {ptcls} } }
# where "allEvents" is every event performed by the particle with ID == ptclID
# and "children" recursively stores "ptcls" structures for each child
def buildPtclTree(filePath):
    ptclTree = {}
    
    data = np.array(pd.read_csv(filePath))
    
    IDtoRow = {}
    for row in data:
        IDtoRow[row[0]] = row

    for row in data:
        skipRow = False
        
        # Stack of parents
        IDStack = []
        ID = row[0]
        while "-0" not in ID:
            IDStack.append(ID)
            if ID not in IDtoRow:
                skipRow = True
                break
            ID = IDtoRow[ID][1]
            
        if skipRow :
            continue

        ID = 0
        currTree = ptclTree
        while True:
            ID = IDStack.pop()
            if ID not in currTree:
                currTree[ID] = { "allEvents" : [] , "children" : {} }

            if len(IDStack) == 0:
                currTree = currTree[ID]
                break

            currTree = currTree[ID]["children"]

        currTree["allEvents"].append(row)
        
    return ptclTree

# Takes in a "ptcl" and a material name, and finds the total energy deposited 
# by this particle, and all of its children, into that material
def netEDep(ptclInTree, materialName):
    eDep = 0
    for event in ptclInTree["allEvents"]:
        if event[4] in materialName:
            eDep += event[3]
    for child in ptclInTree["children"].values():
        eDep += netEDep(child, materialName)
    ptclInTree["totalEDep"] = eDep
    return eDep

# Takes in a "ptcl" and a particle type, and if this particle or any of its
# children have that type
def hasNoChildrenOfType(ptclInTree, ptclType):
    hasChild = True
    
    if ptclInTree["allEvents"][0][2] in ptclType:
        return False
    
    for child in ptclInTree["children"].values():
        hasChild = hasChild and hasNoChildrenOfType(child, ptclType)
    
    return hasChild

# Equation for Pulse Height uncertainty as a function of LY (From Dietz et Al.)
A = 0.113
B = 0.065
G = 0.060
def FWHM(L):
    if L == 0:
        return 0
    return L * ( ( A**2 + ( ( B**2 ) / L ) + ( ( G**2 ) / ( L**2 ) ) )**0.5 )

# Takes in a particle and finds the light generated in the scintillator 
# by the particle, or any of its children. 
# -> pRF is the "proton response function" which takes total energy deposited by a proton and 
# -> cRF is the "carbon response function"
def ptclLightYield(ptcl, pRF, cRF):
    pEDep = 0
    cEDep = 0
    
    for event in ptcl["allEvents"]:
        ptclType = event[2]
        if ptclType not in "proton C12 C13":
            break
        if event[4] not in "ej309":
            continue
        if ptclType in "proton":
            pEDep += event[3]
        if ptclType in "C12 C13":
            cEDep += event[3]
        
    # Get LY from Edep (convert from eV to MeV)
    pLY = pRF( pEDep / 1e6 )
    cLY = cRF( cEDep / 1e6 )
    # Now pLY contains the light yield from this particular proton or carbon, 
    # in units relative to a 477-keV e-
    # Must convert into MeVee to use function, then convert back
    sigma = FWHM(pLY * ( 477 / 1000 ) ) * ( 1000 / 477 ) / 2.355
    uncertainty = np.random.normal(0, sigma)
    pLY += uncertainty

    sigma = FWHM(cLY * ( 477 / 1000 ) ) * ( 1000 / 477 ) / 2.355
    uncertainty = np.random.normal(0, sigma)
    cLY += uncertainty
    
    lightYield = float( pLY + cLY )
    for child in ptcl["children"].values():
        lightYield += ptclLightYield(child, pRF, cRF)
    
    return lightYield

# Reads the proton and carbon light yield data and fits a curve to it
def f(x, A, B, C, D, E, F, G):
    return A*x + B*x**2 + C*x**3 + D*x**4

protonLightYield = np.array(pd.read_csv(PROTON_LY_PATH, delimiter=' ', header = None))
xP = np.append(np.array([0]), protonLightYield[:,0])
yP = np.append(np.array([0]), protonLightYield[:,1])

#poptP, pcovP = optimize.curve_fit(f, xP, yP)
#pRF = lambda x: f(x, *poptP)

pRF = interpolate.interp1d(xP, yP, fill_value='extrapolate')

carbonLightYield = np.array(pd.read_csv(CARBON_LY_PATH, delimiter=' ', header = None))
xC = np.append(np.array([0]), carbonLightYield[:,0])
yC = np.append(np.array([0]), carbonLightYield[:,1])

#poptC, pcovC = optimize.curve_fit(f, xC, yC)
#cRF = lambda x : f(x, *poptC)

cRF = interpolate.interp1d(xC, yC, fill_value='extrapolate')

# Gets an array of all primary particle light yields
# from raw path to data file
def GetLightYield(data):
    
    ptclTree = buildPtclTree(data)
    
    lightYield = []
    for ID in ptclTree:
        ptclYield = ptclLightYield(ptclTree[ID], pRF, cRF)
        if ptclYield > 0.0:
            lightYield.append(ptclYield)
            
    return np.array(lightYield)

# Bins to use for light yield histograma
BINS = np.linspace(0.3, 30, 51)

# Change the bins so that they fall at the midpoint of each range, 
# instead of being the beginning and end points 
bins = np.zeros(len(BINS) - 1)
for i in range(0, len(BINS) - 1):
    bins[i] = (BINS[i + 1] + BINS[i]) / 2.0
bins = np.array(bins)

def getLY(data):
    if data + ".npy" in os.listdir(OUTPUT_DIR):
        return
    # Generates a histogram of light yield
    lightYield = GetLightYield(DATA_DIR + data)
    #counts = np.array(plt.hist(lightYield, bins = BINS)[0])

    # Save to file
    np.save(OUTPUT_DIR + data, np.array(lightYield))

if __name__ == '__main__':
    # Multithreads each process
    ALLDATA = [f for f in os.listdir(DATA_DIR) if ".csv" in f]

    print(len(ALLDATA), " spectra to process")

    # This for loop just ensures that less than NUMCORES processes
    # are running at a time. There was probably a better way to do this.
    TEMPDATA = []
    i = 0
    for dat in ALLDATA:

        if len(TEMPDATA) < NUMCORES:
            TEMPDATA.append(dat)

        else:
            running_tasks = [Process(target = getLY, args = (data_,)) for data_ in TEMPDATA]
            for running_task in running_tasks:
                running_task.start()
            for running_task in running_tasks:
                running_task.join()
                i += 1
                print(i, " of ", len(ALLDATA))

            TEMPDATA = []
            TEMPDATA.append(dat)

    running_tasks = [Process(target = getLY, args = (data_,)) for data_ in TEMPDATA]
    for running_task in running_tasks:
        running_task.start()
    for running_task in running_tasks:
        running_task.join()
        i += 1
        print(i, " of ", len(ALLDATA))
            
    TEMPDATA = []

    print(TEMPDATA)