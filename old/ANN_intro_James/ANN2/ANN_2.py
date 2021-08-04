# -*- coding: utf-8 -*-
"""
Experimenting on ANN with Keras

Created on Sat Aug 29 10:20:04 2020

@author: siefman1
"""

import numpy as np
import matplotlib.pyplot as plt
import pandas as pd

from keras import losses
from keras.models import Sequential
from keras.layers import Dense
from keras.callbacks import EarlyStopping
from keras.callbacks import ModelCheckpoint
from keras.models import load_model
from tensorflow.keras import regularizers

from sklearn.model_selection import train_test_split
from sklearn.model_selection import KFold
from sklearn.metrics import mean_squared_error

# Import unfolding data
unfolding_data = pd.read_pickle('../data/unfolding_data.pkl')

X = np.zeros((251,15))  # detector responses for 251 different spectra
Y = np.zeros((251,60))  # Neutron flux distrinution for 251 different spectra
for row in range(251):

    X[row,:] = unfolding_data['Detector Response'][row]
    Y[row,:] = unfolding_data['Spectrum'][row]

# Possible to normalize the data to see if performance improved
'''
from sklearn.preprocessing import MinMaxScaler
from sklearn.preprocessing import StandardScaler

xscaler = StandardScaler()
xscaler = MinMaxScaler() # create scaler
xscaler.fit(X) # fit scaler on data
X = xscaler.transform(X) # apply transform

yscaler = StandardScaler()
yscaler = MinMaxScaler() # create scaler
yscaler.fit(Y) # fit scaler on data
Y = yscaler.transform(Y) # apply transform
'''

in_dim = X.shape[1]
out_dim = Y.shape[1]

# Split into training and testing data, set at 10%
Xtrain, Xtest, Ytrain, Ytest= train_test_split(X, Y, test_size=0.1)

# Try Kfold cross validation from the remaining 90% of data, finally test
# best model on the 10% left out
from sklearn.model_selection import ShuffleSplit
SPLITS = 5
boot = ShuffleSplit(n_splits=SPLITS, test_size=0.50, random_state=42)

# Track progress
train_mses = np.zeros(SPLITS)
test_mses = np.zeros(SPLITS)
num = 0

# Loop through samples
for train, test in boot.split(Xtrain):
    
    # Split train and test
    xtrain = Xtrain[train]
    ytrain = Ytrain[train]

    xtest = Xtrain[test]
    ytest = Ytrain[test]
    
    in_dim = xtrain.shape[1]
    out_dim = ytrain.shape[1]

    model = Sequential()
    model.add(Dense(100, input_dim=in_dim, activation="relu"))
    model.add(Dense(100, activation="relu"))
    model.add(Dense(100, activation="relu"))
    model.add(Dense(out_dim, activation='linear'))
    
    model.compile(loss='mse', optimizer="adam")
    
    # Setup early stopping criterion: prevent overtraining based on validation loss
    # saves computer time by stopping training when overfitting has already occurred
    es = EarlyStopping(monitor='val_loss', mode='min',
                       min_delta=1e-5, verbose=1, patience=100)
    
    # Setup checkpoint to go back to best model at cretain epoch
    mc = ModelCheckpoint(f'fold#{num}_model.h5', monitor='val_loss',
                         mode='min', verbose=0, save_best_only=True)
    
    history = model.fit(xtrain, ytrain,
                        validation_data=(xtest, ytest),
                        epochs=1000, batch_size=25, callbacks=[es, mc],
                        verbose=0)

    # plot training history
    plt.plot(history.history['loss'], label='train')
    plt.plot(history.history['val_loss'], label='test')
    plt.semilogy()
    plt.legend()
    plt.ylabel('MSE')
    plt.xlabel('Epoch')
    plt.show()

    # Calculate MSEs by inverting the normilization
    ytrain_predict = model.predict(xtrain)
    ytest_predict = model.predict(xtest)
   
    train_mse = mean_squared_error(ytrain, ytrain_predict) #saved_model.evaluate(xtrain, ytrain, verbose=0)
    test_mse = mean_squared_error(ytest,ytest_predict)
    
    # if the data has been normalized
#    train_mse = mean_squared_error(yscaler.inverse_transform(ytrain),
#                                   yscaler.inverse_transform(ytrain_predict)) #saved_model.evaluate(xtrain, ytrain, verbose=0)
#    test_mse = mean_squared_error(yscaler.inverse_transform(ytest),
#                                  yscaler.inverse_transform(ytest_predict))
    
    train_mses[num] = train_mse
    test_mses[num] = test_mse

    print(f'Fold {num}')
    print('\tTrain MSE: %.3e, Test MSE: %.3e' % (np.mean(train_mse), np.mean(test_mse)))
    num += 1



# Load the best model (best defined as best for training data)
best = np.argmin(test_mses)
model = load_model(f'fold#{best}_model.h5')

# Run model with the 90% training data and 10% training data that it never saw
ypred_train = model.predict(Xtrain)
ypred_test = model.predict(Xtest)                 
                   
plt.clf()
plt.rcParams["font.family"] = "Arial"
plt.rcParams.update({'font.size': 18})

# Plot the ANN predicted spectra vs. the real spectra
E_struct = np.load('../data/E_struct.npy') # energy bins corresponding to neutron spectrum


for i in range(ytrain.shape[0]):
    plt.step(E_struct, np.insert(Ytrain[i,:], 0, 0), label="Real")
    plt.step(E_struct, np.insert(ypred_train[i,:], 0, 0), label="ANN Prediction")
    plt.semilogx()
    plt.legend()
    plt.xlabel('MeV')

    index =  np.arange(251)[np.sum(Y == ytrain[i,:],axis=1) == 60][0]
    title = unfolding_data['Spectrum Name'][index]
    plt.title(title + ' (Training)')
    saveas = title.replace('\n', '').replace('/', '-').replace(' ', '_')
    plt.savefig('train/%s.png' % saveas, bbox_inches='tight', dpi=500)
    plt.close()


for i in range(ytest.shape[0]):
    plt.step(E_struct, np.insert(Ytest[i,:], 0, 0), label="Real", linestyle='--')
    plt.step(E_struct, np.insert(ypred_test[i,:], 0, 0), label="ANN Prediction")
    plt.semilogx()
    plt.legend()
    plt.xlabel('MeV')
    plt.ylabel('Fluence per Unit Lethargy')
    index =  np.arange(251)[np.sum(Y == ytest[i,:],axis=1) == 60][0]
    title = unfolding_data['Spectrum Name'][index]
    plt.title(title + ' (Test)')
    saveas = title.replace('\n', '').replace('/', '-').replace(' ', '_')
    plt.savefig('test/%s.png' % saveas, bbox_inches='tight', dpi=500)
    plt.close()

