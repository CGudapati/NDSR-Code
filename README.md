


# NDSR algorithms:
The code for the paper for https://arxiv.org/abs/2107.01101


## NDSR_GRB_ALLPATHS
#### Building the code
After cloning the repositry, in the `NDSR_GRB_allpaths` directory, type
```
$ mkdir build
$ cd build
$ cmake ..
$ make
```

##### Running the code
To run, in the build directory, type
```
$ bin/NDSR_grb_allpaths ../../data/RBWQN30A120C90W2MM_S5138_net.txt
```

## GRB_BASEMODEl

#### Building and running the code
Ensure that gurobi is installed

you can run using following commabds

```
python3 NDSR_basemodel.py ../data/RBWQN30A120C90W2MM_S5138_net.txt 1.0      
```

## SCIP_BRANCH_AND_PRICE
#### Building the code

Ensure that scip is installed. 

Just type 
`
make
`

in the SCIP_BranchPrice directory. It will create the necessary `bin` and `obj` directories. 

##### Running the code
```
bin/NDSR_only_colgen_MM_pricer_cutoff_w_presol ../data/RBWQN30A120C90W2MM_S5138_net.txt
```

for any assistance, contact chaitanyagudapati [at] gmail [dot] com or gudapatichaitanya [at] outlook [dot] com


