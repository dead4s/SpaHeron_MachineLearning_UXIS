# Score prediction using Multiple Linear Regression

import torch
import torch.nn as nn
import torch.nn.functional as F
import torch.optim as optim
import numpy as np

torch.manual_seed(3)

Data_set = np.loadtxt("./state_count.csv", delimiter=",")

X = Data_set[:, 0:4]
Y = Data_set[:, 4]

x_train = torch.FloatTensor(X)
y_train = torch.FloatTensor(Y)

W = torch.zeros((4, 1), requires_grad=True)
b = torch.zeros(1, requires_grad=True)

optimizer = optim.SGD([W, b], lr=1e-5)

nb_epochs = 200
for epoch in range(nb_epochs + 1):
    hypothesis = x_train.matmul(W) + b

    cost = torch.mean((hypothesis - y_train) ** 2)

    optimizer.zero_grad()
    cost.backward()
    optimizer.step()

    if epoch % 100 == 0:
        print('Epoch {:4d}/{} hypothesis: {} Cost: {:.6f}'.format(
            epoch, nb_epochs, hypothesis.squeeze().detach(), cost.item()
    ))

x_test = torch.FloatTensor([200, 30, 270, 100])
y_predict = x_test.matmul(W) + b
print('\n[Predict]\n Test Set: {}\n Predicted Score{}'.format(x_test, y_predict))