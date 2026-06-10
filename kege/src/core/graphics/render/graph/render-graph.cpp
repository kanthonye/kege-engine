//
//  render-graph.cpp
//  ecs
//
//  Created by Kenneth Esdaile on 4/25/25.
//

#include "render-graph.hpp"
#include "../renderer.hpp"

namespace kege{

    kege::RenderGraphDescriptor* RenderGraph::getRenderGraphDescriptor()
    {
        return &_description;
    }

    void RenderGraph::addPass( const RgRenderPassDesc& desc )
    {
        for (auto& read : desc.reads )
        {
            switch ( read.type )
            {
                case kege::RgResrcType::Buffer:
                    //_assets.getBufferDesc(<#const RgHandle &id#>)
                    break;

                default:
                    break;
            }
        }
        _description.passes.push_back( desc );
    }

    std::vector< kege::SubmitInfo > RenderGraph::execute
    (
        kege::Renderer* renderer,
        const kege::RenderGraphExecutionPlan& execution_plan
    )
    {
        return _executor.execute(renderer, execution_plan);
    }

    kege::RenderGraphExecutionPlan RenderGraph::compile
    (
        const kege::RenderGraphFrame& frame,
        const std::vector< RgHandle >& roots
    )
    {
        return _compiler.compile( frame, roots );
    }

    kege::RenderGraphFrame RenderGraph::build( const kege::RenderViewFrame& views )
    {
        kege::RenderGraphFrame render_graph_frame;
        render_graph_frame.render_pass_frame.resize( views.size() );
        render_graph_frame.render_view_frame = &views;
        
        for (int j,i=0; i < views.size(); ++i )
        {
            render_graph_frame.render_pass_frame[i].resize( views[i]->pass_types.size() + 1 );
            j = 0;
            for (const kege::RenderPassType& type : views[i]->pass_types )
            {
                render_graph_frame.render_pass_frame[i][j] = _description.getPassByType( type );
                j += 1;
            }
            render_graph_frame.render_pass_frame[i][j] = _description.getPassByType( kege::RenderPassType::Present );
        }
        return render_graph_frame;
    }

    bool RenderGraph::load( const std::string& filename )
    {
        if( !kege::RenderGraphLoader::load( _description, _renderer->getSwapchain(), filename ) )
        {
            return false;
        }
        integrateResources();
        return true;
    }

    void RenderGraph::integrateResources()
    {
        kege::AssetManager* asset_manager = _renderer->getAssetManager();
        kege::RenderGraphDescriptor* descriptor = getRenderGraphDescriptor();
        for (int i = 0; i < descriptor->passes.size(); ++i)
        {
            kege::RgRenderPassDesc& pass = descriptor->passes[i];
            for (int r = 0; r < pass.reads.size(); ++r)
            {
                kege::RgReadResrcDesc& read = pass.reads[r];

                read.resource.type = read.type;
                read.resource.id = asset_manager->getId< kege::ResrcLink >( read.name );

                switch ( read.type )
                {
                    case kege::RgResrcType::Buffer:
                    {
                        if ( read.resource.id == 0 )
                        {
                            kege::ResrcLink link;
                            link.type = read.type;
                            link.semantic = read.semantic;
                            link.bufr_desc = descriptor->findBuffer( read.name );
                            
                            read.resource.id = asset_manager->add< kege::ResrcLink >( read.name, link );
                            read.resource.type = kege::RgResrcType::Buffer;
                        }
                        break;
                    }
                    case kege::RgResrcType::Image:
                    {
                        if ( read.resource.id == 0 )
                        {
                            kege::ResrcLink link;
                            link.type = read.type;
                            link.semantic = read.semantic;
                            link.imag_desc = descriptor->findImage( read.name );

                            read.resource.id = asset_manager->add< kege::ResrcLink >( read.name, link );
                            read.resource.type = kege::RgResrcType::Image;
                        }
                        break;
                    }
                    case kege::RgResrcType::Sampler:
                    {
                        if ( read.resource.id == 0 )
                        {
                            kege::ResrcLink link;
                            link.type = read.type;
                            link.semantic = read.semantic;
                            link.samp_desc = descriptor->findSampler( read.name );
                            
                            read.resource.id = asset_manager->add< kege::ResrcLink >( read.name, link );
                            read.resource.type = kege::RgResrcType::Sampler;
                        }
                        break;
                    }

                    default: break;
                }
            }

            for (int r = 0; r < pass.writes.size(); ++r)
            {
                kege::RgWriteResrcDesc& write = pass.writes[r];

                write.resource.type = write.type;
                write.resource.id = asset_manager->getId< kege::ResrcLink >( write.name );

                switch ( write.type )
                {
                    case kege::RgResrcType::Buffer:
                    {
                        if ( write.resource.id == 0 )
                        {
                            kege::ResrcLink link;
                            link.type = write.type;
                            link.bufr_desc = descriptor->findBuffer( write.name );
                            write.resource.id = asset_manager->add< kege::ResrcLink >( write.name, link );
                            write.resource.type = kege::RgResrcType::Buffer;
                        }
                        break;
                    }
                    case kege::RgResrcType::Image:
                    {
                        if ( write.resource.id == 0 )
                        {
                            kege::ResrcLink link;
                            link.type = write.type;
                            link.imag_desc = descriptor->findImage( write.name );
                            write.resource.id = asset_manager->add< kege::ResrcLink >( write.name, link );
                            write.resource.type = kege::RgResrcType::Image;
                        }
                        break;
                    }
                    case kege::RgResrcType::Sampler:
                    {
                        if ( write.resource.id == 0 )
                        {
                            kege::ResrcLink link;
                            link.type = write.type;
                            link.samp_desc = descriptor->findSampler( write.name );
                            write.resource.id = asset_manager->add< kege::ResrcLink >( write.name, link );
                            write.resource.type = kege::RgResrcType::Sampler;
                        }
                        break;
                    }

                    default: break;
                }
            }
        }
    }

    void RenderGraph::clear()
    {
        _description.passes.clear();
    }

    kege::Renderer* RenderGraph::getRenderer()
    {
        return _renderer;
    }
    
    void RenderGraph::begin()
    {
        //_assets.beginFrame( _frame_index );
        _frame_index = (_frame_index >= 60) ? _frame_index = 0 : _frame_index + 1;
    }

    void RenderGraph::end()
    {
        //_assets.endFrame();
    }

    RenderGraph::RenderGraph( kege::Renderer* renderer )
    :   _frame_index(0)
    ,   _renderer( renderer )
    {
    }
    
    RenderGraph::~RenderGraph()
    {
        clear();
    }
}
