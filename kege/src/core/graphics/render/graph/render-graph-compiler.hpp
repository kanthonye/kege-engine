//
//  render-graph-compiler.hpp
//  KE-GE
//
//  Created by Kenneth Esdaile on 9/30/25.
//

#ifndef render_graph_compiler_hpp
#define render_graph_compiler_hpp

#include "render-pass-context.hpp"

namespace kege {

    typedef std::unordered_map< int, std::unordered_set< int > > DependencyGraph;
    
    class RenderGraphCompiler
    {
    public:

        bool compile( RenderGraph* graph );

    private:

        bool doKahnsTopologicalSort( const DependencyGraph& dependency_graph, std::vector< int >& sorted_pass_indices );
        void buildDependencyGraph( DependencyGraph& dependency_graph );

        void generateSubmitInfo( DependencyGraph& dependency_graph );
        bool resolvePhysicalResosurces( const std::vector< int >& sorted_pass_indices );
        void updateExecutionOrder( const std::vector< int >& sorted_pass_indices );
        void transitionToInitialImageLayout( const std::vector< int >& sorted_pass_indices );
        void emitBarriers( const std::vector< int >& sorted_pass_indices );
        void generateBarriers( const std::vector< int >& sorted_pass_indices );
        ImageLayout inferLayout( AccessFlags access );
        bool resolveResosurceLinks();
        Graphics* getGraphics();

    private:

        std::vector< RenderStage >* _passes;
        std::vector< RenderStage* >* _execution_order;
        std::queue< int > _ready_queue;
        RenderGraph* _graph;
    };

} // namespace kege

#endif /* render_graph_compiler_hpp */
