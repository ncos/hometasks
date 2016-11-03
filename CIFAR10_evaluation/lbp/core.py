#!/usr/bin/python

from input import *
#from ggplot import *
import numpy as np
import time
from random import *

from linear_classifier import SVM

def loss_grad_svm_vectorized(W, X, y, reg):
    """
    Compute the loss and gradients using softmax function 
    with loop, which is slow.

    Parameters
    ----------
    W: (K, D) array of weights, K is the number of classes and D is the dimension of one sample.
    X: (D, N) array of training data, each column is a training sample with D-dimension.
    y: (N, ) 1-dimension array of target data with length N with lables 0,1, ... K-1, for K classes
    reg: (float) regularization strength for optimization.

    Returns
    -------
    a tuple of two items (loss, grad)
    loss: (float)
    grad: (K, D) with respect to W
    """

    dW = np.zeros(W.shape)
    loss = 0.0
    delta = 1.0

    num_train = y.shape[0]

    # compute all scores
    scores_mat = W.dot(X) # [C x N] matrix
 
    # get the correct class score 
    correct_class_score = scores_mat[y, xrange(num_train)] # [1 x N]
    
    margins_mat = scores_mat - correct_class_score + delta # [C x N]

    # set the negative score to be 0
    margins_mat = np.maximum(0, margins_mat)
    margins_mat[y, xrange(num_train)] = 0

    loss = np.sum(margins_mat) / num_train

    # add regularization to loss
    loss += 0.5 * reg * np.sum(W * W)

    # compute gradient
    scores_mat_grad = np.zeros(scores_mat.shape)

    # compute the number of margin > 0 for each sample
    num_pos = np.sum(margins_mat > 0, axis=0)
    scores_mat_grad[margins_mat > 0] = 1
    scores_mat_grad[y, xrange(num_train)] = -1 * num_pos

    # compute dW
    dW = scores_mat_grad.dot(X.T) / num_train + reg * W
    
    return loss, dW




# generate a rand weights W 
W = np.random.randn(10, X_train.shape[0]) * 0.001

tic = time.time()
loss_vec, grad_vect = loss_grad_svm_vectorized(W, X_train, y_train, 0)
toc = time.time()
print 'Vectorized loss: %f, and gradient: computed in %fs' % (loss_vec, toc - tic)

# file: algorithms/gradient_check.py
def grad_check_sparse(f, x, analytic_grad, num_checks):
  """
  sample a few random elements and only return numerical
  in this dimensions.
  """
  h = 1e-5

  print x.shape

  for i in xrange(num_checks):
    ix = tuple([randrange(m) for m in x.shape])
    print ix
    x[ix] += h # increment by h
    fxph = f(x) # evaluate f(x + h)
    x[ix] -= 2 * h # increment by h
    fxmh = f(x) # evaluate f(x - h)
    x[ix] += h # reset

    grad_numerical = (fxph - fxmh) / (2 * h)
    grad_analytic = analytic_grad[ix]
    rel_error = abs(grad_numerical - grad_analytic) / (abs(grad_numerical) + abs(grad_analytic))
    print 'numerical: %f analytic: %f, relative error: %e' % (grad_numerical, grad_analytic, rel_error)


f = lambda w: loss_grad_svm_vectorized(w, X_train, y_train, 0)[0]
grad_numerical = grad_check_sparse(f, W, grad_vect, 10)




# # using SGD algorithm
#SVM_sgd = SVM()
#tic = time.time()
#losses_sgd = SVM_sgd.train(X_train, y_train, method='sgd', batch_size=200, learning_rate=1e-6,
#              reg = 1e5, num_iters=1000, verbose=True, vectorized=True)
#toc = time.time()
#print 'Traning time for SGD with vectorized version is %f \n' % (toc - tic)

#y_train_pred_sgd = SVM_sgd.predict(X_train)[0]
#print 'Training accuracy: %f' % (np.mean(y_train == y_train_pred_sgd))
#y_val_pred_sgd = SVM_sgd.predict(X_val)[0]
#print 'Validation accuracy: %f' % (np.mean(y_val == y_val_pred_sgd))


#qplot(xrange(len(losses_sgd)), losses_sgd) + labs(x='Iteration number', y='SGD Loss value')




# Using validation set to tune hyperparameters, i.e., learning rate and regularization strength
learning_rates = [1e-5, 1e-8]
regularization_strengths = [10e2, 10e4]

# Result is a dictionary mapping tuples of the form (learning_rate, regularization_strength) 
# to tuples of the form (training_accuracy, validation_accuracy). The accuracy is simply the fraction
# of data points that are correctly classified.
results = {}
best_val = -1
best_svm = None
# Choose the best hyperparameters by tuning on the validation set
i = 0
interval = 5
for learning_rate in np.linspace(learning_rates[0], learning_rates[1], num=interval):
    i += 1
    print 'The current iteration is %d/%d' % (i, interval)
    for reg in np.linspace(regularization_strengths[0], regularization_strengths[1], num=interval):
        svm = SVM()
        svm.train(X_train, y_train, method='sgd', batch_size=200, learning_rate=learning_rate,
              reg = reg, num_iters=1000, verbose=False, vectorized=True)
        y_train_pred = svm.predict(X_train)[0]
        y_val_pred = svm.predict(X_val)[0]
        train_accuracy = np.mean(y_train == y_train_pred)
        val_accuracy = np.mean(y_val == y_val_pred)
        results[(learning_rate, reg)] = (train_accuracy, val_accuracy)
        if val_accuracy > best_val:
            best_val = val_accuracy
            best_svm = svm
        else:
            pass

# Print out the results
for learning_rate, reg in sorted(results):
    train_accuracy,val_accuracy = results[(learning_rate, reg)]
    print 'learning rate %e and regularization %e, \n \
    the training accuracy is: %f and validation accuracy is: %f.\n' % (learning_rate, reg, train_accuracy, val_accuracy)



y_test_predict_result = best_svm.predict(X_test)
y_test_predict = y_test_predict_result[0]
test_accuracy = np.mean(y_test == y_test_predict)
print 'The test accuracy is: %f' % test_accuracy


