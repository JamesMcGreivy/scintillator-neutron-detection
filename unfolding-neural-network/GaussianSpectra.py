import numpy as np
import multiprocessing
from scipy import interpolate

# Normalizes the data
def normalize(bins, counts):
    width = bins[1] - bins[0]
    total_counts = sum(counts)
    if total_counts == 0:
        total_counts = 1
    counts = counts / ( width * total_counts )
    return counts

# Randomly adds a gaussian peak to a 1D array
def addGaussian(data, i, count):
    # Randomly picks the width and height of the gaussian
    # ~8 is placeholder for typical width of peak in the real-world data (depends on detector resolution)
    H = np.random.normal(loc = 10 / count, scale = 5 / count)
    wLoc = len(data) * 0.1
    wScale = wLoc / 6
    W = int( np.abs(np.random.normal(loc = wLoc * count, scale = wScale * count))) + 1

    for k in range(-W, W + 1):
        if (i + k) >= len(data):
            break
        elif (i + k) < 0:
            continue
        else:
            data[i + k] += H * np.exp(- (2.5*k/W)**2)

# This is my attempt at multi-threading the random data generation
def GaussianSpectra(num, yDim, bins_LY, energy_to_LY, THREADS=4):
    energies = list(energy_to_LY)
    energies.sort()
    xDim = len(energy_to_LY[energies[0]])

    # Gets the detector response of a given neutron spectra
    def getX(yRow):
        xRow = np.zeros(xDim)
        for index in range(len(energies)):
            xRow += yRow[index] * energy_to_LY[energies[index]]        
        return normalize(bins_LY, xRow)
    
    # interpolates the energy spectrum for training data
    # so that there only need to be N output neurons
    def interp_y(y):
        f = interpolate.interp1d(energies, y)
        new_energies = np.linspace(np.min(energies), np.max(energies), yDim)
        return new_energies, f(new_energies)
    
    # Generates (count) data points and appends to a thread-safe queue
    def addXYtoQueue(count, queue, seed):
        np.random.seed(seed)
        x = np.zeros( (0, xDim) )
        y = np.zeros( (0, yDim) )

        for _ in range(count):
            yRow = np.zeros(len(energies))

            i = np.random.randint(low = 0, high = len(energies) + 1)
            addGaussian(yRow, i, 1)

            # Flip a coin and keep adding gaussian spikes
            p = np.random.random()
            nSpikes = 2
            while p > 0.4:
                i = np.random.randint(low = 0, high = len(energies) + 1)
                addGaussian(yRow, i, nSpikes)
                nSpikes += 1
                p = np.random.random()
            yRow = np.abs(yRow)
            xRow = getX(yRow)
            yRow = normalize(*interp_y(yRow))

            x = np.vstack( (x, xRow) )
            y = np.vstack( (y, yRow) )   

        queue.put((x, y))

    manager = multiprocessing.Manager()
    queue = manager.Queue()
    
    numPerThread = int(np.ceil(num / THREADS))
    
    proccesses = [multiprocessing.Process(target = addXYtoQueue, args = (numPerThread, queue, int(1e9 * np.random.random())), daemon = True) for _ in range(THREADS)]
    
    for p in proccesses:
        p.start()
    for p in proccesses:
        p.join(timeout = 3)
        
    x = np.zeros( (0, xDim) )
    y = np.zeros( (0, yDim) )
    
    while not queue.empty():
        xRow, yRow = queue.get()
        x = np.vstack( (x, xRow) )
        y = np.vstack( (y, yRow) )
   
    return x, y
