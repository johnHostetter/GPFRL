# -*- coding: utf-8 -*-
"""
Created on Sun Sep 13 18:22:43 2020

@author: jhost
"""

import numpy as np
import matplotlib.pyplot as plt

class GPFRL:
    def __init__(self, totalActions, totalStates):
        self.alpha = 0.003                                  # actor learning rate
        self.beta = 0.005                                   # critic learning rate
        self.epsilon = 0.01                                 # exploration/exploitation control parameter
        self.gamma = 0.95                                   # discount factor
        self.totalActions = totalActions                    # number of possible actions
        self.totalAgents = self.totalActions                # total number of learning agents
        self.totalStates = totalStates
        # initializing actor and critic weights
        self.v = self.zeros(totalStates, self.totalAgents)
        self.w = self.zeros(totalStates, self.totalAgents)
        self.p = self.zeros(self.totalAgents, 1)
        self.oldp = self.p
    def zeros(self, rows, columns):
        """ Matlab's zeros function. """
        if columns == 1:
            return np.array([0.0]*rows)
        else:
            return np.array([0.0]*rows*columns).reshape(rows, columns)
    def __logsig(self, n):
        """ Matlab's logsig function. """
        try:
            return 1 / (1 + np.exp(-n))
        except RuntimeWarning:
            print('The statement: np.exp(-n) in logsig(n) resulted in an overflow, and an infinite value was produced as a result. Approximating the value to be zero instead.')
            return 0.0
    def __randn(self):
        """ Matlab's normally distributed random number function. """
        return np.random.normal()
    def softReset(self):
        """ Performs a soft reset of the GPFRL agent. This is to be done at the end of a 
        trial/episode when either the goal has been reached or a failure has occurred. """
        self.p = self.zeros(self.totalAgents, 1)
        self.oldp = self.p
    def hardReset(self):
        """ Performs a hard reset of the GPFRL agent. This is to be done at the end of a
        play or sequence of episodes, if the agent is performing poorly or to see how well
        the agent does over a sequence of multiple plays - is similar to recreating the agent. """
        self.softReset()
        # initializing actor and critic weights
        self.v = self.zeros(self.totalStates, self.totalAgents)
        self.w = self.zeros(self.totalStates, self.totalAgents)
    def calculateProbabilities(self):
        # find probabilities from actor weights
        self.sumWeights = self.zeros(self.totalAgents, 1)
        self.ro = self.zeros(self.totalStates, self.totalAgents)
        for agent in range(self.totalAgents):
            for state in range(self.totalStates):
                self.ro[state, agent] = self.__logsig(self.w[state, agent])
                self.sumWeights[agent] += self.ro[state, agent]
    def findFinalProbabilities(self, idx):
        """ Finds the final probabilities given the index ('stidx') of the current state
        observed in the environment. """
        self.P = self.zeros(self.totalAgents, 1)
        for agent in range(self.totalAgents):
            self.P[agent] = self.ro[idx, agent]
    def indexOfRecommendedAction(self):
        # initializing values
        self.Nu = self.zeros(self.totalAgents, 1)
        self.stdev = self.zeros(self.totalAgents, 1)
            
        # stochastic random number generator
        for agent in range(self.totalAgents):
            self.stdev[agent] = 2 * self.__logsig(self.p[agent]) - 1
            self.Nu[agent] = self.stdev[agent]*self.__randn()
            if self.Nu[agent] >= 1:
                self.Nu[agent] = 1
            elif self.Nu[agent] <= -1:
                self.Nu[agent] = -1
            
        # finding choices (Probability + exploitation/exploration)
        self.choices = self.zeros(self.totalAgents, 1)
        for agent in range(self.totalAgents):
            self.choices[agent] = self.P[agent] + self.Nu[agent]
                
        # choose the path with the maximum probability, or explore
        maximum = max(self.choices)
        indices = np.where(self.choices == maximum)[0]
        if len(indices) == 1 and maximum > self.epsilon:
            index = indices[0]
        else:
            index = np.random.choice(list(range(self.totalActions))) # randomly select direction
        return index
    def backpropagate(self, r, stidx, X):
        """ Backpropagate with the given array of rewards, 'r', (that is the length of number of agents),
        and the index ('stidx') of the current state observed in the environment. """
        # saving prediction information
        self.oldp = self.p
        self.p = self.zeros(self.totalAgents, 1)
        
        # computing the prediction of eventual reinforcement
        for agent in range(self.totalAgents):
            self.p[agent] = self.v[stidx, agent]
        
        # computing the prediction error using temporal differences method
        self.rbar = self.zeros(self.totalAgents, 1)
        for agent in range(self.totalAgents):
            self.predErr = self.gamma * self.p[agent] - self.oldp[agent]        # NOTE: in dissertation, the minus is missing
            self.rbar[agent] = r[agent] + self.predErr
        
        # learning the value functions
        for agent in range(self.totalAgents):
            for state in range(self.totalStates):
                self.w[state, agent] = self.w[state, agent] + self.alpha * self.rbar[agent] * X[state] * ( 1 / self.ro[state, agent] ) * np.exp( -self.w[state, agent] ) * self.sumWeights[ agent ]
                self.v[state, agent] = self.v[state, agent] - self.beta * self.gamma * self.rbar[agent] * X[state]
    def demo():
        # simulation control
        gridSize = 10              
        totalStates = gridSize*gridSize
        rewardM = np.ceil(gridSize/2)
        rewardN = rewardM + 1
        startM = 0 # originally 1 in MatLab
        startN = 0 # originally 1 in MatLab
        totalPlays = 1 # default is 100
        totalTrials = 40 # default is 40
        
        simul = GPFRL(4, totalStates)
        
        # variables
        totalNumSteps = simul.zeros(totalTrials, 1)           # total number of steps for each trial over all plays
        
        # start walking
        for j in range(totalPlays):
            numSteps = simul.zeros(totalTrials, 1) # store number of steps for each trial
            
            gpfrl = GPFRL(4, 100)
            
            for i in range(totalTrials):
                print(j+(i/100))
                # current position on grid is represented by m,n
                m = startM
                n = startN
                
                # clean some variables
                gpfrl.softReset()
                
                # begin stepping through a path
                continueTrial = True
                while continueTrial:
                    # increment number of steps taken this trial
                    numSteps[i] += 1
                    
                    # find probabilities from actor weights
                    gpfrl.calculateProbabilities()
                    
                    # perform an observation (using boxes system)
                    stidx = ((m * 1) * gridSize) + n                    # NOTE: in dissertation, this was '(m   1)'
                    X = simul.zeros(totalStates, 1)
                    X[stidx] = 1
                    
                    # finding final probability
                    gpfrl.findFinalProbabilities(stidx)
                        
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
                        
                    # asn
                    index = gpfrl.indexOfRecommendedAction()
                    
                    # take action
                    r = simul.zeros(simul.totalAgents, 1)
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
                        continueTrial = False
                    
                    # update agent
                    gpfrl.backpropagate(r, stidx, X)
                
                # update storage matrices for across all plays
                totalNumSteps = totalNumSteps + numSteps
            
            # outputs
            # average number of steps across all plays
            plt.plot(totalNumSteps / totalPlays)
            plt.xlabel('Episodes')
            plt.ylabel('Time Steps Until Goal Reached')
            plt.show()