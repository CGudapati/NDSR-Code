//
//  RCSPP_GRAPH_READER.hpp
//  NDSR_basemodel
//
//  Created by Naga V Gudapati on 12/17/19.
//  Copyright © 2019 Naga V Gudapati. All rights reserved.
//

#ifndef RCSPP_GRAPH_READER_h
#define RCSPP_GRAPH_READER_h

#include "RCSPP_graph.hpp"

#include <fstream>
#include <sstream>

std::vector<std::string> split ( const std::string& s, char delimiter )
{
        std::vector<std::string> tokens;
        std::string              token;
        std::istringstream       tokenStream ( s );
        while ( std::getline ( tokenStream, token, delimiter ) )
        {
                tokens.push_back ( token );
        }
        return tokens;
}

RCSPP::RCSPP_graph read_instance_RCSPP_graph ( const std::string filepath )
{

        std::ifstream infile ( filepath );
        int           N = 0;
        int           A = 0;
        int           K = 0;
        int           w = 0;

        std::string num_nodes_str ( "num_nodes" );
        std::string num_arcs_str ( "num_arcs" );
        std::string num_commodities_str ( "num_commodities" );
        std::string num_weights_str ( "num_weights" );
        std::string commodities_start_str ( "*Commodities: " );
        std::string arcs_start_str ( "*Arcs:" );

        for ( std::string line; getline ( infile, line ); )
        {
                if ( line.find ( num_nodes_str ) == 0u )
                {
                        N = std::stoi ( ( split ( line, ' ' ) ).back () );
                }

                if ( line.find ( num_arcs_str ) == 0u )
                {
                        A = std::stoi ( ( split ( line, ' ' ) ).back () );
                }

                if ( line.find ( num_commodities_str ) == 0u )
                {
                        K = std::stoi ( ( split ( line, ' ' ) ).back () );
                }

                if ( line.find ( num_weights_str ) == 0u )
                {
                        w = std::stoi ( ( split ( line, ' ' ) ).back () );
                }
        }

        RCSPP::RCSPP_graph RCSPP_graph_G ( N );

        std::ifstream infile2 ( filepath );

        for ( std::string line2; getline ( infile2, line2 ); )
        {
                if ( line2.find ( arcs_start_str ) == 0u )
                {
                        for ( int i = 0; i < A; ++i )
                        {
                                std::string arc_line = "";
                                std::getline ( infile2, arc_line );

                                auto arc_line_tokens = split ( arc_line, ' ' );

                                auto arc_tail          = std::stoi ( arc_line_tokens[1] ) - 1;
                                auto arc_head          = std::stoi ( arc_line_tokens[2] ) - 1;
                                auto arc_variable_cost = std::stod ( arc_line_tokens[4] );

                                double w1 = std::stoi ( arc_line_tokens[5] );
                                double w2 = std::stoi ( arc_line_tokens[6] );

                                RCSPP_graph_G.add_arc ( arc_tail, arc_head, arc_variable_cost, w1, w2 );
                        }
                }
        }

        RCSPP_graph_G.set_const_graphs_weights ();

        return RCSPP_graph_G;
}

#endif /* RCSPP_GRAPH_READER_h */