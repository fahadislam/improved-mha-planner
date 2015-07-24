/*
 * Copyright (c) 2013, Mike Phillips and Maxim Likhachev
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the University of Pennsylvania nor the names of its
 *       contributors may be used to endorse or promote products derived from
 *       this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef _MHA_PLANNER_H_
#define _MHA_PLANNER_H_

#include "../../sbpl/headers.h"
#include <sbpl/discrete_space_information/environment_mha.h>
#include <queue>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

class MHALazyListElement;

namespace mha_planner
{
  enum MetaSearchType
  {
    ROUND_ROBIN,
    META_A_STAR,
    DTS
  };

  enum PlannerType
  {
    IMHA,
    SMHA
  };

  enum MHAType
  {
    ORIGINAL,
    PLUS,
    FOCAL,
    UNCONSTRAINED,
    GBFS
  };
}

class MHAReplanParams: public ReplanParams
{
  public:
    MHAReplanParams(double allocated_time): ReplanParams(allocated_time)
  {
    inflation_eps = 1.0;
    anchor_eps = 1.0;
    use_anchor = true;
    meta_search_type = mha_planner::MetaSearchType::ROUND_ROBIN;
    planner_type = mha_planner::PlannerType::SMHA;
    mha_type = mha_planner::MHAType::PLUS;
  };
    double inflation_eps, anchor_eps;
    bool use_anchor;
    mha_planner::MetaSearchType meta_search_type;
    mha_planner::PlannerType planner_type;
    mha_planner::MHAType mha_type;
};

class MHAState: public AbstractSearchState{     //fahad
  public:
    int id;
    int q_id;       //doesnt matter i think
    unsigned int v;     //v[2]  
    unsigned int g;     //g[2]
    int h;      //h[2]
    short unsigned int iteration_closed;        //doesnt matter i think
    short unsigned int replan_number;           //doesnt matter i think
    MHAState* best_parent;      //best_parent[2]
    MHAState* expanded_best_parent;     //expanded_best_parent[2]
    bool in_incons;         //in_incons[2]
    std::priority_queue<MHALazyListElement> lazyList;       //doesnt matter i think
    bool isTrueCost;    //isTrueCost[2]
};

class BestHState: public AbstractSearchState{
  public:
    int id;
};

class MHALazyListElement{
  public:
    MHALazyListElement(MHAState* p, int ec, bool itc){
      parent = p;
      edgeCost = ec;
      isTrueCost = itc;
    }
    bool operator< (const MHALazyListElement& other) const{
      return (parent->v + edgeCost > other.parent->v + other.edgeCost);
    }
    MHAState* parent;
    int edgeCost;
    bool isTrueCost;
};

class MHAPlanner : public SBPLPlanner{

  public:
    virtual int replan(double allocated_time_secs, std::vector<int>* solution_stateIDs_V){
      printf("Not supported. Use MHAReplanParams");
      return -1;
    };
    virtual int replan(double allocated_time_sec, std::vector<int>* solution_stateIDs_V, int* solcost){
      printf("Not supported. Use MHAReplanParams");
      return -1;
    };

    virtual int replan(int start, int goal, std::vector<int>* solution_stateIDs_V, ReplanParams params, int* solcost)
    {
      printf("Not supported. Use MHAReplanParams");
      return -1;
    }
    virtual int replan(std::vector<int>* solution_stateIDs_V, ReplanParams params)
    {
      printf("Not supported. Use MHAReplanParams");
      return -1;
    }
    virtual int replan(std::vector<int>* solution_stateIDs_V, ReplanParams params, int* solcost)
    {
      printf("Not supported. Use MHAReplanParams");
      return -1;
    }

    virtual int replan(int start, int goal, std::vector<int>* solution_stateIDs_V, MHAReplanParams params, int* solcost);
    virtual int replan(std::vector<int>* solution_stateIDs_V, MHAReplanParams params);
    virtual int replan(std::vector<int>* solution_stateIDs_V, MHAReplanParams params, int* solcost);

    void interrupt();

    virtual int set_goal(int goal_stateID);
    virtual int set_start(int start_stateID);

    virtual void costs_changed(StateChangeQuery const & stateChange){return;};
    virtual void costs_changed(){return;};

    virtual int force_planning_from_scratch(){return 1;};
    virtual int force_planning_from_scratch_and_free_memory(){return 1;};

    virtual int set_search_mode(bool bSearchUntilFirstSolution){
      printf("Not supported. Use MHAReplanParams");
      return -1;
    };

    virtual void set_initialsolution_eps(double initialsolution_eps){
      printf("Not supported. Use MHAReplanParams");
    };

    MHAPlanner(EnvironmentMHA* environment, int num_heuristics, bool bforwardsearch, bool biDirectional);
    ~MHAPlanner();

    virtual void get_search_stats(std::vector<PlannerStats>* s);

  protected:
    //data structures (open and incons lists)
    std::vector<CHeap> heaps;    
    std::vector<CHeap> heapss[2];    //fahad
    std::vector<std::vector<MHAState*>> incons;
    std::vector<std::vector<MHAState*>> states;
    std::vector<std::vector<MHAState*>> statess[2];
    std::vector<std::vector<BestHState*>> best_h_states; 
    std::vector<std::vector<BestHState*>> best_h_statess[2];    //fahad    

    //params
    MHAReplanParams params;
    bool bforwardsearch; //if true, then search proceeds forward, otherwise backward
    bool bidirectional;     //fahad
    MHAState* start_state;
    MHAState* goal_state; 
    MHAState* goal_states[2];    //fahad      
    MHAState* start_states[2];      
    int min_state_ids[2][2];    //fahad
    int goal_state_id;      
    int start_state_id;
    int intersection_state_id;  
    unsigned int best_path_length;   

    //mha params
    EnvironmentMHA* env_;
    int num_heuristics;
    double inflation_eps, anchor_eps;
    bool use_anchor;
    mha_planner::PlannerType planner_type;
    mha_planner::MetaSearchType meta_search_type;
    mha_planner::MHAType mha_type;
    bool use_lazy_;

    // meta-A* variables
    std::vector<int> queue_expands;
    std::vector<int> queue_expandss[2]; //fahad
    std::vector<int> queue_best_h_dts;      //todo
    std::vector<CHeap> queue_best_h_meta_heaps;
    std::vector<CHeap> queue_best_h_meta_heapss[2];  //fahad
    bool intersect_flag;
    int max_edge_cost;
    std::vector<int> max_heur_dec;

    // Dynamic Thompson Sampling variables
    std::vector<double> alpha;
    std::vector<double> beta;
    double betaC;
    const gsl_rng_type* gsl_rand_T;
    gsl_rng* gsl_rand;


    //search member variables
    double eps_satisfied;
    int search_expands;
    clock_t TimeStarted;
    short unsigned int search_iteration;
    short unsigned int replan_number;
    bool use_repair_time;

    //stats
    std::vector<PlannerStats> stats;
    unsigned int totalExpands;
    double totalTime;
    double totalPlanTime;
    double reconstructTime;

    bool interruptFlag;

    void checkHeaps(std::string msg);

    virtual MHAState* GetState(int backward_forward, int q_id, int id);   //fahad
    virtual BestHState* GetBestHState(int backward_forward, int q_id, int id);
    virtual void ExpandState(int backward_forward, int q_id, MHAState* parent); //fahad
    virtual void EvaluateState(int backward_forward, int q_id, MHAState* parent);
    void getNextLazyElement(int backward_forward, int q_id, MHAState* state);
    void insertLazyList(int backward_forward, int q_id, MHAState* state, MHAState* parent, int edgeCost, bool isTrueCost);
    void putStateInHeap(int backward_forward, int q_id, MHAState* state);

    virtual int ImprovePath();

    virtual std::vector<int> GetSearchPath(int backward_forward, int& solcost); //todo
    virtual std::vector<int> GetBidirectionalSearchPath(int id, int& solcost); //todo

    virtual bool outOfTime();
    virtual void initializeSearch();
    virtual void prepareNextSearchIteration();
    virtual bool Search(std::vector<int>& pathIds, int & PathCost);

    // MHA-specific
    virtual int GetBestHeuristicID(int backward_forward);
    virtual bool UpdateGoal(MHAState* state);
};

#endif
