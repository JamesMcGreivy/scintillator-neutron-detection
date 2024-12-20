import tensorflow as tf
import tensorflow.keras as keras
from sklearn.model_selection import StratifiedKFold
from tensorflow.keras.layers import Dense, Dropout

def get_model(inputDim, outputDim, numNeurons0, numNeurons1, numNeurons2, alpha0, alpha1, alpha2):
    numNeurons0 = int( numNeurons0 )
    numNeurons1 = int( numNeurons1 )
    numNeurons2 = int( numNeurons2 )
    
    dropout = 0.02
    
    model = keras.Sequential()
    model.add(keras.Input(shape=(inputDim,)))
    if numNeurons0 >= 1:
        model.add(Dropout(dropout))
        model.add(Dense(numNeurons0, activation=keras.layers.LeakyReLU(negative_slope = alpha0)))
    if numNeurons1 >= 1:
        model.add(Dropout(dropout))
        model.add(Dense(numNeurons1, activation=keras.layers.LeakyReLU(negative_slope = alpha1)))
    if numNeurons2 >= 1:
        model.add(Dropout(dropout))
        model.add(Dense(numNeurons2, activation=keras.layers.LeakyReLU(negative_slope = alpha2)))
        
    model.add(Dense(outputDim,