/* MAM2_Agent.cpp */

#include "MAM2_Agent.h"

RepastHPCDemoAgent::RepastHPCDemoAgent(repast::AgentId id): id_(id), c(round(float(rand()% 101)/100)), total(50), location(rand()% 5 + 1), age(rand() % 75 + 15) { } 

RepastHPCDemoAgent::RepastHPCDemoAgent(repast::AgentId id, float newC, double newTotal, int newLocation, int newAge): id_(id), c(newC), total(newTotal), location(newLocation), age(newAge){ }

RepastHPCDemoAgent::~RepastHPCDemoAgent(){ }


void RepastHPCDemoAgent::set(int currentRank, float newC, double newTotal, int newLocation, int newAge){
    id_.currentRank(currentRank);
    c     = newC;
    total = newTotal;
    location = newLocation;
    age = newAge;
}

void RepastHPCDemoAgent::update(){
	if (total >= 55){
	c = 1;
	}
	else{
	c=0;
	}
	//std::cout<<c;
}

void RepastHPCDemoAgent::ageImpact(){
	if (age <=25) { total += 7.5;}
	else if (age >25 && age <=40) {total += 5;}
	else if (age >40 && age <=65) {total += 2.5;}
	else {total =total;} 
}

void RepastHPCDemoAgent::timeIncrease(){
	age ++;
}

/*double RepastHPCDemoAgent::agentCalcs(){
	
}*/

void RepastHPCDemoAgent::locationImpact(){

	switch(location) {
		case 1: total += 7.5; //Implementing new infrastructure in location 1 increasing cycle chance by 7.5%
			break;
		case 2: total = total; //No new infrastructure in location 2
			break;
		case 3: total = total; //No new infrastructure in location 3
			break;
		case 4: total = total; //No new infrastructure in location 4
			break;
		case 5: total += 10; //Implementing new infrastructure in location 5 increasing cycle chance by 10%
			break;
		default: 
			break;
	}	


}

void RepastHPCDemoAgent::newLocationImpact(){

	switch(location) {
		case 1: total =total; //No new infrastructure in location 1
			break;
		case 2: total = total; //No new infrastructure in location 2
			break;
		case 3: total += 15; //Implementing new infrastructure in location 3 increasing cycle chance by 15%
			break;
		case 4: total += 10; //Implementing new infrastructure in location 4 increasing cycle chance by 10%
			break;
		case 5: total = total; //No new infrastructure in location 5
			break;
		default: 
			break;
	}	

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

	double newCycleChance = ((c>=1) ? ((*agentToPlay)->c>=1 ? 1.005 : 0.998): 	//I cycle, does my neighbour?
					  ((*agentToPlay)->c>=1 ? 1.005 : 0.995)); //I don't cycle, does my neighbour?      
	 
	totalPayoff +=newCycleChance;

	agentToPlay++;
    }
    
	totalPayoff = totalPayoff/2;
	if (total >100){ total = 100;}
	else if (total <0){total = 0;}
	else{ total*= totalPayoff; }       
	
    	//total  *= totalPayoff;
        //std::cout<< c;
    
	
}


/* Serializable Agent Package Data */

RepastHPCDemoAgentPackage::RepastHPCDemoAgentPackage(){ }

RepastHPCDemoAgentPackage::RepastHPCDemoAgentPackage(int _id, int _rank, int _type, int _currentRank, float _c, double _total, int _location, int _age):
id(_id), rank(_rank), type(_type), currentRank(_currentRank), c(_c), total(_total), location(_location), age(_age){ }
