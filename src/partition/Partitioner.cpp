//
// Created by alwin on 6/19/19.
//

#include <iostream>
#include <spawn.h>
#include <wait.h>
#include <fstream>
#include <cstring>


#include "Partitioner.h"

Algora::FastPropertyMap<unsigned long long>
Partitioner::handlePartitioning(unsigned long long int k, Algora::DiGraph *graph) {
    std::string kahipFileName = "forKahip";

    convertDiGraphToKahip(graph, kahipFileName);
    //TODO implement Kahip options
    pid_t pid;

    std::string kahipName = "kaffpaE";
    std::string preconfig = "--preconfiguration=eco";   //TODO

    std::string kahipInputFileName = "k";
    std::string kahipArgInputFileName = "--output_filename=" + kahipInputFileName;
    std::string kahipK = "--k=" + std::to_string(k);

    char* kahipArgv[] = {kahipName.data(), kahipFileName.data(), kahipK.data(), preconfig.data(), kahipArgInputFileName.data(), nullptr};
    char* const envp[]={nullptr};
    std::cout << "\n\nStarting Kahip with k=" + std::to_string(k) <<"\n--------------KAHIP OUTPUT----------------"<< std::endl;
    int kahipStatus = posix_spawn(&pid, kahipName.data(), nullptr, nullptr, kahipArgv, envp);

    if(kahipStatus != 0 || waitpid(pid, &kahipStatus, 0) == -1){
        if(kahipStatus == 2) std::cerr << strerror(kahipStatus) << ": " << kahipName << std::endl;
        throw std::runtime_error("kahip could not be executed");
    }
    std::cout << "------------------KAHIP OUTPUT END----------------"<<std::endl;
    return makePartitionMap(kahipInputFileName, graph);
}

Algora::FastPropertyMap<unsigned long long> Partitioner::makePartitionMap(const std::string &partitionFileName, Algora::DiGraph* graph) {


    std::map<unsigned long long, Algora::Vertex*> vertices;

    unsigned long long vertexI = 0ULL;
    graph->mapVertices([&vertices, &vertexI](Algora::Vertex* vertex){
        vertices.insert({vertexI++, vertex});
    });

    Algora::FastPropertyMap<unsigned long long> partitionMap;

    std::ifstream file(partitionFileName);

    if(file.good()) {
        std::string line;
        for (unsigned long long i = 0ULL; std::getline(file, line); i++) {
            partitionMap[vertices[i]] = std::stoul(line);
        }
    }
    else{
        //TODO error
        std::cerr << "PartitionFile not good" << std::endl;
        std::cerr << "Error: " << strerror(errno);
    }

    return partitionMap;
}

void Partitioner::convertDiGraphToKahip(Algora::DiGraph *graph, const std::string& outputFileName){
    std::map<unsigned long long int, std::map<unsigned long long int, unsigned long long int>> vertexMap = generateVertexMapFromGraph(graph);
    writeMapToFile(outputFileName, graph, vertexMap);
}

std::map<unsigned long long int, std::map<unsigned long long int, unsigned long long int>> Partitioner::generateVertexMapFromGraph(
        Algora::DiGraph *graph) {


    std::map<unsigned long long , std::map<unsigned long long, unsigned long long>> vertexMap;

    graph->mapArcs([&vertexMap](Algora::Arc* arc){
        if(!arc->isLoop()){
            unsigned long long headId = arc->getHead()->getId();
            unsigned long long tailId = arc->getTail()->getId();

            if(vertexMap.find(headId) == vertexMap.end()){
                vertexMap[headId][tailId] = 1;
                vertexMap[tailId][headId] = 1;
            } else{
                vertexMap[headId][tailId]++;
                vertexMap[tailId][headId]++;
            }
        }
    });

    return vertexMap;
}

void Partitioner::writeMapToFile(const std::string &outFileName, Algora::DiGraph *graph,
                                        std::map<unsigned long long, std::map<unsigned long long, unsigned long long>> vertexMap) {


    std::ofstream outFile(outFileName, std::ios::trunc);

    unsigned long long arcSize = 0;

    for(const auto& [_,map]: vertexMap){
        arcSize += map.size();
    }
    arcSize /= 2;

    if(outFile.good() && outFile.is_open()){
        outFile << graph->getSize() << " " << arcSize << " 1\n";
        //outFile.flush();
    }

    for(unsigned long long i = 0; i < graph->getSize(); i++){
        if(vertexMap.find(i) != vertexMap.end()){
            for(const auto &[tail, weight]: vertexMap[i]){
                outFile << tail + 1 << " " << weight << " ";
            }
        }
        outFile << "\n";
        //outFile.flush();
    }

    outFile.close();
}