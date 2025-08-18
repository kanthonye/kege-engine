//
//  render-graph.cpp
//  ecs
//
//  Created by Kenneth Esdaile on 4/25/25.
//

#include "../graph/render-pass.hpp"
#include "../graph/render-graph.hpp"

namespace kege{

    kege::RgResrcHandle RenderGraph::defineShaderResource( const std::string& name, int frames_in_flight, const std::vector< kege::DescriptorSetLayoutBinding >& bindings )
    {
        auto i = _shader_resrc_map.find( name );
        if ( i != _shader_resrc_map.end() )
        {
            return i->second;
        }

        int index = static_cast<int>( _shader_resrc_definitions.size() );
        kege::RgResrcHandle resouce_handle = {RgResrcType::ShaderResource, index};
        _shader_resrc_map[ name ] = resouce_handle;
        _shader_resrc_definitions.push_back({});

        kege::RgShaderResourceDefn& definition = _shader_resrc_definitions[ index ];
        definition.resource_sets.resize( frames_in_flight );
        definition.resouce_handle = resouce_handle;

        for( int frame=0; frame < frames_in_flight; ++frame )
        {
            RgShaderResourceBindingSet& resource_binding_set = definition.resource_sets[ frame ];
            resource_binding_set.bindings.resize( bindings.size() );

            for(int i=0; i<bindings.size(); ++i)
            {
                kege::RgShaderResourceBinding& resource_binding = resource_binding_set.bindings[ i ];
                resource_binding.descriptor_type = bindings[i].descriptor_type;
                resource_binding.binding = bindings[i].binding;
                resource_binding.name = bindings[i].name;

                switch ( bindings[i].descriptor_type )
                {
                    case kege::DescriptorType::UniformBuffer:
                    case kege::DescriptorType::StorageBuffer:
                    case kege::DescriptorType::StorageBufferDynamic:
                    case kege::DescriptorType::UniformBufferDynamic:
                    {
                        resource_binding.resource.type = RgShaderResource::BUFFER;
                        resource_binding.resource.buffers.resize( bindings[i].count );
                        break;
                    }

                    case kege::DescriptorType::Sampler:
                    {
                        resource_binding.resource.type = RgShaderResource::SAMPLER;
                        resource_binding.resource.samplers.resize( bindings[i].count );
                        break;
                    }

                    case kege::DescriptorType::SampledImage:
                    case kege::DescriptorType::CombinedImageSampler:
                    {
                        resource_binding.resource.type = RgShaderResource::IMAGE;
                        resource_binding.resource.images.resize( bindings[i].count );
                        break;
                    }

                    case kege::DescriptorType::StorageTexelBuffer:
                    case kege::DescriptorType::UniformTexelBuffer:
                    {
                        resource_binding.resource.type = RgShaderResource::BUFFER_VIEW;
                        resource_binding.resource.views.resize( bindings[i].count );
                        break;
                    }

                    default:
                    {
                        KEGE_LOG_ERROR << "Invalid DescriptorType at DescriptorSetLayoutBinding -> " << i <<"\n";
                        return {};
                    }
                }
            }

            resource_binding_set.descriptor_sets = _graphics->allocateDescriptorSet( bindings );
        }
        return resouce_handle;
    }

    bool RenderGraph::updateShaderResource( kege::RgResrcHandle handle, const std::vector< RgShaderResource >& bindings )
    {
        if ( !handle )
        {
            KEGE_LOG_ERROR << "invalid RgResrcHandle handle in RenderGraph::updateShaderResource()";
            return false;
        }

        kege::RgShaderResourceDefn& definition = _shader_resrc_definitions[ handle.index ];

        for( int frame=0; frame<definition.resource_sets.size(); ++frame )
        {
            RgShaderResourceBindingSet& resource_binding_set = definition.resource_sets[ frame ];
            for(int i=0; i<bindings.size(); ++i)
            {
                if ( resource_binding_set.bindings[ i ].resource.type != bindings[ i ].type )
                {
                    KEGE_LOG_ERROR
                    << "Invalid Operation: attempting to map resource type -> "<< resource_binding_set.bindings[ i ].resource.type
                    <<" to incompatable resource type -> " << bindings[ i ].type <<Log::nl;
                    return false;
                }

                if ( resource_binding_set.bindings[ i ].resource.buffers.size() != bindings[ i ].buffers.size() )
                {
                    KEGE_LOG_ERROR <<"setting resource -> " << resource_binding_set.bindings[ i ].name << " of size "
                    << resource_binding_set.bindings[ i ].resource.buffers.size() <<" must be equal to binding resource of size "
                    << bindings[ i ].buffers.size() <<Log::nl;
                    return false;
                }

                switch ( resource_binding_set.bindings[ i ].resource.type )
                {
                    case RgShaderResource::BUFFER:
                    {
                        memcpy
                        (
                            resource_binding_set.bindings[ i ].resource.buffers.data(),
                            bindings[ i ].buffers.data(),
                            bindings[ i ].buffers.size() * sizeof( bindings[ i ].buffers[0] )
                        );
                        break;
                    }

                    case RgShaderResource::SAMPLER:
                    {
                        memcpy
                        (
                            resource_binding_set.bindings[ i ].resource.samplers.data(),
                            bindings[ i ].samplers.data(),
                            bindings[ i ].samplers.size() * sizeof( bindings[ i ].samplers[0] )
                        );
                        break;
                    }

                    case RgShaderResource::IMAGE:
                    {
                        memcpy
                        (
                            resource_binding_set.bindings[ i ].resource.images.data(),
                            bindings[ i ].images.data(),
                            bindings[ i ].images.size() * sizeof( bindings[ i ].images[0] )
                        );
                        break;
                    }

                    case RgShaderResource::BUFFER_VIEW:
                    {
                        memcpy
                        (
                            resource_binding_set.bindings[ i ].resource.views.data(),
                            bindings[ i ].views.data(),
                            bindings[ i ].views.size() * sizeof( bindings[ i ].views[0] )
                        );
                        break;
                    }

                    default:
                    {
                        KEGE_LOG_ERROR << "Invalid DescriptorType in RenderGraph::defineShaderResource" <<Log::nl;
                        return false;
                    }
                }
            }
        }
        return true;
    }

    bool RenderGraph::updateShaderResources()
    {
        // with the physical resource handles resovle, update descriptor sets
        for(int defn_index=0; defn_index<_shader_resrc_definitions.size(); ++defn_index)
        {
            kege::RgShaderResourceDefn& definition = _shader_resrc_definitions[defn_index];
            for ( int frame = 0; frame < definition.resource_sets.size(); ++frame )
            {
                std::vector< kege::WriteDescriptorSet > writes;
                for( int j = 0; j < definition.resource_sets[ frame ].bindings.size(); ++j )
                {
                    kege::RgShaderResourceBinding& bindings = definition.resource_sets[ frame ].bindings[ j ];

                    kege::WriteDescriptorSet write = {};
                    write.set = definition.resource_sets[ frame ].descriptor_sets;
                    write.descriptor_type = bindings.descriptor_type;
                    write.binding = bindings.binding;
                    write.array_element = 0;

                    switch ( write.descriptor_type )
                    {
                        case kege::DescriptorType::UniformBuffer:
                        case kege::DescriptorType::StorageBuffer:
                        case kege::DescriptorType::StorageBufferDynamic:
                        case kege::DescriptorType::UniformBufferDynamic:
                        {
                            for (int k=0; k<bindings.resource.buffers.size(); ++k)
                            {
                                const RgResrcHandle& resource = bindings.resource.buffers[ k ].buffer;
                                if ( _buffer_definitions[ resource.index ].physical_handle.empty() )
                                {
                                    createBuffer( _buffer_definitions[ resource.index ] );
                                }

                                const BufferDefn& buffer_defn = _buffer_definitions[ resource.index ];
                                const int index = frame % buffer_defn.frames_in_flight;
                                const BufferHandle& buffer = buffer_defn.physical_handle[ index ];

                                write.buffer_info.push_back
                                (
                                    BufferInfo
                                    {
                                        .buffer = buffer,
                                        .offset = bindings.resource.buffers[k].offset,
                                        .range = bindings.resource.buffers[k].range
                                    }
                                );
                            }
                            break;
                        }

                        case kege::DescriptorType::Sampler:
                        {
                            for (int k=0; k<bindings.resource.buffers.size(); ++k)
                            {
                                const RgResrcHandle resource = bindings.resource.samplers[ k ].sampler;
                                SamplerHandle sampler = _sampler_definitions[ resource.index ].physical_handle;
                                write.image_info.push_back
                                (
                                    ImageInfo
                                    {
                                        .image = {-1},
                                        .sampler = sampler,
                                        .layout = ImageLayout::ShaderReadOnly
                                    }
                                );
                            }
                            break;
                        }

                        case kege::DescriptorType::SampledImage:
                        {
                            for (int k=0; k<bindings.resource.samplers.size(); ++k)
                            {
                                const RgResrcHandle resource = bindings.resource.images[ k ].image;
                                if ( _image_definitions[ resource.index ].physical_handle.empty() )
                                {
                                    createImage( _image_definitions[ resource.index ] );
                                }

                                const ImageDefn& image_defn = _image_definitions[ resource.index ];
                                const int index = frame % image_defn.frames_in_flight;
                                const ImageHandle& image = image_defn.physical_handle[ index ];

                                write.image_info.push_back
                                (
                                    ImageInfo
                                    {
                                        .image = image,
                                        .sampler = {-1},
                                        .layout = ImageLayout::ShaderReadOnly
                                    }
                                );
                            }
                            break;
                        }

                        case kege::DescriptorType::CombinedImageSampler:
                        {
                            for (int k=0; k<bindings.resource.images.size(); ++k)
                            {
                                const RgResrcHandle image_resource = bindings.resource.images[ k ].image;
                                if ( _image_definitions[ image_resource.index ].physical_handle.empty() )
                                {
                                    createImage( _image_definitions[ image_resource.index ] );
                                }

                                const ImageDefn& image_defn = _image_definitions[ image_resource.index ];
                                const int index = frame % image_defn.frames_in_flight;
                                const ImageHandle& image = image_defn.physical_handle[ index ];

                                const RgResrcHandle& sampler_resource = bindings.resource.images[ k ].sampler;
                                const SamplerHandle& sampler = _sampler_definitions[ sampler_resource.index ].physical_handle;

                                write.image_info.push_back
                                (
                                    ImageInfo
                                    {
                                        .image = image,
                                        .sampler = sampler,
                                        .layout = ImageLayout::ShaderReadOnly
                                    }
                                );
                            }
                            break;
                        }

                        case kege::DescriptorType::StorageTexelBuffer:
                        case kege::DescriptorType::UniformTexelBuffer:
                        {
                            /*
                            for (int k=0; k<bindings.resource.views.size(); ++k)
                            {
                                const RgResrcHandle& resource = bindings.resource.views[ k ].view;
                                if ( _buffer_view_definitions[ resource.index ].physical_handle.empty() )
                                {
                                    createBufferView( _buffer_definitions[ resource.index ] );
                                }
                            }
                             */
                            break;
                        }

                        default:
                        {
                            KEGE_LOG_ERROR << "Invalid ShaderResrcUpdateInfo DescriptorType with image object" << Log::nl;
                            return false;
                        }
                    }
                    writes.push_back( write );
                }

                if ( !_graphics->updateDescriptorSets( writes ) )
                {
                    return false;
                }
            }
        }
        return true;
    }

    RgResrcHandle RenderGraph::importImage( std::string name, const std::vector< kege::ImageHandle >& handles )
    {
        ImageDefn defn;
        defn.name = name;
        defn.physical_handle = handles;
        return defineImage( defn );
    }

    RgResrcHandle RenderGraph::importBuffer( std::string name, const std::vector< kege::BufferHandle >& handles )
    {
        BufferDefn defn;
        defn.name = name;
        defn.physical_handle = handles;
        return defineBuffer( defn );
    }

    RgResrcHandle RenderGraph::importSampler( std::string name, const kege::SamplerHandle& handle )
    {
        int index = static_cast<int>( _image_definitions.size() );
        SamplerDefn defn;
        defn.physical_handle = handle;
        defn.handle = { RgResrcType::Sampler, index };
        _sampler_definitions.push_back( defn );
        _sampler_resource_map[ name ] = defn.handle;
        return defn.handle;
    }

    RgResrcHandle RenderGraph::defineImage( const ImageDefn& defn )
    {
        auto i = _image_resource_map.find( defn.name );
        if ( i == _image_resource_map.end() )
        {
            int index = static_cast<int>( _image_definitions.size() );
            kege::RgResrcHandle handle = {kege::RgResrcType::Image, index};
            _image_resource_map[ defn.name ] = handle;
            _image_definitions.push_back( defn );
            _image_definitions[ index ].handle = handle;
            return handle;
        }
        else
        {
            //kegeLog["warning"]["render-graph.hpp"] <<"render graph resource -> " << defn.name <<" already exist. Warning from RenderGraph::defineImage()";
            return i->second;
        }
    }

    RgResrcHandle RenderGraph::defineBuffer(const BufferDefn& defn )
    {
        auto i = _buffer_resource_map.find( defn.name );
        if ( i == _buffer_resource_map.end() )
        {
            int index = static_cast<int>( _buffer_definitions.size() );
            kege::RgResrcHandle handle = {kege::RgResrcType::Buffer, index};
            _buffer_resource_map[ defn.name ] = handle;
            _buffer_definitions.push_back( defn );
            _buffer_definitions[ index ].handle = handle;
            return handle;
        }
        else
        {
            //kegeLog["warning"]["render-graph.hpp"] <<"render graph resource -> " << defn.name <<" already exist. Warning from RenderGraph::defineBuffer()";
            return i->second;
        }
    }


    RgResrcHandle RenderGraph::defineSampler( std::string name, const kege::SamplerDesc& desc )
    {
        auto i = _sampler_resource_map.find( name );
        if ( i == _sampler_resource_map.end() )
        {
            int index = static_cast<int>( _sampler_definitions.size() );
            _sampler_definitions.push_back({});
            
            kege::RgResrcHandle handle = {kege::RgResrcType::Sampler, index};
            _sampler_definitions[ index ].physical_handle = _graphics->createSampler( desc );
            _sampler_definitions[ index ].handle = handle;
            _sampler_resource_map[ name ] = handle;
            return handle;
        }
        else
        {
            //KEGE_LOG_ERROR <<"render graph resource -> " << name <<" already exist. Warning from RenderGraph::defineSampler()";
            return i->second;
        }
    }

    const std::vector< kege::BufferHandle >& RenderGraph::getPhysicalBuffers( const RgResrcHandle& handle )const
    {
        return _buffer_definitions[ handle.index ].physical_handle;
    }

    const std::vector< kege::ImageHandle >& RenderGraph::getPhysicalImages( const RgResrcHandle& handle )const
    {
        return _image_definitions[ handle.index ].physical_handle;
    }

    const kege::SamplerHandle& RenderGraph::getPhysicalSampler( const RgResrcHandle& handle )const
    {
        return _sampler_definitions[ handle.index ].physical_handle;
    }

    const std::vector< kege::BufferHandle >* RenderGraph::getPhysicalBuffers( const std::string& name )
    {
        auto i = _buffer_resource_map.find( name );
        return ( i != _buffer_resource_map.end() ) ? &_buffer_definitions[ i->second ].physical_handle : nullptr;
    }
    
    const std::vector< kege::ImageHandle >* RenderGraph::getPhysicalImages( const std::string& name )
    {
        auto i = _image_resource_map.find( name );
        return ( i != _image_resource_map.end() ) ? &_image_definitions[ i->second ].physical_handle : nullptr;
    }

    const kege::SamplerHandle* RenderGraph::getPhysicalSampler( const std::string& name )
    {
        auto i = _sampler_resource_map.find( name );
        return ( i != _sampler_resource_map.end() ) ? &_sampler_definitions[ i->second ].physical_handle : nullptr;
    }

    kege::DescriptorSetHandle RenderGraph::getPhysicalDescriptorSet( const RgResrcHandle& handle )
    {
        uint32_t frame_index = _graphics->getCurrFrameIndex() % _shader_resrc_definitions[ handle.index ].resource_sets.size();
        return _shader_resrc_definitions[ handle.index ].resource_sets[ frame_index ].descriptor_sets;
    }

    kege::DescriptorSetHandle RenderGraph::getPhysicalDescriptorSet( const std::string& name )
    {
        auto i = _shader_resrc_map.find( name );
        if ( i == _shader_resrc_map.end() ) return {};
        return getPhysicalDescriptorSet( i->second );
    }

    kege::BufferHandle RenderGraph::getPhysicalBuffer( const RgResrcHandle& handle )
    {
        uint32_t frame_index = (uint32_t)_buffer_definitions[ handle.index ].physical_handle.size();
        frame_index = _graphics->getCurrFrameIndex() % frame_index;
        return _buffer_definitions[ handle.index ].physical_handle[ frame_index ];
    }

    kege::ImageHandle RenderGraph::getPhysicalImage( const RgResrcHandle& handle )
    {
        uint32_t frame_index = (uint32_t)_image_definitions[ handle.index ].physical_handle.size();
        frame_index = _graphics->getSwapchainImageIndex() % frame_index;
        return _image_definitions[ handle.index ].physical_handle[ frame_index ];
    }

    void RenderGraph::addGraphicsPass( const RenderPassDefn& definition)
    {
        int id = static_cast<int>( _render_passes.size() );
        _render_passes.push_back({});
        _render_passes[id]._type = QueueType::Graphics;
        _render_passes[id]._defn = definition;
        _render_passes[id]._graph = this;
        _render_passes[id]._id = id;
    }

    void RenderGraph::addComputePass( const RenderPassDefn& definition)
    {
        int id = static_cast<int>( _render_passes.size() );
        _render_passes.push_back({});
        _render_passes[id]._type = QueueType::Compute;
        _render_passes[id]._defn = definition;
        _render_passes[id]._graph = this;
        _render_passes[id]._id = id;
    }

    RgResrcHandle RenderGraph::getSamplerRgResrc( const std::string& name )
    {
        auto i = _sampler_resource_map.find( name );
        return ( i != _sampler_resource_map.end() ) ? i->second : RgResrcHandle{};
    }

    RgResrcHandle RenderGraph::getBufferRgResrc( const std::string& name )
    {
        auto i = _buffer_resource_map.find( name );
        return ( i != _buffer_resource_map.end() ) ? i->second : RgResrcHandle{};
    }

    RgResrcHandle RenderGraph::getImageRgResrc( const std::string& name )
    {
        auto i = _image_resource_map.find( name );
        return ( i != _image_resource_map.end() ) ? i->second : RgResrcHandle{};
    }

    void RenderGraph::add( RenderPassSetupContext context )
    {
        _render_pass_setup_context.push_back( context );
    }

    kege::Graphics* RenderGraph::getGraphics()
    {
        return _graphics;
    }

    void RenderGraph::execute()
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
            if( pass->execute() )
            {
                submitables.push_back( pass->_context._command_buffer );
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
        for ( RenderPassSetupContext& context : _render_pass_setup_context )
        {
            if ( context.add_resources )
            {
                context.add_resources( this );
            }
        }
        for ( RenderPassSetupContext& context : _render_pass_setup_context )
        {
            context.add_passes( this );
        }

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
        if( !updateShaderResources() )
        {
            return false;
        }

        // --- Step 8: Barrier Calculation & Final Plan Generation ---
        analyzeTransitions( sorted_pass_definitions );

        setupRenderPassContext( sorted_pass_definitions );
        return true;
    }

    void RenderGraph::setupRenderPassContext( std::vector< RenderPass* >& sorted_pass_definitions )
    {
        RenderPass* pass = nullptr;
        RenderPassContext* context = nullptr;
        for ( int i = 0; i < sorted_pass_definitions.size(); ++i )
        {
            pass = sorted_pass_definitions[i];
            context = &pass->getContext();

            context->_image_defn_map.clear();
            context->_buffer_defn_map.clear();
            context->_pass = pass;
            for ( const RgReadResrcDesc& resrc : pass->getReads() )
            {
                switch ( resrc.type )
                {
                    case RgResrcType::Buffer: context->_buffer_defn_map[ resrc.name ] = getBufferRgResrc( resrc.name ); break;
                    case RgResrcType::Image:  context->_image_defn_map[ resrc.name ]  = getImageRgResrc( resrc.name );  break;
                    default: break;
                }
            }
        }
    }

    bool RenderGraph::resolveResosurceLinks( std::vector< RenderPass* >& sorted_pass_definitions )
    {
        for ( RenderPass* pass : sorted_pass_definitions)
        {
            RenderPassContext* context = &_render_passes[ pass->_id ].getContext();
            context->_graphics = _graphics;
            context->_pass = pass;
            context->_name = pass->_defn.name;

            for ( auto& write : pass->getWrites() )
            {
                if ( !write.handle )
                {
                    if ( write.type == RgResrcType::Image )
                    {
                        auto i = _image_resource_map.find( write.name );
                        if ( i == _image_resource_map.end() )
                        {
                            KEGE_LOG_ERROR <<"undefinded image resource - " << write.name <<" in RenderGraph::resolveResosurceLinks" <<Log::nl;
                            return false;
                        }
                        write.handle = i->second;

                        if ( _image_definitions[ write.handle.index ].physical_handle.empty() )
                        {
                            createImage( _image_definitions[ i->second ] );
                        }
                    }
                    else if ( write.type == RgResrcType::Buffer )
                    {
                        auto i = _buffer_resource_map.find( write.name );
                        if ( i == _buffer_resource_map.end() )
                        {
                            KEGE_LOG_ERROR <<"undefinded buffer resource - " << write.name <<" in RenderGraph::resolveResosurceLinks" <<Log::nl;
                            return false;
                        }
                        write.handle = i->second;

                        if ( _buffer_definitions[ write.handle.index ].physical_handle.empty() )
                        {
                            createBuffer( _buffer_definitions[ write.handle.index ] );
                        }
                    }
                }
                else
                {
//                    if ( write.type == RgResrcType::Image )
//                    {
//                        if ( _image_definitions[ write.handle.index ].physical_handle.empty() )
//                        {
//                            createImage( _image_definitions[ write.handle.index ] );
//                        }
//                    }
//                    else if ( write.type == RgResrcType::Buffer )
//                    {
//                        if ( _buffer_definitions[ write.handle.index ].physical_handle.empty() )
//                        {
//                            createBuffer( _buffer_definitions[ write.handle.index ] );
//                        }
//                    }
                }
            }

            for ( auto& read : pass->getReads() )
            {
                if ( !read.handle )
                {
                    if ( read.type == RgResrcType::Image )
                    {
                        auto i = _image_resource_map.find( read.name );
                        if ( i != _image_resource_map.end() )
                        {
                            read.handle = _image_definitions[ i->second ].handle;
                        }
                        else
                        {
                            read.handle = i->second;
                        }

                        if ( _image_definitions[ read.handle.index ].physical_handle.empty() )
                        {
                            createImage( _image_definitions[ read.handle.index ] );
                        }

                        context->_image_defn_map[ read.name ] = read.handle;
                    }
                    else if ( read.type == RgResrcType::Buffer )
                    {
                        auto i = _buffer_resource_map.find( read.name );
                        if ( i != _buffer_resource_map.end() )
                        {
                            read.handle = _buffer_definitions[ i->second ].handle;
                        }
                        else
                        {
                            read.handle = i->second;
                        }

                        if ( _buffer_definitions[ read.handle.index ].physical_handle.empty() )
                        {
                            createBuffer( _buffer_definitions[ read.handle.index ] );
                        }

                        context->_buffer_defn_map[ read.name ] = read.handle;
                    }
                    else if ( read.type == RgResrcType::Sampler )
                    {
                        auto i = _sampler_resource_map.find( read.name );
                        if ( i != _sampler_resource_map.end() )
                        {
                            read.handle = _sampler_definitions[ i->second ].handle;
                        }
                        else
                        {
                            read.handle = i->second;
                        }
                        //context->_sampler_map[ read.name ] = read.handle;
                    }
                }
                else
                {
//                    if ( read.type == RgResrcType::Image )
//                    {
//                        if ( _image_definitions[ read.handle.index ].physical_handle.empty() )
//                        {
//                            createImage( _image_definitions[ read.handle.index ] );
//                        }
//                        context->_image_defn_map[ read.name ] = read.handle;
//                    }
//                    else if ( read.type == RgResrcType::Buffer )
//                    {
//                        if ( _buffer_definitions[ read.handle.index ].physical_handle.empty() )
//                        {
//                            createBuffer( _buffer_definitions[ read.handle.index ] );
//                        }
//                        context->_buffer_defn_map[ read.name ] = read.handle;
//                    }
                }
            }

            pass->_command_buffers.resize( MAX_FRAMES_IN_FLIGHT );
            for ( int i=0; i<pass->_command_buffers.size(); ++i )
            {
                if ( !pass->_command_buffers[ i ] )
                {
                    pass->_command_buffers[ i ] = _graphics->createCommandBuffer( pass->_type );
                }
            }
        }
        return true;
    }

    void RenderGraph::createBuffer( BufferDefn& defn )
    {
        kege::BufferDesc desc = {};
        desc.debug_name   = defn.name.data();
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
        desc.debug_name = defn.name;
        desc.usage      = defn.usages;

        defn.physical_handle.resize( defn.frames_in_flight );
        for (int i = 0; i<defn.frames_in_flight; ++i )
        {
            defn.physical_handle[i] = _graphics->createImage( desc );
        }
    }

    void RenderGraph::analyzeTransitions( std::vector< RenderPass* >& sorted_pass_definitions )
    {
        _compiled_pass_execution_plan.clear();
        _compiled_pass_execution_plan.reserve( sorted_pass_definitions.size() );

        std::unordered_map< kege::ImageHandle,  RgResrcUsage > current_image_states;
        std::unordered_map< kege::BufferHandle, RgResrcUsage > current_buffer_states;

        // Initialize states for imported resources
        for (const auto& defn : _image_definitions)
        {
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

        for (const auto& defn : _buffer_definitions)
        {
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
            KEGE_LOG_ERROR << "Unresolved resource link processing usage for: " << name <<Log::nl;
            return;
        }

        RgResrcUsage* current_state_ptr = nullptr;
        kege::ImageHandle  img_handle = {};
        kege::BufferHandle buf_handle = {};
        bool is_image = (type == RgResrcType::Image);

        // Simplified: Use only the first handle for barrier calculation
        int frame_resource_index = _graphics->getCurrFrameIndex(); //0; // TODO: Needs current frame context!

        const void* definition_ptr = nullptr; // Generic pointer to definition
        const std::vector< kege::ImageHandle >*  image_handles_ptr  = nullptr;
        const std::vector< kege::BufferHandle >* buffer_handles_ptr = nullptr;

        if ( is_image )
        {
            const ImageDefn& defn = _image_definitions[ handle.index ];
            definition_ptr = &defn;
            image_handles_ptr = &defn.physical_handle;
            if (frame_resource_index < defn.physical_handle.size())
            {
                img_handle = defn.physical_handle[ frame_resource_index ];
                current_state_ptr = &current_image_states[ img_handle ];
            }
        }
        else
        {
            const BufferDefn& defn = _buffer_definitions[ handle.index ];
            definition_ptr = &defn;
            buffer_handles_ptr = &defn.physical_handle;
            if (frame_resource_index < defn.physical_handle.size())
            {
                buf_handle = defn.physical_handle[ frame_resource_index ];
                current_state_ptr = &current_buffer_states[ buf_handle ];
            }
        }

        if ( !current_state_ptr )
        {
            KEGE_LOG_ERROR << "Could not get handle/state processing usage for: " << name <<Log::nl;
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
            const ImageDefn& defn = _image_definitions[ handle ];
            if (frame_resource_index < defn.physical_handle.size())
            {
                img_handle = defn.physical_handle[ frame_resource_index ];
                current_state_ptr = &current_image_states[img_handle];
            }
        }
        else
        {
            const BufferDefn& defn = _buffer_definitions[ handle ];
            if (frame_resource_index < defn.physical_handle.size())
            {
                buf_handle = defn.physical_handle[ frame_resource_index ];
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
            for ( auto& defn : _buffer_definitions )
            {
                for (int i=0; i<defn.physical_handle.size(); ++i)
                {
                    _graphics->destroyBuffer( defn.physical_handle[i] );
                }
            }

            for ( auto& defn : _image_definitions )
            {
                for (int i=0; i<defn.physical_handle.size(); ++i)
                {
                    _graphics->destroyImage( defn.physical_handle[i] );
                }
            }

            for ( auto& defn : _sampler_definitions )
            {
                _graphics->destroySampler( defn.physical_handle );
            }

            for ( auto& defn : _shader_resrc_definitions )
            {
                for (int i=0; i<defn.resource_sets.size(); ++i)
                {
                    _graphics->freeDescriptorSet( defn.resource_sets[i].descriptor_sets );
                }
            }

            for ( auto& defn : _render_passes )
            {
                for (int i=0; i<defn._command_buffers.size(); ++i)
                {
                    _graphics->destroyCommandBuffer( defn._command_buffers[i] );
                }
                defn._barriers.resource_barriers.clear();
                defn._context._buffer_defn_map.clear();
                defn._context._image_defn_map.clear();
            }
        }
        _buffer_definitions.clear();
        _image_definitions.clear();
        _render_passes.clear();
    }

    RenderGraph::RenderGraph( kege::Graphics* graphics )
    :   _graphics( graphics )
    {}
    
    RenderGraph::~RenderGraph()
    {
        clear();
    }
}
