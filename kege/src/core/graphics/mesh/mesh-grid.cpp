//
//  mesh-grid.cpp
//  kege
//
//  Created by Kenneth Esdaile on 10/26/24.
//

#include "mesh-grid.hpp"
namespace kege{

    GridMesh::GridMesh( int width, float height, int cols, int rows )
    {
        int vsize = (cols + 1) * (rows + 1);
        int isize = cols * rows;

        //float uvw = 1.00f / gsize;
        float sw = 1.00f / cols;
        float sh = 1.00f / rows;

        vertices.resize( vsize );

        float tx,ty, vy;
        int count = 0;
        for ( uint32_t z=0; z<=rows; z++ )
        {
            ty = sh * float(z);
            vy = (ty - 0.5f) * height;
            for ( uint32_t x=0; x<=cols; x++ )
            {
                tx = sw * float(x);
                vertices[ count ].texcoord = vec2(tx, ty);
                vertices[ count ].position = vec3((tx - 0.5f) * width, 0.0f, vy);
                vertices[ count ].normal = vec3(0.0f, 1.0f, 0.0f);
                count++;
            }
        }

        indices.resize( isize * 6 );
        count = 0;

        for (int z = 0; z < rows; z++)
        {
            float row = (rows + 1) * z;
            for (int x = 0; x < cols; x++)
            {
                int a = x + row;
                int b = a + 1;
                int c = a + cols + 1;
                int d = c + 1;
                //printf("%i %i %i %i\n",a,b,c,d);
                indices[ count++ ] = a;
                indices[ count++ ] = c;
                indices[ count++ ] = d;

                indices[ count++ ] = d;
                indices[ count++ ] = b;
                indices[ count++ ] = a;
            }
        }
    }

}
