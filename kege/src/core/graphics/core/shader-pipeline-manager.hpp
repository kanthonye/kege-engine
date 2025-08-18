//
//  shader-pipeline-manager.hpp
//  graphics
//
//  Created by Kenneth Esdaile on 5/22/25.
//

#ifndef shader_pipeline_manager_hpp
#define shader_pipeline_manager_hpp

#include "../loaders/pipeline-loader.hpp"
#include "../../graphics/core/graphics-core.hpp"

namespace kege{

    class Graphics;
    
    class ShaderPipelineManager
    {
    public:

        PipelineHandle load( const std::string& name, const std::string& filename );

        void set( const std::string& name_id, kege::PipelineHandle pipeline );

        kege::PipelineHandle get( const std::string& name_id );
        kege::PipelineHandle get( int index_id );

        int getIndexOf( const std::string& name_id );
        void initalize( kege::Graphics* graphics );
        void shutdown();

        ShaderPipelineManager();

    private:

        std::unordered_map< std::string, int > _name_index_map;
        std::vector< kege::PipelineHandle > _pipelines;
        kege::Graphics* _graphics;
    };

}

#endif /* shader_pipeline_manager_hpp */
