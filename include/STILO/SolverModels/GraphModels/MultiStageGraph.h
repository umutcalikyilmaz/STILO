#pragma once
#include "STILO/SolverModels/GraphModels/Graph.h"

namespace STILO
{
    template<
        class VertexType,
        bool IsUnique>
    class MultiStageGraph final : public Graph<MultiStageGraph, VertexType, IsUnique>
    {
        using VertexClass = Vertex<VertexType>;
        using BaseClass = Graph<MultiStageGraph, VertexType, IsUnique>;

    public:

        MultiStageGraph(BaseClass::SolverKey, int stringLength, int stateCount) 
            : Graph<MultiStageGraph, VertexType, IsUnique>(stringLength, stateCount),
              vertices_(stringLength - 1, stateCount)
        {
            this->vertexCount = (stringLength - 1) * stateCount + 1;            
        }

        void insertVertexExplicit(BaseClass::GraphKey, int ind1, int ind2, VertexClass* vertex)
        {
            if(ind1 == 0)
            {
                sourceVertex_ = vertex;
            }
            else
            {
                vertices_[ind1 - 1][ind2] = vertex;
            }
        }

        void obtainStringExplicit(BaseClass::GraphKey, std::span<int> solutionString)
        {
            if constexpr (IsUnique)
            {
                int num = this->stateCount;
    
                for(int i = 0; i < this->stateCount; i++)
                {
                    this->permitted[i] = i;
                }

                int ind = sourceVertex_->measure(typename VertexClass::GraphKey{}, this->permitted, num);
                solutionString[0] = this->permitted[ind];
                int pind = solutionString[0];
                deleteRandom(this->permitted, ind, num);
                num--;

                for(int i = 0; i < this->stringLength - 2; i++)
                {
                    int ind = vertices_[i][pind]->measure(typename VertexClass::GraphKey{}, this->permitted, num);
                    solutionString[i + 1] = this->permitted[ind];
                    pind = solutionString[i + 1];
                    deleteRandom(this->permitted, ind, num);
                    num--;
                }
    
                solutionString[this->stringLength - 1] = this->permitted[0];
            }
            else
            {                
                int ind = sourceVertex_->measure(typename VertexClass::GraphKey{});

                for(int i = 0; i < this->stringLength - 1; i++)
                {
                    ind = vertices_[i][ind]->measure(typename VertexClass::GraphKey{});
                    solutionString[i + 1] = ind;
                }
            }
        }

        void updateGraphExplicit(BaseClass::GraphKey, double value, std::span<const int> list)
        {
            int ind = 0;

            sourceVertex_->addUpdate(typename VertexClass::GraphKey{}, value, list[0]);
    
            for(int i = 0; i < this->stringLength - 1; i++)
            {
                this->vertices_[i][ind]->addUpdate(typename VertexClass::GraphKey{}, value, list[i + 1]);
                ind = list[i + 1];
            }
        }
    
    private:

        Matrix<VertexClass*> vertices_;
        VertexClass* sourceVertex_;
    };
}