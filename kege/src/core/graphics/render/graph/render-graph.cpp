//
//  render-graph.cpp
//  ecs
//
//  Created by Kenneth Esdaile on 4/25/25.
//

#include "render-stage.hpp"
#include "render-graph.hpp"
#include "render-graph-loader.hpp"

namespace kege{

    void RenderGraph::addInitialImageTransition( const RgImageLayoutTransition& transition )
    {
        _initial_image_transitions.push_back( transition );
    }

    kege::RgResrcHandle RenderGraph::importShaderResource( std::string name, const ref::ShaderSet& handle )
    {
        RgShaderResrcDefn defn;
        defn.name = name;
        defn.physical_handle = handle;
        return defnShaderResource( defn );
    }

    const kege::ShaderSet* RenderGraph::fetchShaderResource( const std::string& name )const
    {
        const RgShaderResrcDefn* defn = _asset_manager->fetch< RgShaderResrcDefn >( name );
        return defn->physical_handle.ref();
    }

    const kege::ShaderSet* RenderGraph::getShaderResource( const RgResrcHandle& handle )const
    {
        const RgShaderResrcDefn* resrc = _asset_manager->get< RgShaderResrcDefn >( handle.index );
        return resrc->physical_handle.ref();
    }

    kege::RgResrcHandle RenderGraph::defnShaderResource( const RgShaderResrcDefn& defn )
    {
        uint64_t id = _asset_manager->add< RgShaderResrcDefn >( defn.name, defn );

        RgShaderResrcDefn* resrc = _asset_manager->get< RgShaderResrcDefn >( id );
        resrc->handle.type = RgResrcType::ShaderResource;
        resrc->handle.index = id;
        return resrc->handle;
    }

    void RenderGraph::removeShaderResource( const RgResrcHandle& handle )
    {
        _asset_manager->erase< RgShaderResrcDefn >( handle.index );
    }

    void RenderGraph::removeShaderResource( const std::string& name )
    {
        _asset_manager->remove< RgShaderResrcDefn >( name );
    }


    

    kege::RgResrcHandle RenderGraph::importBuffer( std::string name, const std::vector<ref::Buffer>& handles )
    {
        BufferDefn defn;
        defn.name = name;
        defn.physical_handle = handles;
        return defnBuffer( defn );
    }

    ref::Buffer RenderGraph::getBuffer( const RgResrcHandle& handle )const
    {
        const kege::BufferDefn* def = _asset_manager->get< kege::BufferDefn >( handle.index );
        uint32_t frame_index = _graphics->getFrameIndex() % def->physical_handle.size();
        return def->physical_handle[ frame_index ];
    }

    ref::Buffer RenderGraph::fetchBuffer( const std::string& name )const
    {
        const kege::BufferDefn* def = _asset_manager->fetch< kege::BufferDefn >( name );
        uint32_t frame_index = _graphics->getFrameIndex() % def->physical_handle.size();
        return def->physical_handle[ frame_index ];
    }

    kege::RgResrcHandle RenderGraph::defnBuffer( const kege::BufferDefn& defn )
    {
        kege::BufferDefn* def = _asset_manager->fetch< kege::BufferDefn >( defn.name );
        if ( def == nullptr )
        {
            uint64_t index = _asset_manager->add< kege::BufferDefn >( defn.name, defn );
            def = _asset_manager->get< kege::BufferDefn >( index );
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
        _asset_manager->erase< kege::BufferDefn >( handle.index );
    }

    void RenderGraph::removeBuffer( const std::string& name )
    {
        _asset_manager->remove< kege::BufferDefn >( name );
    }

    const kege::BufferDefn* RenderGraph::getBufferDefn( const std::string& name )
    {
        return _asset_manager->fetch< kege::BufferDefn >( name );
    }

    RgResrcHandle RenderGraph::importImage( std::string name, const std::vector< ref::Image >& handles )
    {
        ImageDefn defn;
        defn.name = name;
        defn.physical_handle = handles;
        return defnImage( defn );
    }

    kege::RgResrcHandle RenderGraph::defnImage( const kege::ImageDefn& defn )
    {
        kege::ImageDefn* def = _asset_manager->fetch< kege::ImageDefn >( defn.name );
        if ( def == nullptr )
        {
            uint64_t index = _asset_manager->add< kege::ImageDefn >( defn.name, defn );
            def = _asset_manager->get< kege::ImageDefn >( index );
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
        return _asset_manager->get< kege::ImageDefn >( handle.index );
    }
    const ImageDefn* RenderGraph::getImageDefn( const std::string& name )
    {
        return _asset_manager->fetch< kege::ImageDefn >( name );
    }


    const ref::Image RenderGraph::getImage( const kege::RgResrcHandle& handle )const
    {
        const kege::ImageDefn* def = _asset_manager->get< kege::ImageDefn >( handle.index );
        uint32_t frame_index = 0;
        if ( def->use_swapchain_image_index )
        {
            frame_index = _graphics->getSwapchain()->getImageIndex() % def->physical_handle.size();
        }
        else
        {
            frame_index = _graphics->getFrameIndex() % def->physical_handle.size();
        }
        return def->physical_handle[ frame_index ];
    }

    const ref::Image RenderGraph::fetchImage( const std::string& name )const
    {
        const kege::ImageDefn* def = _asset_manager->fetch< kege::ImageDefn >( name );
        uint32_t frame_index = _graphics->getFrameIndex() % def->physical_handle.size();
        return def->physical_handle[ frame_index ];
    }

    void RenderGraph::removeImage( const kege::RgResrcHandle& handle )
    {
        _asset_manager->erase< kege::ImageDefn >( handle.index );
    }
    
    void RenderGraph::removeImage( const std::string& name )
    {
        _asset_manager->remove< kege::ImageDefn >( name );
    }



    RgResrcHandle RenderGraph::importSampler( std::string name, const ref::Sampler& handle )
    {
        SamplerDefn defn;
        defn.name = name;
        defn.physical_handle = handle;
        return defnSampler( defn );
    }

    RgResrcHandle RenderGraph::defnSampler( const kege::SamplerDefn& defn )
    {
        kege::SamplerDefn* def = _asset_manager->fetch< kege::SamplerDefn >( defn.name );
        if ( def == nullptr )
        {
            uint64_t index = _asset_manager->add< kege::SamplerDefn >( defn.name, defn );
            def = _asset_manager->get< kege::SamplerDefn >( index );
            def->handle = {kege::RgResrcType::Sampler, index};
            return def->handle;
        }
        else
        {
            return def->handle;
        }
    }

    const ref::Sampler RenderGraph::getSampler( const RgResrcHandle& handle )const
    {
        const kege::SamplerDefn* def = _asset_manager->get< kege::SamplerDefn >( handle.index );
        return def->physical_handle;
    }

    const ref::Sampler RenderGraph::fetchSampler( const std::string& name )const
    {
        const kege::SamplerDefn* def = _asset_manager->fetch< kege::SamplerDefn >( name );
        return def->physical_handle;
    }

    void RenderGraph::removeSampler( const kege::RgResrcHandle& handle )
    {
        _asset_manager->erase< kege::ImageDefn >( handle.index );
    }

    void RenderGraph::removeSampler( const std::string& name )
    {
        _asset_manager->remove< kege::ImageDefn >( name );
    }

    RgResrcHandle RenderGraph::getRgResrcShaderResrc( const std::string& name )
    {
        const kege::RgShaderResrcDefn* def = _asset_manager->fetch< kege::RgShaderResrcDefn >( name );
        return def->handle;
    }

    RgResrcHandle RenderGraph::getRgResrcSampler( const std::string& name )
    {
        const kege::SamplerDefn* def = _asset_manager->fetch< kege::SamplerDefn >( name );
        return def->handle;
    }

    RgResrcHandle RenderGraph::getRgResrcBuffer( const std::string& name )
    {
        const kege::BufferDefn* def = _asset_manager->fetch< kege::BufferDefn >( name );
        return def->handle;
    }

    RgResrcHandle RenderGraph::getRgResrcImage( const std::string& name )
    {
        const kege::ImageDefn* def = _asset_manager->fetch< kege::ImageDefn >( name );
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

    ref::RenderExecutor RenderGraph::getRenderExecutor()
    {
        return _executor;
    }

    void RenderGraph::execute(const ref::Semaphore& image_available_sem, const ref::Semaphore& render_complete_sem)
    {
        _executor->execute( this, image_available_sem, render_complete_sem );
    }

    bool RenderGraph::load( const std::string& filename )
    {
        return kege::RenderGraphLoader::load( *this, filename );
    }

    bool RenderGraph::compile()
    {
        RenderGraphCompiler compiler;
        return compiler.compile( this );
    }

    void RenderGraph::createShaderResource( RgShaderResrcDefn* def )
    {
//        def->physical_handles.resize( def->frames );
//        for (int frame=0; frame<def->frames; ++frame)
//        {
//            UniformResourceSet resources;
//            UniformDescriptorSet descriptors;
//
//            descriptors.set = def->set_index;
//            for ( auto& binding : def->bindings )
//            {
//                descriptors.descriptors.push_back
//                ({
//                    .descriptor_type = binding.type,
//                    .binding = binding.binding,
//                    .count = binding.count,
//                    .name = binding.name,
//                });
//
//                switch ( binding.targets[0].type )
//                {
//                    case RgResrcType::Image:
//                    {
//                        ImageBindings images( binding.targets.size() );
//                        for (int i=0; i<binding.targets.size(); ++i)
//                        {
//                            ImageDefn* image = _asset_manager->fetch< kege::ImageDefn >( binding.targets[i].name );
//                            if ( image->physical_handle.empty() )
//                            {
//                                createImage( *image );
//                            }
//
//                            SamplerDefn* sampler = _asset_manager->fetch< kege::SamplerDefn >( binding.targets[i].sampler );
//                            if ( !sampler->physical_handle )
//                            {
//                                createSampler( *sampler );
//                            }
//
//                            int index = image->physical_handle.size() % def->frames;
//
//                            images[i] = ImageInfo
//                            {
//                                .image = image->physical_handle[ index ],
//                                .sampler = sampler->physical_handle,
//                                .layout = ImageLayout::ShaderRead
//                            };
//                        }
//                        UniformResource ur{ .binding = binding.binding, .uniform = images };
//                        resources.push_back(ur);
//                        break;
//                    }
//
//                    case RgResrcType::Buffer:
//                    {
//                        BufferBindings buffers( binding.targets.size() );
//
//                        for (int i=0; i<binding.targets.size(); ++i)
//                        {
//                            BufferDefn* buffer = _asset_manager->fetch< kege::BufferDefn >( binding.targets[i].name );
//                            if ( buffer->physical_handle.empty() )
//                            {
//                                createBuffer( *buffer );
//                            }
//
//                            int index = buffer->physical_handle.size() % def->frames;
//
//                            buffers[i] = BufferInfo
//                            {
//                                .buffer = buffer->physical_handle[ index ],
//                                .offset = 0,
//                                .range = buffer->info.size
//                            };
//                        }
//                        UniformResource ur{ .binding = binding.binding, .uniform = buffers };
//                        resources.push_back(ur);
//                        break;
//                    }
//
//                    default: break;
//                }
//            }
//
//            def->physical_handles[ frame ] = new ShaderResrc
//            ({
//                .descriptors = { descriptors },
//                .resources = { resources },
//                .graphics = _graphics
//            });
//        }
    }

    void RenderGraph::createBuffer( BufferDefn& defn )
    {
        kege::BufferDesc desc = {};
        desc.name         = defn.name.data();
        desc.size         = defn.info.size;
        desc.data         = defn.info.data;
        desc.memory_usage = defn.info.memory_usage;
        desc.usage        = defn.info.usage;

        kege::Frames< ref::Buffer >* frame_buffers = _asset_manager->create< kege::Frames< ref::Buffer > >( defn.name );
        frame_buffers->frames = defn.info.frames;

        defn.physical_handle.resize( defn.frames );
        for (int i = 0; i<defn.frames; ++i )
        {
            defn.physical_handle[i] = _graphics->createBuffer( desc );
            frame_buffers->sources[i] = defn.physical_handle[i];

            _asset_manager->add< ref::Buffer >( defn.name, defn.physical_handle[i] );
        }
    }

    void RenderGraph::createImage( ImageDefn& defn )
    {
        kege::Image::Desc desc = {};
        desc.extent.width   = defn.info.width;
        desc.extent.height  = defn.info.height;
        desc.extent.depth   = defn.info.depth;
        desc.format         = defn.info.format;
        desc.type           = defn.info.type;
        desc.debug_name     = defn.name;
        desc.usage          = defn.usages;

        kege::Frames< ref::Image >* frame_images = _asset_manager->create< kege::Frames< ref::Image > >( defn.name );
        frame_images->frames = defn.frames;

        defn.physical_handle.resize( defn.frames );
        for (int i = 0; i<defn.frames; ++i )
        {
            defn.physical_handle[i] = _graphics->createImage( desc );
            frame_images->sources[i] = defn.physical_handle[i];
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
            AssetCacheTable< RgShaderResrcDefn >* resources = _asset_manager->getAssetCacheTable< RgShaderResrcDefn >();
            resources->clear();

            AssetCacheTable< BufferDefn >* buffers = _asset_manager->getAssetCacheTable< BufferDefn >();
            buffers->clear();

            AssetCacheTable< ImageDefn >* images = _asset_manager->getAssetCacheTable< ImageDefn >();
            images->clear();

            AssetCacheTable< SamplerDefn >* samplers = _asset_manager->getAssetCacheTable< SamplerDefn >();
            samplers->clear();

            for ( auto& pass : _passes )
            {
                pass.destroy();
            }

            _asset_manager = nullptr;
            _graphics = nullptr;
        }
        _passes.clear();
        _executor.clear();
    }

    RenderGraph::RenderGraph( kege::Graphics* graphics, kege::AssetManager* asset_manager )
    :   _graphics( graphics )
    ,   _asset_manager( asset_manager )
    ,   _executor( new RenderExecutor( graphics, MAX_FRAMES_IN_FLIGHT ) )
    {
    }
    
    RenderGraph::~RenderGraph()
    {
        clear();
    }
}
