


# NDSR algorithms:
This repository contains the code associated with the paper "Network Design with Service Requirements: Scaling-up the Size of Solvable Problems” (to appear on INFORMS Journal on Computing). A preprint is available at https://arxiv.org/abs/2107.01101 

### Input Data

All the input files are stored in the `data` folder.

The files have the following structure. They have the list of Parameters, Commodities and Arcs.
```
*Parameters
ProblemName RBWQN30A120C90W2MM_S5138
netGenMethod 2
netConnMethod 1
comGenMethod 1
weightlimitGenMethod 0
num_weights 2
num_nodes 30
num_arcs 120
num_commodities 90
seed_num 5138
arc_span 2
num_comm_hop_lb 1
num_comm_hop_ub 1
q_lb 15.00
q_ub 25.00
rw_lb 0.00
rw_ub 1.00
demand_lb 1.00
demand_ub 1.00
variableCostAlpha 0.10
variableCostBeta 0.10000
rewire_ratio 0.10
euc_random_ratio 0.50
Fcost_random_ratio 0.50
cost_scale 100
weight_scale 200
*Commodities: Index fromNode toNode demand weightLimit(s) qthValue
1 19 10 1.000 568 905 20
2 7 3 1.000 862 1157 22
3 16 21 1.000 726 560 23
.....
.....
.....
90 6 28 1.000 434 640 19
*Arcs: Index fromNode toNode fixedCost variableCost Weight(s)
1 28 1 78 9 67 198
2 28 24 45 9 79 65
3 1 22 32 5 78 145
.....
.....
.....
120 4 18 52 7 206 55
```

The `*Parameters` section identifies the various paramters that were used to generate the problem data. The `num_weights`, `num_nodes`, `num_arcs`, `num_commodities` are self explanatory. The `seed_num` informs us the seed that is used to generate the data. The `Fcost_random_ratio` is used for weight generation, negatively correlated with the arc fixed costs. The `q_lb` and `q_ub` are the qth shortest path or multiplier lower bound/ upper bound for weight limit. For the other parameters, please take a look at https://pubsonline.informs.org/doi/abs/10.1287/opre.2016.1579
 

Each line in the `*Commodities:`  section starts with the index of the commodity, the start node, the terminal node, the demand (which is always 1.0), the weight limits (2 weights are considered) and the qth value tells us which path is choses between `q_lb` and `q_ub` for the weight generation. 

Similarly, each line in The `*Arcs` section starts with the index of the arc, the tail and head of the arc, the fixed cost, variable cost and the resources/weights (2 here) that will be consumed when a commidity is routed through this arc. 

All the algorithms have a built in reader that reads these files and creates the appropriate datastructures that will then be used to solve the problems. 



### NDSR_GRB_ALLPATHS
#### Building the code
After cloning the repository, in the `NDSR_GRB_allpaths` directory, type
```
$ mkdir build
$ cd build
$ cmake ..
$ make
```

##### Running the code
To run, in the build directory, type
```
$ bin/NDSR_grb_allpaths ../../data/RBWQN30A120C90W2MM_S5138_net.txt 1.25
```
where the last number (1.25 in this case) is a weight multiplier ($\alpha$) in Table 3

### GRB_BASEMODEL

#### Building and running the code
Ensure that gurobi is installed

you can run using following commabds

```
python3 NDSR_basemodel.py ../data/RBWQN30A120C90W2MM_S5138_net.txt 1.0      
```

### SCIP_BRANCH_AND_PRICE
#### Building the code

Ensure that scip is installed. 

Just type 
`
make
`

in the SCIP_BranchPrice directory. It will create the necessary `bin` and `obj` directories. 

##### Running the code
```
bin/NDSR_only_colgen_MM_pricer_cutoff_w_presol ../data/RBWQN30A120C90W2MM_S5138_net.txt 1.0
```

for any assistance, contact chaitanyagudapati [at] gmail [dot] com or gudapatichaitanya [at] outlook [dot] com


