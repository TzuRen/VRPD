//
//  ModifiedPostsert.cpp
//  VRPD
//
//  Created by Xiaoya Wei on 5/3/15.
//  Copyright (c) 2015 Xiaoya Wei. All rights reserved.
//

#include <stdio.h>
#include "VRPH.h"

bool Postsert::evaluate(class VRP *V, int u, int i, VRPMove *M)
{    
    ///
    /// Evaluates the move of placing u AFTER node i in 
    /// whatever route node i is currently in.  If a move
    /// is found, the relevant solution modification information
    /// is placed in the VRPMove M
    ///    

    if (evaluateLocked) {
        return true;
    }
    
    if(V->routed[u]==false || V->routed[i]==false)
        return false;

    int start_u,end_u, start_i, end_i;
    int n,t,v,j,load_change, i_load, u_load, i_route, u_route;
    double tu, uv, tv, iu, ju, ij;
    double ui,uj,ti,savings;
    double u_loss, i_gain, i_change, i_length, u_length;

    n= V->num_nodes;
    i_route= V->route_num[i];
    u_route= V->route_num[u];    

    if(V->next_array[i]==u)
    {
        // Nothing to do - 
        return false;
    }


    // Can check load feasibility easily
    if(u_route != i_route)
    {
        if(V->route[i_route].load+V->nodes[u].demand>V->max_veh_capacity)
            return false;
    }

    // First, calculate the relevant nodes and distances
    t=VRPH_MAX(V->pred_array[u],0);
    v=VRPH_MAX(V->next_array[u],0);
    j=VRPH_MAX(V->next_array[i],0);

    tu= V->d[t][u];
    uv= V->d[u][v];
    tv= V->d[t][v];
    iu= V->d[i][u];
    ui= V->d[u][i];
    uj= V->d[u][j];
    ju= V->d[j][u];
    ij= V->d[i][j];
    ti= V->d[t][i];

    u_loss = tu+uv-tv;
    i_gain = iu+uj-ij;
    savings = i_gain-u_loss;



    if(v==i)
    {
        if(u_route!=i_route)
        {
            fprintf(stderr,"POSTSERT:: error in routes\ntuv=%d-%d-%d; ij=%d-%d; u_route=%d; i_route=%d\n",
                t,u,v,i,j,u_route, i_route);
            report_error("%s: intra/inter conflict occurred\n",__FUNCTION__);
        }

        // We have t-u-i-j and we make
        //         t-i-u-j
        // Must be in same route.
        savings = (ti+iu+uj)-(tu+ui+ij);
    }

    start_u= V->route[u_route].start;
    start_i= V->route[i_route].start;
    end_u= V->route[u_route].end;
    end_i= V->route[i_route].end;

    if(u_route==i_route)
    {
        // u and i were in the same route originally
        // No overall change in the load here - we just shifted u around
        load_change = 0;
        // The total length of route i is now old_length + savings
        i_change = savings; 
        i_length = V->route[i_route].length + i_change;
        i_load = V->route[i_route].load;
        // Same route
        u_length = i_length;
        u_load = i_load;
    }
    else
    {
        // Different routes
        i_change = i_gain;
        load_change = V->nodes[u].demand; 

        i_length = V->route[i_route].length + i_change;
        i_load = V->route[i_route].load + load_change;
        u_length = V->route[u_route].length - u_loss;
        u_load = V->route[u_route].load - load_change;
    }

    // Check feasibility  
    if( (i_length > V->max_route_length) || (u_length > V->max_route_length) || 
        (i_load > V->max_veh_capacity)   || (u_load > V->max_veh_capacity) )
    {
        return false;
    }

    // else the move is feasible - record the move 


    if(u_route==i_route)
    {

        M->num_affected_routes=1;
//        M->savings=i_gain-u_loss;
        M->route_nums[0]=u_route;
        M->route_lens[0]=u_length;
        M->route_loads[0]=u_load;
        M->route_custs[0]= V->route[u_route].num_customers; // no change
        M->new_total_route_length= V->total_route_length+M->savings;
        M->total_number_of_routes = V->total_number_of_routes;
        M->move_type=POSTSERT;
        M->num_arguments=2;
        M->move_arguments[0]=u;
        M->move_arguments[1]=i;
    }
    else
    {
        // Different routes
        if(start_u==end_u)
            M->total_number_of_routes = V->total_number_of_routes-1;
        else
            M->total_number_of_routes = V->total_number_of_routes;


        M->num_affected_routes=2;
//        M->savings=i_gain-u_loss;
        M->route_nums[0]=u_route;
        M->route_nums[1]=i_route;
        M->route_lens[0]=u_length;
        M->route_lens[1]=i_length;
        M->route_loads[0]=u_load;
        M->route_loads[1]=i_load;
        if(u!= V->dummy_index)
        {
            M->route_custs[0] = V->route[u_route].num_customers-1;
            M->route_custs[1]=  V->route[i_route].num_customers+1;
        }
        else
        {
            M->route_custs[0] = V->route[u_route].num_customers;
            M->route_custs[1]=  V->route[i_route].num_customers;
        }

        M->new_total_route_length= V->total_route_length+M->savings;
        M->move_type=POSTSERT;
        M->num_arguments=2;
        M->move_arguments[0]=u;
        M->move_arguments[1]=i;
    }

    //Modify the calculation of M.savings
    evaluateLocked = true;
    int *tmpSol = new int[V->num_original_nodes + 2];
    double oldObject = V->getCurrentObject();
    V->export_solution_buff(tmpSol);
    move(V, M->move_arguments[0], M->move_arguments[1]);
    double newObject = V->vrpd->getDroneDeploymentsolution(*V);
    M->savings = newObject - oldObject;
    V->import_solution_buff(tmpSol);
    delete [] tmpSol;
    evaluateLocked = false;

    return true;

}

