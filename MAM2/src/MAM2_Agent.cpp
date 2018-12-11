/* MAM2_Agent.cpp */

#include "MAM2_Agent.h"

RepastHPCDemoAgent::RepastHPCDemoAgent(repast::AgentId id): id_(id), c(round(float(rand()% 101)/100)), total(50){ } 

RepastHPCDemoAgent::RepastHPCDemoAgent(repast::AgentId id, float newC, double newTotal): id_(id), c(newC), total(newTotal){ }

RepastHPCDemoAgent::~RepastHPCDemoAgent(){ }


void RepastHPCDemoAgent::set(int currentRank, float newC, double newTotal){
    id_.currentRank(currentRank);
    c     = newC;
    total = newTotal;
}

void RepastHPCDemoAgent::update(){
	if (total >= 55){
	c = 1;
	}
	else{
	c=0;
	}
	std::cout<<c;
}

void RepastHPCDemoAgent::play(repast::SharedNetwork<RepastHPCDemoAgent,
                              DemoModelCustomEdge<RepastHPCDemoAgent>,
                              DemoModelCustomEdgeContent<RepastHPCDemoAgent>,
                              DemoModelCustomEdgeContentManager<RepastHPCDemoAgent> > *network){
    std::vector<RepastHPCDemoAgent*> agentsToPlay;
    network->successors(this, agentsToPlay);

    double cPayoff     = 0;
    double totalPayoff = 0;
    std::vector<RepastHPCDemoAgent*>::iterator agentToPlay = agentsToPlay.begin();
    while(agentToPlay != agentsToPlay.end()){
        boost::shared_ptr<DemoModelCustomEdge<RepastHPCDemoAgent> > edge = network->findEdge(this, *agentToPlay);
        double edgeWeight = edge->weight();
        int confidence = edge->getConfidence();

	double newCycleChance = ((c>=1) ? ((*agentToPlay)->c>=1 ? 1.1 : 0.9): 	//I cycle, does my neighbour?
					  ((*agentToPlay)->c>=1 ? 1.1 : 0.9)); //I don't cycle, does my neighbour?      
	 
	totalPayoff +=newCycleChance;

	agentToPlay++;
    }
    
	totalPayoff = totalPayoff/2;
	//std::cout<<"total P = "<<totalPayoff;         
	total*= totalPayoff;
    	//total  *= totalPayoff;
        //std::cout<< c;
    
	
}


/* Serializable Agent Package Data */

RepastHPCDemoAgentPackage::RepastHPCDemoAgentPackage(){ }

RepastHPCDemoAgentPackage::RepastHPCDemoAgentPackage(int _id, int _rank, int _type, int _currentRank, float _c, double _total):
id(_id), rank(_rank), type(_type), currentRank(_currentRank), c(_c), total(_total){ }
