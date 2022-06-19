# ai-agent
Simulating AI-based agents

## Goals

1. The neural network (brain) has one input layer, 3 hidden layers and one output layer. 
2. A Genome define the connections between the neurons. The number of genes is limited.
3. The simulation 

# Concept and Design

## Rules

1. Agents are placed randomly on a 2D world at the beginning of a cycle.
2. Agents can reproduce at the end of a cycle before they die.
3. Agents need to reach a target area where they can reproduce.
4. Accelerating (a>0) costs energy. Energy is replenished while stationary (v=0).
5. Friction reduces the velocity over time if agents do not accelerate.
6. Agents can not move into or over each other.
7. Target areas emit a field that permeates the world and can be sensed by the agents.
8. A cycle consists of $N_{Cy}$ steps.

## Input Neurons

Short | Name            | Description | Range 
------|-----------------|-------------|------
TDe   | Target field Density | |
TGH   | Target field Gradient Heading | |
TGL   | Target field gradient Lateral | |
Vel   | Velocity | | 
Hdg   | Heading  | | 
Age   | Age | |
Rnd   | Random Value | |
Nrg   | Energy | |
Pop   | Population Density | Averaged over a NxN Grid |
PGH   | Population Gradient Heading | along the current heading over a NxN grid
PGL   | Population Gradient Lateral | lateral to current heading over a NxN Grid |
Osc   | Oscillator | |
Blk   | Blockage | Path (heading) is blocked Within N gid-units. | 0..1
BLt   | Blockage Lateral | Blockage left/right with respect to heading over NxN grid | 

## Output Neurons

Short | Name            | Description | Range 
------|-----------------|-------------|------
Acc   | Accelerate | | 
Rot   | Rotate | |

##  Gene encoding

A gene is encoded as a 32-bit unsigned integer (`uint32_t`)

```
0 0000000 0 0000000 0000000000000000
|    |    |    |           L 16-bit weight (-4..4)
|    |    |    L 7-bit sinkID
|    |    L sinkType (0=Hidden, 1=Output)
|    L 7-bit sourceID
sourceType (0=Hidden, 1=Input)
```

## Mutations

During reproduction, a random bit in the genome of an agent is flipped. These mutations occur with a defined probability.

## Simulation

# Compiling

```
git submodule update --init --recursive
mkdir build && cd build
cmake ..
make
./project
```

# glfw

```
cd path/to/glfw
cmake -S . -B build
cd build
make
```

# glad

see [glad](https://github.com/Dav1dde/glad)

# References

## Idea

## GLFW

- [How to build & install GLFW 3 and use it in a Linux project](https://stackoverflow.com/questions/17768008/how-to-build-install-glfw-3-and-use-it-in-a-linux-project)

## Dear ImGui

- [GLFW and Dear ImGui](https://decovar.dev/blog/2019/08/04/glfw-dear-imgui/)
- [An introduction to the Dear ImGui library](https://blog.conan.io/2019/06/26/An-introduction-to-the-Dear-ImGui-library.html)

## AI-Agents and Simulations

- [Pezzza's Work: Evolving AIs - Predator vs Prey, who will win?](https://www.youtube.com/watch?v=qwrp3lB-jkQ)
- [davidrandallmiller: I programmed some creatures. They Evolved.](https://www.youtube.com/watch?v=N3tRFayqVtk)

## Machine Learning and Math

- [Wikipedia: Activation function](https://en.wikipedia.org/wiki/Activation_function)

## Graphics Resources 

- [The JetBrains Mono - A typeface for developers](https://www.jetbrains.com/lp/mono)