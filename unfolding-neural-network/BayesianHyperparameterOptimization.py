import numpy as np
from bayes_opt import BayesianOptimization
from sklearn.model_selection import ShuffleSplit
from sklearn import metrics

# Tensorflow Stuff
from tensorflow.keras.callbacks import EarlyStopping
from tensorflow.keras.optimizers import Adam

from UnfoldingNeuralNetwork import get_model

def bayesian_hyperparameter_optimizer(pbounds, x, y, patience=5):
    xDim = len(x[0])
    yDim = len(y[0])
    splitter = ShuffleSplit(n_splits=5, test_size=0.2, random_state=42)

    def bootstrapModel(numNeurons0, numNeurons1, numNeurons2, alpha0, alpha1, alpha2):
        error = []
        for train_index, test_index in splitter.split(x, y):
            x_train, y_train = x[train_index], y[train_index]
            x_test, y_test = x[test_index], y[test_index]

            model = get_model(xDim, yDim, numNeurons0, numNeurons1, numNeurons2, alpha0, alpha1, alpha2)
            model.compile(loss = "mse", optimizer = Adam())

            monitor = EarlyStopping(monitor='val_loss', min_delta=1e-7, 
                                    patience=patience, verbose=0,
                                    mode='min', restore_best_weights=True)
            model.fit(x_train, y_train * 1e4, validation_data = (x_test, y_test * 1e4), batch_size = 256, callbacks=[monitor], verbose=0, epochs=10000)

            y_pred = model(x_test)
            error.append(metrics.mean_squared_error(y_test * 1e4, y_pred.numpy()))
        return -np.mean(error)

    return BayesianOptimization(
        f=bootstrapModel,
        pbounds=pbounds,
        verbose=2,
        random_state=42
    )

