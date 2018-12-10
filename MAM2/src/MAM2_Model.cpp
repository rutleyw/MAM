/* MAM2_Model.cpp */

#include <stdio.h>
#include <vector>
#include <boost/mpi.hpp>
#include "repast_hpc/AgentId.h"
#include "repast_hpc/RepastProcess.h"
#include "repast_hpc/Utilities.h"
#include "repast_hpc/Properties.h"
#include "repast_hpc/initialize_random.h"
#include "repast_hpc/SVDataSetBuilder.h"

#include "MAM2_Model.h"


BOOST_CLASS_EXPORT_GUID(repast::SpecializedProjectionInfoPacket<DemoModelCustomEdgeContent<RepastHPCDemoAgent> >, "SpecializedProjectionInfoPacket_CUSTOM_EDGE");

RepastHPCDemoAgentPackageProvider::RepastHPCDemoAgentPackageProvider(repast::SharedContext<RepastHPCDemoAgent>* agentPtr): agents(agentPtr){ }

void RepastHPCDemoAgentPackageProvider::providePackage(RepastHPCDemoAgent * agent, std::vector<RepastHPCDemoAgentPackage>& out){
    repast::AgentId id = agent->getId();
    RepastHPCDemoAgentPackage package(id.id(), id.startingRank(), id.agentType(), id.currentRank(), agent->getC(), agent->getTotal());
    out.push_back(package);
}

void RepastHPCDemoAgentPackageProvider::provideContent(repast::AgentRequest req, std::vector<RepastHPCDemoAgentPackage>& out){
    std::vector<repast::AgentId> ids = req.requestedAgents();
    for(size_t i = 0; i < ids.size(); i++){
        providePackage(agents->getAgent(ids[i]), out);
    }
}


RepastHPCDemoAgentPackageReceiver::RepastHPCDemoAgentPackageReceiver(repast::SharedContext<RepastHPCDemoAgent>* agentPtr): agents(agentPtr){}

RepastHPCDemoAgent * RepastHPCDemoAgentPackageReceiver::createAgent(RepastHPCDemoAgentPackage package){
    repast::AgentId id(package.id, package.rank, package.type, package.currentRank);
    return new RepastHPCDemoAgent(id, package.c, package.total);
}

void RepastHPCDemoAgentPackageReceiver::updateAgent(RepastHPCDemoAgentPackage package){
    repast::AgentId id(package.id, package.rank, package.type);
    RepastHPCDemoAgent * agent = agents->getAgent(id);
    agent->set(package.currentRank, package.c, package.total);
}



DataSource_AgentTotals::DataSource_AgentTotals(repast::SharedContext<RepastHPCDemoAgent>* c) : context(c){ }

int DataSource_AgentTotals::getData(){
	int sum = 0;
	repast::SharedContext<RepastHPCDemoAgent>::const_local_iterator iter    = context->localBegin();
	repast::SharedContext<RepastHPCDemoAgent>::const_local_iterator iterEnd = context->localEnd();
	while( iter != iterEnd) {
		sum+= (*iter)->getTotal();
		iter++;
	}
	return sum;
}

DataSource_AgentCTotals::DataSource_AgentCTotals(repast::SharedContext<RepastHPCDemoAgent>* c) : context(c){ }

int DataSource_AgentCTotals::getData(){
	int sum = 0;
	repast::SharedContext<RepastHPCDemoAgent>::const_local_iterator iter    = context->localBegin();
	repast::SharedContext<RepastHPCDemoAgent>::const_local_iterator iterEnd = context->localEnd();
	while( iter != iterEnd) {
		sum+= (*iter)->getC();
		iter++;
	}
	return sum;
}



RepastHPCDemoModel::RepastHPCDemoModel(std::string propsFile, int argc, char** argv, boost::mpi::communicator* comm): context(comm){
	props = new repast::Properties(propsFile, argc, argv, comm);
	stopAt = repast::strToInt(props->getProperty("stop.at"));
	countOfAgents = repast::strToInt(props->getProperty("count.of.agents"));
	initializeRandom(*props, comm);
	if(repast::RepastProcess::instance()->rank() == 0) props->writeToSVFile("./output/record.csv");
	provider = new RepastHPCDemoAgentPackageProvider(&context);
	receiver = new RepastHPCDemoAgentPackageReceiver(&context);
	
  agentNetwork = new repast::SharedNetwork<RepastHPCDemoAgent, DemoModelCustomEdge<RepastHPCDemoAgent>, DemoModelCustomEdgeContent<RepastHPCDemoAgent>, DemoModelCustomEdgeContentManager<RepastHPCDemoAgent> >("agentNetwork", false, &edgeContentManager);
	context.addProjection(agentNetwork);
	
	// Data collection
	// Create the data set builder
	std::string fileOutputName("./output/agent_total_data.csv");
	repast::SVDataSetBuilder builder(fileOutputName.c_str(), ",", repast::RepastProcess::instance()->getScheduleRunner().schedule());
	
	// Create the individual data sets to be added to the builder
	DataSource_AgentTotals* agentTotals_DataSource = new DataSource_AgentTotals(&context);
	builder.addDataSource(createSVDataSource("Total", agentTotals_DataSource, std::plus<int>()));

	DataSource_AgentCTotals* agentCTotals_DataSource = new DataSource_AgentCTotals(&context);
	builder.addDataSource(createSVDataSource("C", agentCTotals_DataSource, std::plus<int>()));

	// Use the builder to create the data set
	agentValues = builder.createDataSet();
	
}

RepastHPCDemoModel::~RepastHPCDemoModel(){
	delete props;
	delete provider;
	delete receiver;
	delete agentValues;

}

void RepastHPCDemoModel::init(){
	int rank = repast::RepastProcess::instance()->rank();
	for(int i = 0; i < countOfAgents; i++){
		repast::AgentId id(i, rank, 0);
		id.currentRank(rank);
		RepastHPCDemoAgent* agent = new RepastHPCDemoAgent(id);
		context.addAgent(agent);
	}
}


void RepastHPCDemoModel::connectAgentNetwork(){
	repast::SharedContext<RepastHPCDemoAgent>::const_local_iterator iter    = context.localBegin();
	repast::SharedContext<RepastHPCDemoAgent>::const_local_iterator iterEnd = context.localEnd();
	int worldSize= repast::RepastProcess::instance()->worldSize();
	size_t i = 0;
	while(iter != iterEnd) {
		RepastHPCDemoAgent* ego = &**iter;
		std::vector<RepastHPCDemoAgent*> agents;
		agents.push_back(ego);                          // Omit self
		context.selectAgents(worldSize, agents, true);          // Choose 5 other agents randomly
		// Make an undirected connection
		repast::AgentId toDisplay(i, 0, 0);
		RepastHPCDemoAgent* agent = context.getAgent(toDisplay); //CHANGE NAME OF TODISPLAY
            if(agent->getId().id()<536){
		repast::AgentId toDisplay((i+1), 0, 0);
		RepastHPCDemoAgent* agent1 = context.getAgent(toDisplay); //CHANGE NAME OF TODISPLAY
              std::cout << "CONNECTING: " << agent->getId() << " to " << agent1->getId() << std::endl;
              boost::shared_ptr<DemoModelCustomEdge<RepastHPCDemoAgent> > demoEdge(new DemoModelCustomEdge<RepastHPCDemoAgent>(agent, agent1, i+1, i*i));
  	  	      agentNetwork->addEdge(demoEdge);
            }
	else if(agent->getId().id() == 536){
		repast::AgentId toDisplay(0, 0, 0);
		RepastHPCDemoAgent* agent3 = context.getAgent(toDisplay); //CHANGE NAME OF TODISPLAY
              std::cout << "CONNECTING2: " << agent->getId() << " to " << agent3->getId() << std::endl;
              boost::shared_ptr<DemoModelCustomEdge<RepastHPCDemoAgent> > demoEdge(new DemoModelCustomEdge<RepastHPCDemoAgent>(agent, agent3, i+1, i*i));
  	  	      agentNetwork->addEdge(demoEdge);
		}
		i++;
		iter++;
	}
}


void RepastHPCDemoModel::doSomething(){
	int whichRank = 0;
	if(repast::RepastProcess::instance()->rank() == whichRank) std::cout << " TICK " << repast::RepastProcess::instance()->getScheduleRunner().currentTick() << std::endl;

	if(repast::RepastProcess::instance()->rank() == whichRank){
		std::cout << "LOCAL AGENTS:" << std::endl;
		for(int r = 0; r < 4; r++){
			for(int i = 0; i < 10; i++){
				repast::AgentId toDisplay(i, r, 0);
				RepastHPCDemoAgent* agent = context.getAgent(toDisplay);
				if((agent != 0) && (agent->getId().currentRank() == whichRank)) std::cout << agent->getId() << " " << agent->getC() << " " << agent->getTotal() << std::endl;
			}
		}

	}
	
	std::vector<RepastHPCDemoAgent*> agents;
	context.selectAgents(countOfAgents, agents);
	std::vector<RepastHPCDemoAgent*>::iterator it = agents.begin();
	while(it != agents.end()){
		(*it)->play(agentNetwork);
		it++;
    }

	repast::RepastProcess::instance()->synchronizeAgentStates<RepastHPCDemoAgentPackage, RepastHPCDemoAgentPackageProvider, RepastHPCDemoAgentPackageReceiver>(*provider, *receiver);

}

void RepastHPCDemoModel::initSchedule(repast::ScheduleRunner& runner){
    runner.scheduleEvent(1, repast::Schedule::FunctorPtr(new repast::MethodFunctor<RepastHPCDemoModel> (this, &RepastHPCDemoModel::connectAgentNetwork)));
	runner.scheduleEvent(2, 1, repast::Schedule::FunctorPtr(new repast::MethodFunctor<RepastHPCDemoModel> (this, &RepastHPCDemoModel::doSomething)));
	runner.scheduleEndEvent(repast::Schedule::FunctorPtr(new repast::MethodFunctor<RepastHPCDemoModel> (this, &RepastHPCDemoModel::recordResults)));
	runner.scheduleStop(stopAt);
	
	// Data collection
	runner.scheduleEvent(1.5, 5, repast::Schedule::FunctorPtr(new repast::MethodFunctor<repast::DataSet>(agentValues, &repast::DataSet::record)));
	runner.scheduleEvent(10.6, 10, repast::Schedule::FunctorPtr(new repast::MethodFunctor<repast::DataSet>(agentValues, &repast::DataSet::write)));
	runner.scheduleEndEvent(repast::Schedule::FunctorPtr(new repast::MethodFunctor<repast::DataSet>(agentValues, &repast::DataSet::write)));
}

void RepastHPCDemoModel::recordResults(){
	if(repast::RepastProcess::instance()->rank() == 0){
		props->putProperty("Result","Passed");
		std::vector<std::string> keyOrder;
		keyOrder.push_back("RunNumber");
		keyOrder.push_back("stop.at");
		keyOrder.push_back("Result");
		props->writeToSVFile("./output/results.csv", keyOrder);
    }
}


	
