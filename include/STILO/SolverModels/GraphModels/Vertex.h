#pragma once
#include <vector>

namespace STILO
{
    template <class VertexType>
    class Vertex
    {
    public:

        class GraphKey
        {
        private:
            
            GraphKey() {}

            template <
                class GVertexType,
                bool GIsUnique>
            friend class MultiStageGraph;

            template <
                class GVertexType,
                bool GIsUnique>
            friend class SingleStageGraph;
        };

        int measure(GraphKey)
        {
            return static_cast<VertexType*>(this)->measureExplicit(typename VertexType::VertexKey{});
        }

        int measure(GraphKey, const std::vector<int>& permitted, int pnum)
        {
            return static_cast<VertexType*>(this)->measureExplicit(typename VertexType::VertexKey{}, permitted, pnum);
        }

        void addUpdate(GraphKey, double value, int edge)
        {
            return static_cast<VertexType*>(this)->addUpdateExplicit(typename VertexType::VertexKey{}, value, edge);
        }
        
        virtual ~Vertex() = default;

    protected:

        int stateCount;

        Vertex(int stateCount)
            : stateCount(stateCount) { }
    };
}