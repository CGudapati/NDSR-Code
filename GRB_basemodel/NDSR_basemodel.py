#!/usr/bin/env python
# coding: utf-8

from gurobipy import *
import sys


#Just settimng up the stuff
# first read will 



instance_name = sys.argv[1]
weight_multiplier = float(sys.argv[2])


filepath = instance_name


threadRestrictionFlag = True


num_commodities = 0
num_metrics = 0
num_arcs = 0
num_nodes = 0
commodity_s_and_t={}
commodity_w_limits={}
variable_costs= {} 
fixed_costs = {}
w={}


#This will get us all the necessary problme stats
with open(filepath, "r") as instance_blob:
    for line in instance_blob:
        if (line.startswith("num_commodities")):
            num_commodities = int(line.split()[1])
        if (line.startswith("num_weights")):
            num_metrics = int(line.split()[1])
        if (line.startswith("num_arcs")):
            num_arcs = int(line.split()[1])
        if (line.startswith("num_nodes")):
            num_nodes = int(line.split()[1])
        if (line.startswith("*Commodities:")):
            for i in range(num_commodities) : 
                l = next(instance_blob)
                commodity_s_and_t[int(l.split()[0])] = list(map(int, l.split()[1:3]))
                # commodity_w_limits[float(l.split()[0])] = list(map(float, l.split()[4:4+num_metrics]))
                commodity_w_limits[int(l.split()[0])] = [weight * weight_multiplier for weight in list(map(int, l.split()[4:4+num_metrics]))]

        if (line.startswith("*Arcs:")):
            for i in range(num_arcs) : #Get all the information about the arcs
                l = next(instance_blob)
                a = list(map(int,l.split()[1:3]))
                fixed_costs[tuple(a)] = int(l.split()[3:4][0])
                for i in range(num_commodities):
                    a = list(map(int,l.split()[1:3]))    #from node and two node info
                    k = [i+1]+a                         #k = for each commodity to node and from node
                    variable_costs[tuple(k)]= int(l.split()[4:5][0])
                    fixed_costs[tuple(a)]= int(l.split()[3:4][0])

                    for j in range(num_metrics):
                        key = [j+1]+[i+1]+a
                        w[tuple(key)] = float(l.split()[5+j])

print("\n")             
print("Optimizing a problem with ", num_nodes, " nodes, ", num_arcs, " arcs", " and ", num_commodities, " commdities") 
print("\n")


m = Model()


m.setParam(GRB.Param.Threads,1)
logFileName = instance_name + "."+ str(weight_multiplier) +"x.presolve_true.basemodel.log"
m.setParam("LogFile", logFileName)
m.setParam("TimeLimit", 3600)
m.setParam("MIPGap", 0.0)
m.setParam("IntFeasTol", 1e-9)
obj = LinExpr()                
x = m.addVars(variable_costs.keys(), vtype=GRB.BINARY, lb = 0.0, name='x')  #x is indexed by  commodity, from node and to node
z = m.addVars(fixed_costs.keys(), vtype=GRB.BINARY, lb = 0.0, name= "z")  #The variable z is indexed by from node and two node
m.update()

for k in range(num_commodities):
    for n in range(num_nodes):
            if ((n+1) == commodity_s_and_t[k+1][0]):
                m.addConstr(x.sum(k+1,n+1,'*') - x.sum(k+1,'*', n+1) == 1, name="FS")
            elif ((n+1) == commodity_s_and_t[k+1][1]):
                m.addConstr(x.sum(k+1,n+1,'*') - x.sum(k+1,'*', n+1) == -1, name="FT")
            else:
                m.addConstr(x.sum(k+1,n+1,'*') - x.sum(k+1,'*', n+1) == 0, name="FI")
            
for k in range(num_commodities):
    for (i,j) in fixed_costs.keys():
        m.addConstr(x[k+1, i, j] <= z[i,j])




m.update()

for k in range(num_commodities):
    for metric in range(num_metrics):
            m.addConstr(sum(w[metric+1,k+1,i,j]*x[k+1,i,j] for (i,j) in fixed_costs.keys()) <= commodity_w_limits[k+1][metric])
        

for (i,j) in fixed_costs.keys():
    obj+= fixed_costs[i,j]*z[i,j]

for k in range(num_commodities):
    for (i,j) in fixed_costs.keys():
        obj+= variable_costs[k+1,i,j]*x[k+1,i,j]


m.update()

m.setObjective(obj, GRB.MINIMIZE); 


m.update()

m.optimize()



