/* MAM2_Agent.h */

#ifndef MAM2_AGENT
#define MAM2_AGENT

#include "repast_hpc/AgentId.h"
#include "repast_hpc/SharedContext.h"
#include "repast_hpc/SharedNetwork.h"
#include "MAM2_Network.h"

/* Agents */
class RepastHPCDemoAgent{
	
private:
	repast::AgentId   id_;
	float               c; //cycle chance
	double          total;
	int	     location;
	int		  age;
	
public:
    RepastHPCDemoAgent(repast::AgentId id);
	RepastHPCDemoAgent(){}
    RepastHPCDemoAgent(repast::AgentId id, float newC, double newTotal, int newLocation, int newAge);
	
    ~RepastHPCDemoAgent();
	
    /* Required Getters */
    virtual repast::AgentId& getId(){                   return id_;    }
    virtual const repast::AgentId& getId() const {      return id_;    }
	
    /* Getters specific to this kind of Agent */
    float getC(){                                       return c;      }
    double getTotal(){                                  return total;  }
    int getLocation(){					return location;}
    int getAge(){                                       return age; }
	
    /* Setter */
    void set(int currentRank, float newC, double newTotal, int newLocation, int newAge);
	
    /* Actions */
    void update();                                                 // Will indicate whether the agent cooperates or not; probability determined by = c / total
    void locationImpact();
    void newLocationImpact();
    void ageImpact();
  //double agentCalcs();
    void timeIncrease();
    void play(repast::SharedNetwork<RepastHPCDemoAgent,
              DemoModelCustomEdge<RepastHPCDemoAgent>,
              DemoModelCustomEdgeContent<RepastHPCDemoAgent>,
              DemoModelCustomEdgeContentManager<RepastHPCDemoAgent> > *network);
	
};

/* Serializable Agent Package */
struct RepastHPCDemoAgentPackage {
	
public:
    int    id;
    int    rank;
    int    type;
    int    currentRank;
    float  c;
    double total;
    int    location;
    int    age;
	
    /* Constructors */
    RepastHPCDemoAgentPackage(); // For serialization
    RepastHPCDemoAgentPackage(int _id, int _rank, int _type, int _currentRank, float _c, double _total, int _location, int _age);
	
    /* For archive packaging */
    template<class Archive>
    void serialize(Archive &ar, const unsigned int version){
        ar & id;
        ar & rank;
        ar & type;
        ar & currentRank;
        ar & c;
        ar & total;
	ar & location;
	ar & age;
    }
	
};


#endif
