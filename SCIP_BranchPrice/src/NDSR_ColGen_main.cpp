//
//  main.cpp
//  NDSR_ColGen
//
//  Created by Naga V Gudapati on 12/26/19.
//  Copyright © 2019 Naga V Gudapati. All rights reserved.
//

#include <iostream>
#include <vector>

#include "Graph_Reader.h"
#include "NDSR_ColGen_Solver.h"
#include "NDSR_Instance.hpp"
#include "NDSR_Reader.h"

#include <scip/scip.h>
#include <scip/scipdefplugins.h>

int execmain(int argc, const char* argv[])
{
        double weight_multiplier = 1.00;
        if (argc > 2)
        {
                weight_multiplier = std::stod(argv[2]);
        }

        std::cout << "weight multiplier: " << weight_multiplier << std::endl;

        auto inst = read_instance_NDSR(argv[1], weight_multiplier);

        auto rcspp_graph = read_instance_RCSPP_graph(argv[1]);

        bool continuous_flag = false;

        bool include_3_or_flag     = true;
        bool include_1_cut_if_flag = true;
        bool include_1_or_if_flag  = true;

        int ret_code = SCIP_solve_NDSR_colgen(inst, rcspp_graph, continuous_flag, include_3_or_flag, include_1_cut_if_flag, include_1_or_if_flag);
        return ret_code;
}

int main(int argc, const char* argv[])
{

        if (argc < 2)
        {
                std::cout << "You need to supply the files name";
                return -1;
        }

        return execmain(argc, argv) != SCIP_OKAY ? 1 : 0;
}