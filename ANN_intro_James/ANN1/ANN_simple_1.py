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
from keras.models import load_model

from sklearn.model_selection import train_test_split
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

print("X:", X.shape, "Y:", Y.shape)
in_dim = X.shape[1]
out_dim = Y.shape[1]

# Split into training and testing data, set at 10%
xtrain, xtest, ytrain, ytest= train_test_split(X, Y, test_size=0.1)
print("xtrain:", xtrain.shape, "ytrain:", ytrain.shape)

# two hidden layers with 60 neurons each
model = Sequential()
model.add(Dense(in_dim, input_dim=in_dim, activation="relu"))
model.add(Dense(60, activation="relu"))
model.add(Dense(60, activation="relu"))
model.add(Dense(out_dim, activation='linear'))

model.compile(loss='mse', optimizer="adam")

# Fit the model
history = model.fit(xtrain, ytrain,
                    validation_data=(xtest, ytest),
                    epochs=1000, batch_size=25, verbose=1)

# plot training history
plt.plot(history.history['loss'], label='train')
plt.plot(history.history['val_loss'], label='test')
plt.semilogy()
plt.legend()
plt.ylabel('MSE')
plt.xlabel('Epoch')
plt.show()

# evaluate the model
train_mse = model.evaluate(xtrain, ytrain, verbose=0)
test_mse = model.evaluate(xtest, ytest, verbose=0)
print('Train MSE: %.3e, Test MSE: %.3e' % (train_mse, test_mse))

ypred_train = model.predict(xtrain)
ypred_test = model.predict(xtest)

plt.clf()
plt.rcParams["font.family"] = "Arial"
plt.rcParams.update({'font.size': 18})

# Plot the ANN predicted spectra vs. the real spectra
E_struct = np.load('../data/E_struct.npy') # energy bins corresponding to neutron spectrum

# Training data (should look good)
for i in range(ytrain.shape[0]):
    plt.step(E_struct, np.insert(ytrain[i,:], 0, 0), label="Real")
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

# Test data (Should look worse)
for i in range(ytest.shape[0]):
    plt.step(E_struct, np.insert(ytest[i,:], 0, 0), label="Real", linestyle='--')
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

