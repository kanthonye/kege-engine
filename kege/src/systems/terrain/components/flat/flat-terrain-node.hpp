//
//  flat-terrain-node.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 1/11/25.
//

#ifndef flat_terrain_node_hpp
#define flat_terrain_node_hpp

#include "ref.hpp"
#include "vectors.hpp"
#include "landscape-settings.h"
#include "flat-terrain-renderer.hpp"

namespace kege{

    class FlatTerrain;

    class TerrainNode : public RefCounter
    {
    public:

        virtual void render( FlatTerrainRenderer& renderer ) = 0;
        virtual void update( const dvec3& eye ) = 0;
        virtual void merge() = 0;
        virtual ~TerrainNode(){}
    };

    class FlatTerrainNode : public TerrainNode
    {
    public:

        void initialize( FlatTerrain* terrain, const dvec3& center, uint32_t diameter, uint32_t depth );
        bool splitable( const dvec3& eye )const;
        void update( const dvec3& eye );
        void split();
        void merge();

        void render( FlatTerrainRenderer& renderer );

        FlatTerrainNode();

        Ref< TerrainNode > children;
        FlatTerrain* terrain;

        dvec3 center;
        uint1 diameter;
        uint1 depth;
    };

    struct TerrainNodeChildren : public TerrainNode
    {
        void render( FlatTerrainRenderer& renderer );
        void update( const dvec3& eye );
        void merge();
        
        TerrainNodeChildren( FlatTerrain* terrain, const FlatTerrainNode* node );
        ~TerrainNodeChildren();

        FlatTerrainNode nodes[4];
    };

}

#endif /* flat_terrain_node_hpp */
