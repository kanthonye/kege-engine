//
//  render-graph.cpp
//  ecs
//
//  Created by Kenneth Esdaile on 4/25/25.
//

#include "render-stage.hpp"
#include "render-graph.hpp"

namespace kege{

    void RenderGraph::addInitialImageTransition( const RgImageLayoutTransition& transition )
    {
        _initial_image_transitions.push_back( transition );
    }

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
        uint32_t frame_index = _graphics->getFrameIndex() % defn->physical_handles.size();
        return defn->physical_handles[ frame_index ].ref();
    }

    const kege::ShaderResrc* RenderGraph::getShaderResource( const RgResrcHandle& handle )const
    {
        const RgShaderResrcDefn* resrc = _asset_manager.get< RgShaderResrcDefn >( handle.index );
        uint32_t frame_index = _graphics->getFrameIndex() % resrc->physical_handles.size();
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
        uint32_t frame_index = _graphics->getFrameIndex() % def->physical_handle.size();
        return &def->physical_handle[ frame_index ];
    }

    const kege::BufferHandle* RenderGraph::fetchBuffer( const std::string& name )const 
    {
        const kege::BufferDefn* def = _asset_manager.fetch< kege::BufferDefn >( name );
        uint32_t frame_index = _graphics->getFrameIndex() % def->physical_handle.size();
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

    kege::RgResrcHandle RenderGraph::defnImage( const kege::ImageDefn& defn )
    {
        kege::ImageDefn* def = _asset_manager.fetch< kege::ImageDefn >( defn.name );
        if ( def == nullptr )
        {
            int index = _asset_manager.add< kege::ImageDefn >( defn.name, defn );
            def = _asset_manager.get< kege::ImageDefn >( index );
            def->handle = {kege::RgResrcType::Image, index};
            return def->handle;
        }
        else
        {
            return def->handle;
        }
    }

    const ImageDefn* RenderGraph::getImageDefn( const kege::RgResrcHandle& handle )
    {
        return _asset_manager.get< kege::ImageDefn >( handle.index );
    }
    const ImageDefn* RenderGraph::getImageDefn( const std::string& name )
    {
        return _asset_manager.fetch< kege::ImageDefn >( name );
    }


    const kege::ImageHandle* RenderGraph::getImage( const kege::RgResrcHandle& handle )const
    {
        const kege::ImageDefn* def = _asset_manager.get< kege::ImageDefn >( handle.index );
        uint32_t frame_index = 0;
        if ( def->use_swapchain_image_index )
        {
            frame_index = _graphics->getSwapchain()->getImageIndex() % def->physical_handle.size();
        }
        else
        {
            frame_index = _graphics->getFrameIndex() % def->physical_handle.size();
        }
        return &def->physical_handle[ frame_index ];
    }

    const kege::ImageHandle* RenderGraph::fetchImage( const std::string& name )const
    {
        const kege::ImageDefn* def = _asset_manager.fetch< kege::ImageDefn >( name );
        uint32_t frame_index = _graphics->getFrameIndex() % def->physical_handle.size();
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
        int id = static_cast<int>( _passes.size() );
        _passes.push_back({});
        _passes[id]._defn = definition;
        _passes[id]._graph = this;
        _passes[id]._id = id;
    }

    kege::Graphics* RenderGraph::getGraphics()
    {
        return _graphics;
    }

    void RenderGraph::execute( RenderManager& render_manager )
    {
        if( _execution_order.empty() )
        {
            KEGE_LOG_ERROR << "calling RenderGraph::execute with out compiling." <<Log::nl;
            return;
        }

        if ( !_graphics->beginFrame() )
        {
            KEGE_LOG_ERROR << "Failed to begin Frame" <<Log::nl;
            return;
        }

        std::vector< kege::SubmitInfo > submits;
        for (int i = 0; i < _execution_order.size(); ++i)
        {
            RenderStage* pass = _execution_order[i];
            if( pass->execute( render_manager ) )
            {
                submits.push_back( pass->getSubmitInfo() );
            }
        }
        _graphics->submit( submits, _graphics->getSwapchain() );
        _graphics->endFrame();

        _graphics->present( _graphics->getSwapchain() );
    }

    bool RenderGraph::compile()
    {
        RenderGraphCompiler compiler;
        return compiler.compile( this );
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
                                .layout = ImageLayout::ShaderRead
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
        desc.name       = defn.name;
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

    void RenderGraph::clear()
    {
        if ( _graphics )
        {
            ResourceManagerT< RgShaderResrcDefn >* resources = _asset_manager.get< RgShaderResrcDefn >();
            for (int i=resources->begin(); 0<=i; i=resources->next(i))
            {
                std::vector< kege::Ref< ShaderResrc > >& handles = resources->get( i ).physical_handles;
                handles.clear();
            }

            ResourceManagerT< BufferDefn >* buffers = _asset_manager.get< BufferDefn >();
            for (int i=buffers->begin(); 0<=i; i=buffers->next(i))
            {
                std::vector< kege::BufferHandle >& handles = buffers->get( i ).physical_handle;
                for (int i=0; i<handles.size(); ++i)
                {
                    _graphics->destroyBuffer( handles[i] );
                }
            }

            ResourceManagerT< ImageDefn >* images = _asset_manager.get< ImageDefn >();
            for (int i=images->begin(); 0<=i; i=images->next(i))
            {
                std::vector< kege::ImageHandle >& handles = images->get( i ).physical_handle;
                for (int i=0; i<handles.size(); ++i)
                {
                    _graphics->destroyImage( handles[i] );
                }
            }

            ResourceManagerT< SamplerDefn >* samplers = _asset_manager.get< SamplerDefn >();
            for (int i=samplers->begin(); 0<=i; i=samplers->next(i))
            {
                _graphics->destroySampler( samplers->get( i ).physical_handle );
            }

            for ( auto& pass : _passes )
            {
                pass.destroy();
            }
        }
        _asset_manager.shutdown();
        _passes.clear();
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
