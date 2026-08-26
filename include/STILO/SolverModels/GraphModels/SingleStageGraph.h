#pragma once
#include "STILO/SolverModels/GraphModels/Graph.h"

namespace STILO
{
    template <
        class VertexType,
        bool IsUnique>
    class SingleStageGraph final : public Graph<SingleStageGraph, VertexType, IsUnique>
    {
        using VertexClass = Vertex<VertexType>;
        using BaseClass = Graph<SingleStageGraph, VertexType, IsUnique>;
        
    public:

        SingleStageGraph(BaseClass::SolverKey, int stringLength, int stateCount) 
            : Graph<SingleStageGraph,VertexType, IsUnique>(stringLength, stateCount),
              vertices_(stateCount + 1)
        {
            this->vertexCount = stateCount + 1;            
        }

        void insertVertexExplicit(BaseClass::GraphKey, int ind1, int ind2, VertexClass* vertex)
        {
            vertices_[ind2] = vertex;
        }

        void obtainStringExplicit(BaseClass::GraphKey, std::span<int> solutionString)
        {
            if constexpr (IsUnique)
            {
                int ind;
                int pind = 0;
                int num = this->stateCount;
    
                for(int i = 0; i < this->stateCount; i++)
                {
                    this->permitted[i] = i;
                }
    
                for(int i = 0; i < this->stringLength - 1; i++)
                {
                    ind = vertices_[pind]->measure(typename VertexClass::GraphKey{}, this->permitted, num);
                    solutionString[i] = this->permitted[ind];
                    pind = solutionString[i] + 1;
                    deleteRandom(this->permitted, ind, num);
                    num--;
                }
    
                solutionString[this->stringLength - 1] = this->permitted[0];
            }
            else
            {
                int ind = 0;

                for(int i = 0; i < this->stringLength; i++)
                {
                    solutionString[i] = vertices_[ind]->measure(typename VertexClass::GraphKey{});
                    ind = solutionString[i] + 1;
                }
            }
        }

        void updateGraphExplicit(BaseClass::GraphKey, double value, const std::span<const int> list)
        {
            int ind = 0;
    
            for(int i = 0; i < this->stringLength; i++)
            {
                vertices_[ind]->addUpdate(typename VertexClass::GraphKey{}, value, list[i]);
                ind = list[i] + 1;
            }
        }

    private:

        std::vector<VertexClass*> vertices_;
    };
}