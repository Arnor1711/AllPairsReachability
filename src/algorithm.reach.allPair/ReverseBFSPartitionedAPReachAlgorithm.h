//
// Created by Alwin Stockinger.
//

#ifndef ALLPAIRREACH_REVERSEBFSPARTITIONEDAPREACHALGORITHM_H
#define ALLPAIRREACH_REVERSEBFSPARTITIONEDAPREACHALGORITHM_H


#include <numeric>
#include "PartitionedAPReachAlgorithm.h"

class ReverseBFSPartitionedAPReachAlgorithm : public PartitionedAPReachAlgorithm{

public:
    bool query(Algora::Vertex *start, const Algora::Vertex *end) override;


    void setSetRemovals( bool b){
        this->setRemovals = b;
    }

    std::string getBaseName() override;

protected:

    void initializeChildStructures() override;

    void insertOverlayEdgeArcs(Algora::DiGraph *subGraph) override;

    void insertOverlayEdgeArcsOfVertex(Algora::Vertex *vertex) override;

    void removeOverlayEdgeArcs(Algora::DiGraph *subGraph) override;

    void resetChildStructures() override;

public:
    void onVertexAdd(Algora::Vertex *vertex) override;

    void onVertexRemove(Algora::Vertex *vertex) override;

public:
    std::string getName() const noexcept override;

    std::string getShortName() const noexcept override;

    unsigned long long getAmount(){
        return overlayGraph->getSize();
    }

    unsigned long long getArcAmount(){
        return overlayGraph->getNumArcs(false);
    }

    unsigned long long getSubConnectionArcs(){
        auto sum = 0ULL;
        diGraph->mapArcs([this, &sum] (Algora::Arc* arc){
            auto* tail = arc->getTail();
            auto* head = arc->getHead();

            auto* subTail = mainToSubMap[tail];
            auto* subHead = mainToSubMap[head];

            auto* overlayTail = mainToOverlayMap[tail];
            auto* overlayHead = mainToOverlayMap[head];

            if(overlayHead && overlayTail && (subHead->getParent() == subTail->getParent())){
                sum++;
            }
        });

        return sum;
    }

private:
    Algora::PropertyMap<Algora::Vertex*> subToMainMap; //can't use FastPropertyMap because vertices of subgraphs could have same id

    bool setRemovals = true;
};


#endif //ALLPAIRREACH_REVERSEBFSPARTITIONEDAPREACHALGORITHM_H
