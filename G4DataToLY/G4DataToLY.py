import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from scipy import interpolate
from scipy import optimize
import os
import multiprocessing as mp
import sys

# Checks validity of command-line args
if len(sys.argv) != 4:
    raise Exception("Please Provide Valid Command Line Args : \n\t [...].py [type] [energy] [unit]")

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
        
    
    lightYield = float( pRF( pEDep / 1e6 ) + cRF( cEDep / 1e6 ) )
    for child in ptcl["children"].values():
        lightYield += ptclLightYield(child, pRF, cRF)
    
    return lightYield

# Reads the proton and carbon light yield data and fits a curve to it
def f(x, A, B, C, D, E, F, G):
    return A*x + B*x**2 + C*x**3 + D*x**4

protonLightYield = np.array(pd.read_csv("G4DataToLY/ProtonResponseEJ309.txt", delimiter=' ', header = None))
xP = np.append(np.array([0]), protonLightYield[:,0])
yP = np.append(np.array([0]), protonLightYield[:,1])

poptP, pcovP = optimize.curve_fit(f, xP, yP)
pRF = lambda x: f(x, *poptP)

carbonLightYield = np.array(pd.read_csv("G4DataToLY/CarbonResponseEJ309.txt", delimiter=' ', header = None))
xC = np.append(np.array([0]), carbonLightYield[:,0])
yC = np.append(np.array([0]), carbonLightYield[:,1])

poptC, pcovC = optimize.curve_fit(f, xC, yC)
cRF = lambda x : f(x, *poptC)


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

# Gets the file to be read from the command line args
ptcl = sys.argv[1]
energy = sys.argv[2]
unit = sys.argv[3]
filename = ptcl + "_" + energy + "_" + unit + ".csv"

DATADIR = "EJ309-build/data/"

bins = None
counts = None

# Bins to use for light yield histograma
BINS = np.linspace(1e-3, 30, 51)

# Generates a histogram of light yield
lightYield = GetLightYield(DATADIR + filename)
counts = plt.hist(lightYield, bins = BINS)[0]

# Change the bins so that they fall at the midpoint of each range, 
# instead of being the beginning and end points 
bins = np.zeros(len(BINS) - 1)
for i in range(0, len(BINS) - 1):
    bins[i] = (BINS[i + 1] + BINS[i]) / 2.0

# Save to file
np.save("G4DataToLY/LightYields/" + ptcl + energy + unit, np.array([bins, counts]))
