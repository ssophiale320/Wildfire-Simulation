1. Overview: Forest Fires
Fighting fires is a risky job where loss of life is a real possibility. Proper training is essential. In the United States, the National Fire Academy, established in 1974, presents courses and programs that are intended "to enhance the ability of fire and emergency services and allied professionals to deal more effectively with fire and related emergencies" (Studebaker 2003).

The Fire Academy partnered with private contractors and the U.S. Forest Service to develop a three-dimensional land fire fighting training simulator. This simulator exposes trainees to a convincing fire propagation model, where instructors can vary fuel types, environmental conditions, and topography. Trainees may call for appropriate resources and construct fire lines. Instructors may alter parameters, changing fire behavior. Students can review the results of their decisions and learn from their mistakes in the safety of a computer laboratory (Studebaker 2003).

This assignment develops a two-dimensional computer simulation to model the spread of fire.
This project uses the C language to write a simulation of spreading fire that:
- Is a cellular automaton;
- Processes command line arguments of the form -xN;
- Uses cursor-control functions to output characters;
- Designs and uses structures, arrays and matrices;
- Uses random number generation1 and basic mathematics; and
- Includes a written status report on the behavior of the program, plus things learned while doing the work.

2. Background Information
   Modeling is the application of methods to analyze complex, real-world problems in order to make predictions about what might happen with various actions. When conducting experiments is too difficult, time-consuming, or dangerous, a computer simulation might be useful to generate and test various scenarios.

A computer simulation involving such a system is a cellular automaton. Cellular automata are dynamic computational models that are finite and discrete in space, state, and time.

Simulation models can provide visualizations and informative animations of the progress of events. For example, one can simulate and view the movement of ants toward a food source, the propagation of infectious diseases, heat diffusion, the distribution of pollution, or the motions of gas molecules.

One way to look at the world is to examine is as a group of smaller pieces, or cells, that are related. One convenient way to lay out the world is as a two-dimensional grid of cells. Each cell corresponds to an area in the world and is in one of several possible states at any given instant.

How can a model represent the spread of fire? Suppose each cell represents a single tree, and a grid of such cells would represent a forest. Should a tree catch fire, that fire could spread to neighboring trees If a cell catches fire, then that fire can spread to neighbors; the probability of the fire spreading can be based on factors such as wind, snow, rain, or dryness/dampness.

At any moment in time, each cell exists in a specific state (empty, contains an unburned tree, contains a burning tree, etc.). The collective state of all cells represents the state of the entire forest, which we may call the configuration of the forest system.

As time passes, cells undergo state transitions (changes from one state to another) according to a set of rules. These rules dictate the conditions under which each cell may change state.

3. Simulating Spreading Fire
   The program simulates the spreading of fire as a sequence of time steps that update the simulation state from one configuration state to another. At each time step, the program displays the new state of the two-dimensional grid of cells that represents a forest. The initial state of the forest is based on the values of several simulation parameters; a transition rule will determine how the state of each cell may change over time.

The forest configuration evolves over time, which we will simulate as a series of discrete steps. Each step will apply the transition rule to the cells in the grid; by examining the state of a cell along with the states of its immediately-adjacent neighbors, the transition rule will determine both how and when the cell will change state.

Each cell will exist in one of four states: Empty (nothing in the cell), Tree (the cell contains an unburned tree), Burning (the cell contains a tree that is actively burning), and Burned (the cell contains the remains of a tree that has finished burning).

Cell neighbors consist of the eight cells that are immediately adjacent to a given cell. Of course, in the case of cells along the edges of the grid, some of these neighbors do not exist.

The simulation will run a series of cycles, which consist of applying the transition rule to the current configuration and then displaying the resulting configuration and some statistics about it. This will occur in one of two modes:

Print mode, in which each configuration is printed to stdout directly.

Overlay mode, in which each configuration is printed at the top of the shell window, overwriting the previous configuration.











