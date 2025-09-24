//
//  shader-pipeline-manager.hpp
//  graphics
//
//  Created by Kenneth Esdaile on 5/22/25.
//

#ifndef shader_pipeline_manager_hpp
#define shader_pipeline_manager_hpp

#include "shader-pipeline.hpp"

namespace kege{

    class Graphics;
    
    class ShaderPipelineManager
    {
    public:

        struct PipelineObject
        {
            std::vector< PipelineContext > pipeline_contexts;
            int duplicates;
        };

    public:

        int set( const std::string& name_id, const kege::CreateShaderPipelineInfo& info, const std::vector< kege::PipelineHandle >& pipeline );
        kege::ShaderPipeline load( const std::string& name, const std::string& filename );
        kege::ShaderPipeline load( const std::string& filename );

        kege::ShaderPipeline get( const std::string& name );
        kege::ShaderPipeline get( int index_id );

        void destroy( const kege::ShaderPipeline& shader );
        
        int getIndexOf( const std::string& name_id );

        void initalize( kege::Graphics* graphics );
        void shutdown();

        ShaderPipelineManager();

    private:

    private:

        std::unordered_map< std::string, int > _name_index_map;
        std::vector< PipelineObject > _pipeline_objects;
        kege::Graphics* _graphics;

        friend kege::ShaderPipeline;
    };

}

#endif /* shader_pipeline_manager_hpp */
