//
//  render-graph.cpp
//  ecs
//
//  Created by Kenneth Esdaile on 4/25/25.
//

#include "render-pass.hpp"
#include "render-graph.hpp"

namespace kege{

    kege::RgResrcHandle RenderGraph::importShaderResource( std::string name, const ShaderResrcs& handles )
    {
        RgShaderResrcDefn defn;
        defn.name = name;
        defn.physical_handles = handles;
        return defnShaderResource( defn );
    }

    const kege::ShaderResrc* RenderGraph::fetchShaderResource( const std::string& name )const
    {
        const RgShaderResrcDefn* defn = _asset_manager.fetch< RgShaderResrcDefn >( name );
        uint32_t frame_index = _graphics->getCurrFrameIndex() % defn->physical_handles.size();
        return defn->physical_handles[ frame_index ].ref();
    }

    const kege::ShaderResrc* RenderGraph::getShaderResource( const RgResrcHandle& handle )const
    {
        const RgShaderResrcDefn* resrc = _asset_manager.get< RgShaderResrcDefn >( handle.index );
        uint32_t frame_index = _graphics->getCurrFrameIndex() % resrc->physical_handles.size();
        return resrc->physical_handles[ frame_index ].ref();
    }

    kege::RgResrcHandle RenderGraph::defnShaderResource( const RgShaderResrcDefn& defn )
    {
        int id = _asset_manager.add< RgShaderResrcDefn >( defn.name, defn );

        RgShaderResrcDefn* resrc = _asset_manager.get< RgShaderResrcDefn >( id );
        resrc->handle.type = RgResrcType::ShaderResource;
        resrc->handle.index = id;
        return resrc->handle;
    }

    void RenderGraph::removeShaderResource( const RgResrcHandle& handle )
    {
        _asset_manager.remove< RgShaderResrcDefn >( handle.index );
    }

    void RenderGraph::removeShaderResource( const std::string& name )
    {
        _asset_manager.remove< RgShaderResrcDefn >( name );
    }


    

    kege::RgResrcHandle RenderGraph::importBuffer( std::string name, const std::vector<kege::BufferHandle>& handles )
    {
        BufferDefn defn;
        defn.name = name;
        defn.physical_handle = handles;
        return defnBuffer( defn );
    }

    const kege::BufferHandle* RenderGraph::getBuffer( const RgResrcHandle& handle )const
    {
        const kege::BufferDefn* def = _asset_manager.get< kege::BufferDefn >( handle.index );
        uint32_t frame_index = _graphics->getCurrFrameIndex() % def->physical_handle.size();
        return &def->physical_handle[ frame_index ];
    }

    const kege::BufferHandle* RenderGraph::fetchBuffer( const std::string& name )const 
    {
        const kege::BufferDefn* def = _asset_manager.fetch< kege::BufferDefn >( name );
        uint32_t frame_index = _graphics->getCurrFrameIndex() % def->physical_handle.size();
        return &def->physical_handle[ frame_index ];
    }

    kege::RgResrcHandle RenderGraph::defnBuffer( const kege::BufferDefn& defn )
    {
        kege::BufferDefn* def = _asset_manager.fetch< kege::BufferDefn >( defn.name );
        if ( def == nullptr )
        {
            int index = _asset_manager.add< kege::BufferDefn >( defn.name, defn );
            def = _asset_manager.get< kege::BufferDefn >( index );
            def->handle = {kege::RgResrcType::Buffer, index};
            return def->handle;
        }
        else
        {
            //kegeLog["warning"]["render-graph.hpp"] <<"render graph resource -> " << defn.name <<" already exist. Warning from RenderGraph::defineBuffer()";
            return def->handle;
        }
    }

    void RenderGraph::removeBuffer( const RgResrcHandle& handle )
    {
        _asset_manager.remove< kege::BufferDefn >( handle.index );
    }

    void RenderGraph::removeBuffer( const std::string& name )
    {
        _asset_manager.remove< kege::BufferDefn >( name );
    }

    const kege::BufferDefn* RenderGraph::getBufferDefn( const std::string& name )
    {
        return _asset_manager.fetch< kege::BufferDefn >( name );
    }




    RgResrcHandle RenderGraph::importImage( std::string name, const std::vector< kege::ImageHandle >& handles )
    {
        ImageDefn defn;
        defn.name = name;
        defn.physical_handle = handles;
        return defnImage( defn );
    }

    const ImageDefn* RenderGraph::getImageDefn( const std::string& name )
    {
        return _asset_manager.fetch< kege::ImageDefn >( name );
    }

    kege::RgResrcHandle RenderGraph::defnImage( const kege::ImageDefn& defn )
    {
        kege::ImageDefn* def = _asset_manager.fetch< kege::ImageDefn >( defn.name );
        if ( def == nullptr )
        {
            int index = _asset_manager.add< kege::ImageDefn >( defn.name, defn );
            def = _asset_manager.get< kege::ImageDefn >( index );
            def->handle = {kege::RgResrcType::Buffer, index};
            return def->handle;
        }
        else
        {
            return def->handle;
        }
    }

    const kege::ImageHandle* RenderGraph::getImage( const kege::RgResrcHandle& handle )const
    {
        const kege::ImageDefn* def = _asset_manager.get< kege::ImageDefn >( handle.index );
        uint32_t frame_index = _graphics->getCurrFrameIndex() % def->physical_handle.size();
        return &def->physical_handle[ frame_index ];
    }

    const kege::ImageHandle* RenderGraph::fetchImage( const std::string& name )const
    {
        const kege::ImageDefn* def = _asset_manager.fetch< kege::ImageDefn >( name );
        uint32_t frame_index = _graphics->getCurrFrameIndex() % def->physical_handle.size();
        return &def->physical_handle[ frame_index ];
    }

    void RenderGraph::removeImage( const kege::RgResrcHandle& handle )
    {
        _asset_manager.remove< kege::ImageDefn >( handle.index );
    }
    
    void RenderGraph::removeImage( const std::string& name )
    {
        _asset_manager.remove< kege::ImageDefn >( name );
    }




    RgResrcHandle RenderGraph::importSampler( std::string name, const kege::SamplerHandle& handle )
    {
        SamplerDefn defn;
        defn.name = name;
        defn.physical_handle = handle;
        return defnSampler( defn );
    }

    RgResrcHandle RenderGraph::defnSampler( const kege::SamplerDefn& defn )
    {
        kege::SamplerDefn* def = _asset_manager.fetch< kege::SamplerDefn >( defn.name );
        if ( def == nullptr )
        {
            int index = _asset_manager.add< kege::SamplerDefn >( defn.name, defn );
            def = _asset_manager.get< kege::SamplerDefn >( index );
            def->handle = {kege::RgResrcType::Sampler, index};
            return def->handle;
        }
        else
        {
            return def->handle;
        }
    }

    const kege::SamplerHandle* RenderGraph::getSampler( const RgResrcHandle& handle )const
    {
        const kege::SamplerDefn* def = _asset_manager.get< kege::SamplerDefn >( handle.index );
        return &def->physical_handle;
    }

    const kege::SamplerHandle* RenderGraph::fetchSampler( const std::string& name )const
    {
        const kege::SamplerDefn* def = _asset_manager.fetch< kege::SamplerDefn >( name );
        return &def->physical_handle;
    }

    void RenderGraph::removeSampler( const kege::RgResrcHandle& handle )
    {
        _asset_manager.remove< kege::ImageDefn >( handle.index );
    }

    void RenderGraph::removeSampler( const std::string& name )
    {
        _asset_manager.remove< kege::ImageDefn >( name );
    }

    RgResrcHandle RenderGraph::getRgResrcShaderResrc( const std::string& name )
    {
        const kege::RgShaderResrcDefn* def = _asset_manager.fetch< kege::RgShaderResrcDefn >( name );
        return def->handle;
    }

    RgResrcHandle RenderGraph::getRgResrcSampler( const std::string& name )
    {
        const kege::SamplerDefn* def = _asset_manager.fetch< kege::SamplerDefn >( name );
        return def->handle;
    }

    RgResrcHandle RenderGraph::getRgResrcBuffer( const std::string& name )
    {
        const kege::BufferDefn* def = _asset_manager.fetch< kege::BufferDefn >( name );
        return def->handle;
    }

    RgResrcHandle RenderGraph::getRgResrcImage( const std::string& name )
    {
        const kege::ImageDefn* def = _asset_manager.fetch< kege::ImageDefn >( name );
        return def->handle;
    }



    void RenderGraph::addPass( const RenderPassDefn& definition)
    {
        int id = static_cast<int>( _render_passes.size() );
        _render_passes.push_back({});
        _render_passes[id]._defn = definition;
        _render_passes[id]._graph = this;
        _render_passes[id]._id = id;
    }

    kege::Graphics* RenderGraph::getGraphics()
    {
        return _graphics;
    }

    void RenderGraph::execute( RenderManager& render_manager )
    {
        if( _compiled_pass_execution_plan.empty() )
        {
            KEGE_LOG_ERROR << "calling RenderGraph::execute with out compiling." <<Log::nl;
            return;
        }
        
        std::vector< CommandBuffer* > submitables;
        for (int i = 0; i < _compiled_pass_execution_plan.size(); ++i)
        {
            RenderPass* pass = _compiled_pass_execution_plan[i];
            if( pass->execute( render_manager ) )
            {
                submitables.push_back( pass->getCommandBuffer() );
            }
        }

        _graphics->submitCommands( submitables );
    }

    ImageLayout determineLayoutFromAccess(AccessFlags access, bool is_image, bool is_write)
    {
        // ... same logic as before ...
        if (!is_image) return ImageLayout::Undefined;
        if (access == AccessFlags::ColorAttachmentWrite) return ImageLayout::ColorAttachment;
        // ... etc ...
        return ImageLayout::Undefined;
    }

    bool RenderGraph::compile()
    {
        // --- Step 1: Resource Writers ---
        std::unordered_map< std::string, int > resource_writers;
        for ( int i = 0; i < _render_passes.size(); ++i )
        {
            const RenderPass& pass = _render_passes[i];
            for ( const auto& write : pass.getWrites() )
            {
                resource_writers[ write.name ] = i; // Pass index `i` is the last writer so far
            }
        }

        // --- Step 2: Build Dependency Graph ---
        std::unordered_map< int, std::unordered_set< int > > pass_dependencies; // pass_index -> set of indices it depends on
        for (int i = 0; i < _render_passes.size(); ++i)
        {
            const RenderPass& pass = _render_passes[i];
            for ( const auto& read : pass.getReads() )
            {
                auto itr = resource_writers.find( read.name );
                if ( itr != resource_writers.end() && itr->second != i )
                {
                    // Pass `i` depends on the writer pass `writer_iter->second`
                    pass_dependencies[i].insert( itr->second );
                }
            }
        }

        // --- Step 3: Compute In-Degrees ---
        std::vector< int > in_degree( _render_passes.size(), 0 );
        for ( const auto& [ pass_index, dependencies ] : pass_dependencies )
        {
            in_degree[ pass_index ] = static_cast< int >( dependencies.size() );
        }


        // --- Step 4: Kahn’s Topological Sort ---
        std::queue< int > ready_queue;
        for (int i = 0; i < _render_passes.size(); ++i)
        {
            if (in_degree[i] == 0)
            {
                ready_queue.push(i);
            }
        }

        std::vector< int > sorted_pass_indices;
        while ( !ready_queue.empty() )
        {
            int current_pass_index = ready_queue.front();
            ready_queue.pop();
            sorted_pass_indices.push_back(current_pass_index);

            // Update in-degrees of dependent passes
            // Iterate through all passes to find those that depended on current_pass_index
            // (As noted before, using a forward adjacency list improves this part's efficiency)
            for (auto& [dependent_pass_index, dependencies] : pass_dependencies)
            {
                // If this pass depended on the one we just processed...
                if (dependencies.count(current_pass_index))
                {
                    // dependencies.erase(current_pass_index); // Erasing might not be needed if only decrementing in_degree
                    if (--in_degree[dependent_pass_index] == 0)
                    {
                        ready_queue.push(dependent_pass_index);
                    }
                }
            }
        }

        // --- Step 4b: Cycle Detection ---
        if ( sorted_pass_indices.size() != _render_passes.size() )
        {
            throw std::runtime_error("RenderGraph Error: Cycle detected in pass dependencies!");
        }

        // --- Step 5: Create Initial Sorted Pass Definition List ---
        std::vector< RenderPass* > sorted_pass_definitions;
        sorted_pass_definitions.reserve( sorted_pass_indices.size() );
        for (int pass_index : sorted_pass_indices)
        {
            sorted_pass_definitions.push_back( &_render_passes[ pass_index ] );
        }

        // --- Step 6: Resolve Resource Links ---
        if( !resolveResosurceLinks( sorted_pass_definitions ) )
        {
            return false;
        }
        // --- Step 7: Update Shader Resource ---
        //if( !updateShaderResources() )
        //{
        //    return false;
        //}

        // --- Step 8: Barrier Calculation & Final Plan Generation ---
        analyzeTransitions( sorted_pass_definitions );

        return true;
    }



    bool RenderGraph::resolveResosurceLinks( std::vector< RenderPass* >& sorted_pass_definitions )
    {
        for ( RenderPass* pass : sorted_pass_definitions)
        {
            resolveResosurceLinks( pass );
        }
        return true;
    }

    bool RenderGraph::resolveResosurceLinks( RenderPass* pass )
    {
        pass->_command_buffers.resize( MAX_FRAMES_IN_FLIGHT );

        for ( int i=0; i<pass->_command_buffers.size(); ++i )
        {
            if ( !pass->_command_buffers[ i ] )
            {
                pass->_command_buffers[ i ] = _graphics->createCommandBuffer( pass->_defn.type );
            }
        }

        for ( auto& write : pass->getWrites() )
        {
            if ( !write.handle )
            {
                if ( write.type == RgResrcType::Image )
                {
                    ImageDefn* def = _asset_manager.fetch< ImageDefn >( write.name );
                    if ( def == nullptr )
                    {
                        KEGE_LOG_ERROR <<"undefinded image resource - " << write.name <<Log::nl;
                        return false;
                    }
                    write.handle = def->handle;

                    if ( def->physical_handle.empty() )
                    {
                        createImage( *def );
                    }
                }
                else if ( write.type == RgResrcType::Buffer )
                {
                    BufferDefn* def = _asset_manager.fetch< BufferDefn >( write.name );
                    if ( def == nullptr )
                    {
                        KEGE_LOG_ERROR <<"undefinded buffer resource - " << write.name <<Log::nl;
                        return false;
                    }
                    write.handle = def->handle;

                    if ( def->physical_handle.empty() )
                    {
                        createBuffer( *def );
                    }
                }
            }
        }

        std::vector< RgShaderResrcDefn* > shader_Resource_defns;
        for ( auto& read : pass->getReads() )
        {
            if ( !read.handle )
            {
                switch ( read.type )
                {
                    case RgResrcType::Image:
                    {
                        ImageDefn* def = _asset_manager.fetch< ImageDefn >( read.name );
                        if ( def == nullptr )
                        {
                            kege::Log::error <<"undefinded image resource - " << read.name <<Log::nl;
                            return false;
                        }

                        read.handle = def->handle;

                        if ( def->physical_handle.empty() )
                        {
                            createImage( *def );
                        }
                        break;
                    }

                    case RgResrcType::Buffer:
                    {
                        BufferDefn* def = _asset_manager.fetch< BufferDefn >( read.name );
                        if ( def == nullptr )
                        {
                            kege::Log::error <<"undefinded buffer resource - " << read.name <<Log::nl;
                            return false;
                        }

                        read.handle = def->handle;
                        if ( def->physical_handle.empty() )
                        {
                            createBuffer( *def );
                        }
                        break;
                    }

                    case RgResrcType::Sampler:
                    {
                        SamplerDefn* def = _asset_manager.fetch< SamplerDefn >( read.name );
                        if ( def == nullptr )
                        {
                            kege::Log::error <<"undefinded sampler resource - " << read.name <<Log::nl;
                            return false;
                        }
                        read.handle = def->handle;
                        if ( !def->physical_handle )
                        {
                            createSampler( *def );
                        }
                        break;
                    }

                    case RgResrcType::ShaderResource:
                    {
                        RgShaderResrcDefn* def = _asset_manager.fetch< RgShaderResrcDefn >( read.name );
                        if ( def == nullptr )
                        {
                            kege::Log::error <<"undefinded shader resource - " << read.name <<Log::nl;
                            return false;
                        }
                        read.handle = def->handle;
                        pass->_shader_resources.push_back( read.handle );
                        shader_Resource_defns.push_back( def );
                        break;
                    }

                    default:
                    {
                        kege::Log::error <<"unsupported read resource -> " << read.name <<Log::nl;
                        break;
                    }
                }
            }
        }

        for ( RgShaderResrcDefn* def : shader_Resource_defns )
        {
            if ( def->physical_handles.empty() )
            {
                createShaderResource( def );
            }
        }
        return true;
    }

    void RenderGraph::createShaderResource( RgShaderResrcDefn* def )
    {
        def->physical_handles.resize( def->frames_in_flight );
        for (int frame=0; frame<def->frames_in_flight; ++frame)
        {
            UniformResourceSet resources;
            UniformDescriptorSet descriptors;

            descriptors.set = def->set_index;
            for ( auto& binding : def->bindings )
            {
                descriptors.descriptors.push_back
                ({
                    .descriptor_type = binding.type,
                    .binding = binding.binding,
                    .count = binding.count,
                    .name = binding.name,
                });

                switch ( binding.targets[0].type )
                {
                    case RgResrcType::Image:
                    {
                        ImageBindings images( binding.targets.size() );
                        for (int i=0; i<binding.targets.size(); ++i)
                        {
                            ImageDefn* image = _asset_manager.fetch< kege::ImageDefn >( binding.targets[i].name );
                            if ( image->physical_handle.empty() )
                            {
                                createImage( *image );
                            }

                            SamplerDefn* sampler = _asset_manager.fetch< kege::SamplerDefn >( binding.targets[i].sampler );
                            if ( !sampler->physical_handle )
                            {
                                createSampler( *sampler );
                            }

                            int index = image->physical_handle.size() % def->frames_in_flight;

                            images[i] = ImageInfo
                            {
                                .image = image->physical_handle[ index ],
                                .sampler = sampler->physical_handle,
                                .layout = ImageLayout::ShaderReadOnly
                            };
                        }
                        UniformResource ur{ .binding = binding.binding, .uniform = images };
                        resources.push_back(ur);
                        break;
                    }

                    case RgResrcType::Buffer:
                    {
                        BufferBindings buffers( binding.targets.size() );

                        for (int i=0; i<binding.targets.size(); ++i)
                        {
                            BufferDefn* buffer = _asset_manager.fetch< kege::BufferDefn >( binding.targets[i].name );
                            if ( buffer->physical_handle.empty() )
                            {
                                createBuffer( *buffer );
                            }

                            int index = buffer->physical_handle.size() % def->frames_in_flight;

                            buffers[i] = BufferInfo
                            {
                                .buffer = buffer->physical_handle[ index ],
                                .offset = 0,
                                .range = buffer->info.size
                            };
                        }
                        UniformResource ur{ .binding = binding.binding, .uniform = buffers };
                        resources.push_back(ur);
                        break;
                    }

                    default: break;
                }
            }

            def->physical_handles[ frame ] = new ShaderResrc
            ({
                .descriptors = { descriptors },
                .resources = { resources },
                .graphics = _graphics
            });
        }
    }

    void RenderGraph::createBuffer( BufferDefn& defn )
    {
        kege::BufferDesc desc = {};
        desc.name         = defn.name.data();
        desc.size         = defn.info.size;
        desc.data         = defn.info.data;
        desc.memory_usage = defn.info.memory_usage;
        desc.usage        = defn.info.usage;

        defn.physical_handle.resize( defn.frames_in_flight );
        for (int i = 0; i<defn.frames_in_flight; ++i )
        {
            defn.physical_handle[i] = _graphics->createBuffer( desc );
        }
    }

    void RenderGraph::createImage( ImageDefn& defn )
    {
        kege::ImageDesc desc = {};
        desc.width      = defn.info.width;
        desc.height     = defn.info.height;
        desc.depth      = defn.info.depth;
        desc.format     = defn.info.format;
        desc.type       = defn.info.type;
        desc.name = defn.name;
        desc.usage      = defn.usages;

        defn.physical_handle.resize( defn.frames_in_flight );
        for (int i = 0; i<defn.frames_in_flight; ++i )
        {
            defn.physical_handle[i] = _graphics->createImage( desc );
        }
    }

    void RenderGraph::createSampler( SamplerDefn& defn )
    {
        defn.physical_handle = _graphics->createSampler( defn.desc );
    }

    void RenderGraph::analyzeTransitions( std::vector< RenderPass* >& sorted_pass_definitions )
    {
        _compiled_pass_execution_plan.clear();
        _compiled_pass_execution_plan.reserve( sorted_pass_definitions.size() );

        std::unordered_map< kege::ImageHandle,  RgResrcUsage > current_image_states;
        std::unordered_map< kege::BufferHandle, RgResrcUsage > current_buffer_states;

        // Initialize states for imported resources
        ResourceManagerT< ImageDefn >* image_definitions = _asset_manager.get< ImageDefn >();
        for (int i=0; i<image_definitions->begin(); ++i)
        {
            ImageDefn& defn = image_definitions->get(i);
            // Assuming handle is valid if non-empty and not the 'needs creation' sentinel
            bool is_imported = !defn.physical_handle.empty() && (defn.physical_handle[0].id >= 0); // Adjust check as needed
            if (is_imported)
            {
                for (const auto& handle : defn.physical_handle)
                {
                    // Determine actual initial state from outside graph - assuming Undefined for now
                    current_image_states[ handle ] = RgResrcUsage{};
                }
            }
        }

        ResourceManagerT< BufferDefn >* buffer_definitions = _asset_manager.get< BufferDefn >();
        for (int i=0; i<buffer_definitions->begin(); ++i)
        {
            BufferDefn& defn = buffer_definitions->get(i);
            bool is_imported = !defn.physical_handle.empty() && (defn.physical_handle[0].id >= 0); // Adjust check as needed
            if ( is_imported )
            {
                for (const auto& handle : defn.physical_handle)
                {
                    current_buffer_states[ handle ] = RgResrcUsage{};
                }
            }
        }

        // Iterate through the topologically sorted passes
        for ( RenderPass* pass : sorted_pass_definitions )
        {
            BarrierDescription& pre_barriers = pass->_barriers;

            for (const auto& read : pass->_defn.reads)
            {
                processUsage
                (
                    read.name,
                    read.type,
                    read.access,
                    read.stage,
                    read.handle,
                    false,
                    pre_barriers,
                    current_image_states,
                    current_buffer_states
                );
            }
            for (const auto& write : pass->_defn.writes)
            {
                processUsage
                ( 
                    write.name,
                    write.type,
                    write.access,
                    write.stage,
                    write.handle,
                    true,
                    pre_barriers,
                    current_image_states,
                    current_buffer_states
                );
            }


            for (const auto& read : pass->_defn.reads)
            {
                updateStateAfterPass
                (
                    read.name,
                    read.type,
                    read.access,
                    read.stage,
                    read.handle,
                    false,
                    current_image_states,
                    current_buffer_states
                );
            }
            for (const auto& write : pass->_defn.writes)
            {
                updateStateAfterPass
                (
                    write.name,
                    write.type,
                    write.access,
                    write.stage,
                    write.handle,
                    true,
                    current_image_states,
                    current_buffer_states
                );
            }

            _compiled_pass_execution_plan.push_back( pass );
        }
    }

    void RenderGraph::processUsage
    (
        const std::string& name,
        RgResrcType type,
        AccessFlags access,
        PipelineStageFlag stage,
        RgResrcHandle handle,
        bool is_write, BarrierDescription& pre_barriers,
        std::unordered_map< kege::ImageHandle,  RgResrcUsage >& current_image_states,
        std::unordered_map< kege::BufferHandle, RgResrcUsage >& current_buffer_states
    )
    {
        if ( !handle )
        {
            kege::Log::error << "Unresolved resource link processing usage for: " << name <<Log::nl;
            return;
        }

        RgResrcUsage* current_state_ptr = nullptr;
        kege::ImageHandle  img_handle = {};
        kege::BufferHandle buf_handle = {};
        kege::Ref< ShaderResrc > sr_handle = {};
        bool is_image = (type == RgResrcType::Image);

        // Simplified: Use only the first handle for barrier calculation
        int frame_resource_index = _graphics->getCurrFrameIndex(); //0; // TODO: Needs current frame context!

        const void* definition_ptr = nullptr; // Generic pointer to definition
        const std::vector< kege::ImageHandle >*  image_handles_ptr  = nullptr;
        const std::vector< kege::BufferHandle >* buffer_handles_ptr = nullptr;
        const std::vector< Ref< kege::ShaderResrc > >* sr_handles_ptr = nullptr;

        switch ( type )
        {
            case RgResrcType::Image:
            {
                const ImageDefn* defn = _asset_manager.get< ImageDefn >( handle.index );
                definition_ptr = defn;
                image_handles_ptr = &defn->physical_handle;
                if (frame_resource_index < defn->physical_handle.size())
                {
                    img_handle = defn->physical_handle[ frame_resource_index ];
                    current_state_ptr = &current_image_states[ img_handle ];
                }
                break;
            }
            case RgResrcType::Buffer:
            {
                const BufferDefn* defn = _asset_manager.get< BufferDefn >( handle.index );
                definition_ptr = defn;
                buffer_handles_ptr = &defn->physical_handle;
                if (frame_resource_index < defn->physical_handle.size())
                {
                    buf_handle = defn->physical_handle[ frame_resource_index ];
                    current_state_ptr = &current_buffer_states[ buf_handle ];
                }
                break;
            }
            case RgResrcType::ShaderResource:
            {
                const RgShaderResrcDefn* defn = _asset_manager.get< RgShaderResrcDefn >( handle.index );
                definition_ptr = defn;
                sr_handles_ptr = &defn->physical_handles;
                if (frame_resource_index < defn->physical_handles.size())
                {
                    sr_handle = defn->physical_handles[ frame_resource_index ];
                    current_state_ptr = &current_buffer_states[ buf_handle ];
                }
                break;
            }

            default: break;
        }

        if ( !current_state_ptr )
        {
            kege::Log::error << "Could not get handle/state processing usage for: " << name <<kege::Log::nl;
            return;
        }

        RgResrcUsage& current_state = *current_state_ptr;
        ImageLayout target_layout = determineLayoutFromAccess( access, is_image, is_write );
        AccessFlags target_access = access;
        PipelineStageFlag target_stage = stage;

        bool barrier_needed = false;
        RgResrcBarrierInfo barrier_info = {};

        if ( is_image && current_state.layout != target_layout && target_layout != ImageLayout::Undefined )
        {
            barrier_needed = true;
            barrier_info.old_layout = current_state.layout;
            barrier_info.new_layout = target_layout;
        }

        bool is_last_write = isWriteAccess( current_state.access ); // Use global scope helper
        bool is_current_write = isWriteAccess( target_access );

        if
        (
            (is_last_write && target_access != AccessFlags::None) ||
            (is_current_write && current_state.access != AccessFlags::None)
        )
        {
             if (!( isReadAccess( current_state.access ) && isReadAccess( target_access ) ))
             {
                 // Avoid read->read barrier
                  barrier_needed = true;
             }
        }

        // TODO: Queue transfer check

        if ( barrier_needed )
        {
            barrier_info.image_handle    = img_handle;
            barrier_info.buffer_handle   = buf_handle;
            barrier_info.src_stage_mask  = current_state.stage;
            barrier_info.src_access_mask = current_state.access;
            barrier_info.dst_stage_mask  = target_stage;
            barrier_info.dst_access_mask = target_access;

            if (is_image && barrier_info.old_layout == ImageLayout::Undefined) barrier_info.old_layout = current_state.layout;
            if (is_image && barrier_info.new_layout == ImageLayout::Undefined) barrier_info.new_layout = target_layout;

            // TODO: Populate queue indices if transferring

            pre_barriers.resource_barriers.push_back(barrier_info);

            // Partially update state based on barrier forcing the change
            if (is_image) current_state.layout = barrier_info.new_layout;
            // if (transferring) current_state.queue_family_index = barrier_info.dst_queue_family_index;
        }
    }

    void RenderGraph::updateStateAfterPass
    (
        const std::string& name,
        RgResrcType type,
        AccessFlags access,
        PipelineStageFlag stage,
        RgResrcHandle handle, bool is_write,
        std::unordered_map< kege::ImageHandle,  RgResrcUsage >& current_image_states,
        std::unordered_map< kege::BufferHandle, RgResrcUsage >& current_buffer_states
    )
    {
        RgResrcUsage* current_state_ptr = nullptr;
        kege::ImageHandle img_handle = {};
        kege::BufferHandle buf_handle = {};
        bool is_image = (type == RgResrcType::Image);
        int frame_resource_index = 0; // TODO: Needs current frame context

        if (is_image)
        {
            //const ImageDefn& defn = _image_definitions[ handle ];
            const ImageDefn* defn = _asset_manager.get< ImageDefn >( handle.index );
            if (frame_resource_index < defn->physical_handle.size())
            {
                img_handle = defn->physical_handle[ frame_resource_index ];
                current_state_ptr = &current_image_states[img_handle];
            }
        }
        else
        {
            //const BufferDefn& defn = _buffer_definitions[ handle ];
            const BufferDefn* defn = _asset_manager.get< BufferDefn >( handle.index );
            if (frame_resource_index < defn->physical_handle.size())
            {
                buf_handle = defn->physical_handle[ frame_resource_index ];
                current_state_ptr = &current_buffer_states[ buf_handle ];
            }
        }
        if (!current_state_ptr) return;
        RgResrcUsage& current_state = *current_state_ptr;

        current_state.access = access;
        current_state.stage = stage;
        if (is_image) current_state.layout = determineLayoutFromAccess(access, true, is_write);
    };

    void RenderGraph::clear()
    {
        if ( _graphics )
        {
            ResourceManagerT< RgShaderResrcDefn >* resources = _asset_manager.get< RgShaderResrcDefn >();
            for (int i=0; i<resources->begin(); ++i)
            {
                std::vector< kege::Ref< ShaderResrc > >& handles = resources->get( i ).physical_handles;
                handles.clear();
            }

            ResourceManagerT< BufferDefn >* buffers = _asset_manager.get< BufferDefn >();
            for (int i=0; i<buffers->begin(); ++i)
            {
                std::vector< kege::BufferHandle >& handles = buffers->get( i ).physical_handle;
                for (int i=0; i<handles.size(); ++i)
                {
                    _graphics->destroyBuffer( handles[i] );
                }
            }

            ResourceManagerT< ImageDefn >* images = _asset_manager.get< ImageDefn >();
            for (int i=0; i<images->begin(); ++i)
            {
                std::vector< kege::ImageHandle >& handles = images->get( i ).physical_handle;
                for (int i=0; i<handles.size(); ++i)
                {
                    _graphics->destroyImage( handles[i] );
                }
            }

            ResourceManagerT< SamplerDefn >* samplers = _asset_manager.get< SamplerDefn >();
            for (int i=0; i<samplers->begin(); ++i)
            {
                _graphics->destroySampler( samplers->get( i ).physical_handle );
            }

            for ( auto& defn : _render_passes )
            {
                for (int i=0; i<defn._command_buffers.size(); ++i)
                {
                    _graphics->destroyCommandBuffer( defn._command_buffers[i] );
                }
                defn._barriers.resource_barriers.clear();
            }
        }
        _asset_manager.shutdown();
        _render_passes.clear();
    }

    RenderGraph::RenderGraph( kege::Graphics* graphics )
    :   _graphics( graphics )
    {
        _asset_manager.initalize();
    }
    
    RenderGraph::~RenderGraph()
    {
        clear();
    }
}
