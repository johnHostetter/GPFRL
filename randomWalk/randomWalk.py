# -*- coding: utf-8 -*-
"""
Created on Sat Sep 12 21:46:48 2020

@author: jhost
"""

import numpy as np

def zeros(rows, columns):
    if columns == 1:
        return np.array([0.0]*rows)
    else:
        return np.array([0.0]*rows*columns).reshape(rows, columns)

def logsig(n):
    """ Matlab's logsig function """
    try:
        return 1 / (1 + np.exp(-n))
    except RuntimeWarning:
        print('The statement: np.exp(-n) in logsig(n) resulted in an overflow, and an infinite value was produced as a result. Approximating the value to be zero instead.')
        return 0.0

def randn():
    """ Matlab's normally distributed random number function """
    return np.random.normal()

# constants
alpha = 0.003               # actor learning rate
beta = 0.005                # critic learning rate
epsilon = 0.01              # exploration/exploitation control parameter
gamma = 0.95                # discount factor
totalActions = 4            # number of possible actions
totalAgents = totalActions  # total number of learning agents

# simulation control
gridSize = 10              
totalStates = gridSize*gridSize
rewardM = np.ceil(gridSize/2)
rewardN = rewardM + 1
startM = 0 # originally 1 in MatLab
startN = 0 # originally 1 in MatLab
totalPlays = 1 # default is 100
totalTrials = 40 # default is 40

# variables
totalNumSteps = zeros(totalTrials, 1)           # total number of steps for each trial over all plays
totalReward = zeros(totalTrials, totalAgents)

# start walking
for j in range(totalPlays):
    numSteps = zeros(totalTrials, 1) # store number of steps for each trial
    
    # initializing actor and critic weights
    v = zeros(totalStates, totalAgents)
    w = zeros(totalStates, totalAgents)
    
    for i in range(totalTrials):
        print(j+(i/100))
        # current position on grid is represented by m,n
        m = startM
        n = startN
        prevM = startM
        prevN = startN
        
        # clean some variables
        p = zeros(totalAgents, 1)
        oldp = p
        
        # begin stepping through a path
        found = 0
        while found == 0:
            # increment number of steps taken this trial
            numSteps[i] += 1
            
            # find probabilities from actor weights
            sumWeights = zeros(totalAgents, 1)
            ro = zeros(totalStates, totalAgents)
            for agent in range(totalAgents):
                for state in range(totalStates):
                    ro[state, agent] = logsig(w[state, agent])
                    sumWeights[agent] = sumWeights[agent] + ro[state, agent]
            
            # perform an observation (using boxes system)
            stidx = ((m * 1) * gridSize) + n                    # NOTE: in dissertation, this was '(m   1)'
            X = zeros(totalStates, 1)
            X[stidx] = 1
            
            # finding final probability
            P = zeros(totalAgents, 1)
            for agent in range(totalAgents):
                P[agent] = ro[stidx, agent]
                
            # creating action set avoiding grid borders
            N = m - 1                                           # NOTE: in dissertation, this was 'N = m   1', referring to SARSA and Q-Learning code
            if N < 0: # originally N < 1
                N = 0 # originally N = 1
            E = n + 1
            if E > gridSize - 1: # originally E > gridSize
                E = gridSize - 1 # originally E = gridSize
            S = m + 1
            if S > gridSize - 1: # originally S > gridSize
                S = gridSize - 1 # originally S = gridSize
            W = n - 1
            if W < 0: # originally W < 1
                W = 0 # originally W = 1
                
            # initializing values
            Nu = zeros(totalAgents, 1)
            stdev = zeros(totalAgents, 1)
            
            # stochastic random number generator
            for agent in range(totalAgents):
                stdev[agent] = 2 * logsig(p[agent]) - 1
                Nu[agent] = stdev[agent]*randn()
                if Nu[agent] >= 1:
                    Nu[agent] = 1
                elif Nu[agent] <= -1:
                    Nu[agent] = -1
            
            # finding choices (Probability + exploitation/exploration)
            choices = zeros(totalAgents, 1)
            for agent in range(totalAgents):
                choices[agent] = P[agent] + Nu[agent]
                
            # choose the path with the maximum probability, or explore
            maximum = max(choices)
            indices = np.where(choices == maximum)[0]
            if len(indices) == 1 and maximum > epsilon:
                index = indices[0]
            else:
                index = np.random.choice([0, 1, 2, 3]) # randomly select direction
            
            # take action
            r = zeros(totalAgents, 1)
            if index == 0: # grid(m-1,n), originally was index == 1
                m = N
                r[index] = np.sign(m-rewardM)
            elif index == 1: # grid(m,n+1), originally was index == 2
                n = E
                r[index] = np.sign(rewardN-n)
            elif index == 2: # grid(m+1,n), originally was index == 3
                m = S
                r[index] = np.sign(rewardM-m)
            elif index == 3: # grid(m,n-1), originally was index == 4
                n = W
                r[index] = np.sign(n-rewardN)
            else:
                print('error random number too big')
            
            # stop after this step if the reward was found
            if m == rewardM and n == rewardN:
                found = 1
                
            # saving prediction information
            oldp = p
            p = zeros(totalAgents, 1)
            
            # computing the prediction of eventual reinforcement
            for agent in range(totalAgents):
                p[agent] = v[stidx, agent]
            
            # computing the prediction error using temporal differences method
            rbar = zeros(totalAgents, 1)
            for agent in range(totalAgents):
                predErr = gamma * p[agent] - oldp[agent]        # NOTE: in dissertation, the minus is missing
                rbar[agent] = r[agent] + predErr
            
            # learning the value functions
            for agent in range(totalAgents):
                for state in range(totalStates):
                    w[state, agent] = w[state, agent] + alpha * rbar[agent] * X[state] * ( 1 / ro[state, agent] ) * np.exp( -w[state, agent] ) * sumWeights[ agent ]
                    v[state, agent] = v[state, agent] - beta * gamma * rbar[agent] * X[state]
            
            # update values for next step, and directional choices
            prevM = m
            prevN = n
        
        # update storage matrices for across all plays
        totalNumSteps = totalNumSteps + numSteps
    
    # outputs
    # average number of steps across all plays
    print(totalNumSteps / totalPlays)