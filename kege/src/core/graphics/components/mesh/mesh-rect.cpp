//
//  mesh-rect.cpp
//  graphics
//
//  Created by Kenneth Esdaile on 6/27/25.
//

#include "mesh-rect.hpp"
namespace kege{

    RectMesh::RectMesh( const kege::vec3& center, float width, float height )
    {
        vertices.resize(4);
        vertices[0].position = center + kege::vec3(  width,  height,  0 );
        vertices[1].position = center + kege::vec3( -width,  height,  0 );
        vertices[2].position = center + kege::vec3( -width, -height,  0 );
        vertices[3].position = center + kege::vec3(  width, -height,  0 );

        vertices[0].normal = kege::vec3( 0, 0, 1 );
        vertices[1].normal = kege::vec3( 0, 0, 1 );
        vertices[2].normal = kege::vec3( 0, 0, 1 );
        vertices[3].normal = kege::vec3( 0, 0, 1 );

        vertices[0].texcoord = kege::vec2( 1, 1 );
        vertices[1].texcoord = kege::vec2( 0, 1 );
        vertices[2].texcoord = kege::vec2( 0, 0 );
        vertices[3].texcoord = kege::vec2( 0, 1 );

        //this->min_bound = vertices[2].position;
        //this->max_bound = vertices[0].position;

        indices = std::vector< uint32_t >
        {
            0, 1, 2,
            2, 3, 0
        };
    }

}
