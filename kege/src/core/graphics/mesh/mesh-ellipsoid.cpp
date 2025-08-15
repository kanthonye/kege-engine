//
//  mesh-ellipsoid.cpp
//  kege
//
//  Created by Kenneth Esdaile on 10/26/24.
//

#include "mesh-ellipsoid.hpp"

namespace kege{

    EllipsoidMesh::EllipsoidMesh( float x_radius, float y_radius, int cols, int rows )
    {
        vec2 angle;
        float ux, uy, cx,cy,sx,sy;
        uint32_t index,a,b,c,d,x,y;

        // make sure the length of the x, y, radius is not zero.
        x_radius = ((x_radius<0.0f) ? 1.0f: x_radius);
        y_radius = ((y_radius<0.0f) ? 1.0f: y_radius);
        // calculate the size of the vertices
        cols = (cols<3) ?3 :cols;
        rows = (rows<2) ?2 :rows;

        const float radianX = (6.28318530718 / (float)cols); // 360/col
        const float radianY = (3.14159265359 / (float)rows); // 180/row
        const float half_pi  =  1.57079632679;
        const int total_vertices = (rows + 1) * (cols + 1);

        this->vertices.resize( total_vertices );

        int index_count[2];
        index_count[0] = 2 * cols; // top and botton triangle faces
        index_count[1] = 2 * cols * (rows - 2); // middle = total triangle faces minus the top and bottom triangle faces
        int total_indices = 3 * (index_count[0] + index_count[1]);
        this->indices.resize( total_indices );

        index = 0;
        this->vertices[ index ].position = vec3( 0.f, y_radius, 0.f );
        this->vertices[ index ].texcoord = vec2( 0.f, 0.f );
        this->vertices[ index ].normal   = vec3( 0.f, 1.f, 0.f );
        index++;
        
        y = 1;
        angle.y = half_pi + (radianY * y);
        cy = cos(angle.y);
        sy = sin(angle.y);
        cy = zeroSmallValue( cy );
        sy = zeroSmallValue( sy );
        for (x=0; x<cols; ++x, ++index)
        {
            angle.x = radianX * x;
            cx = cos(angle.x);
            sx = sin(angle.x);
            cx = zeroSmallValue( cx );
            sx = zeroSmallValue( sx );

            ux = sx * cy;
            uy = cx * cy;
            this->vertices[ index ].position = vec3( ux * x_radius, sy * y_radius, uy * x_radius );
            this->vertices[ index ].texcoord = vec2( float(x) / float(cols), 0.f);//( angle.x, angle.y );
            this->vertices[ index ].normal   = vec3( ux, sy, uy );
            //std::cout <<std::setw(2) <<index <<": "<< vertices[ index ].position <<"\n";
        }

        //std::cout<<"\n";

        for (y=2; y<rows; y++)
        {
            angle.y = half_pi + (radianY * y);
            cy = cos(angle.y);
            sy = sin(angle.y);
            cy = zeroSmallValue( cy );
            sy = zeroSmallValue( sy );

            for (x=0; x<cols; x++)
            {
                angle.x = radianX * x;
                cx = cos(angle.x);
                sx = sin(angle.x);
                cx = zeroSmallValue( cx );
                sx = zeroSmallValue( sx );

                ux = sx * cy;
                uy = cx * cy;
                this->vertices[ index ].position = vec3( ux * x_radius, sy * y_radius, uy * x_radius );
                this->vertices[ index ].texcoord = vec2(float(x)/float(cols), float(y)/float(rows));//( angle.x, angle.y );
                this->vertices[ index ].normal   = vec3( ux, sy, uy );
                //std::cout <<std::setw(2) <<index <<": "<< vertices[ index ].position <<"\n";
                index++;
            }
            //std::cout<<"\n";
        }
        int vend = index;
        this->vertices[ index ].position = vec3( 0.f, -y_radius, 0.f );
        this->vertices[ index ].texcoord = vec2( 0.f, 0.f );
        this->vertices[ index ].normal   = vec3( 0.f, -1.f, 0.f );
        index++;

        index = 0;
        for (x=0; x<cols; ++x, index += 3)
        {
            indices[   index   ] = 0;
            indices[ index + 1 ] = x + 1;
            indices[ index + 2 ] = x + 2;
            //std::cout << indices[ i ].a <<", " << indices[ i ].b <<", " << indices[ i ].c <<"\n";
        }
        --index;
        indices[ index ] = 1;
        //std::cout << indices[ i ].a <<", " << indices[ i ].b <<", " << indices[ i ].c <<"\n";
        index++;

        int w[2];
        int r = cols;
        for (y=0; y<rows-2; y++)
        {
            w[0] = r * y;
            w[1] = r * (y + 1);
            for (x=1; x<=cols; x++)
            {
                a = x + w[0];
                b = a + 1;
                c = x + w[1];
                d = c + 1;

                indices[   index   ] = a;
                indices[ index + 1 ] = c;
                indices[ index + 2 ] = d;
                //std::cout <<std::setw(2) <<index <<": "<< indices[ index ].a <<", " << indices[ index ].b <<", " << indices[ index ].c <<"\n";
                index += 3;
                indices[   index   ] = d;
                indices[ index + 1 ] = b;
                indices[ index + 2 ] = a;
                //std::cout <<std::setw(2) <<index <<": "<< indices[ index ].a <<", " << indices[ index ].b <<", " << indices[ index ].c <<"\n";
                index += 3;
            }
            index -= 6;
            indices[ index + 2 ] = w[1] + 1;
            //std::cout <<std::setw(2) <<index <<": "<< indices[ index ].a <<", " << indices[ index ].b <<", " << indices[ index ].c <<"\n";
            index += 3;
            indices[   index   ] = w[1] + 1;
            indices[ index + 1 ] = w[0] + 1;
            //std::cout <<std::setw(2) <<index <<": "<< indices[ index ].a <<", " << indices[ index ].b <<", " << indices[ index ].c <<"\n\n";
            index += 3;
        }


        y = rows - 2;
        w[0] = r * y + 1;

        for (x=0; x<cols; ++x, index += 3)
        {
            indices[   index   ] = vend;
            indices[ index + 1 ] = x + w[0] + 1;
            indices[ index + 2 ] = x + w[0];
            //std::cout <<std::setw(2) <<index <<": "<< indices[ index ].a <<", " << indices[ index ].b <<", " << indices[ index ].c <<"\n";
        }
        index -= 3;
        indices[ index + 1 ] = w[0];
        //std::cout <<std::setw(2) <<index <<": "<< indices[ index ].a <<", " << indices[ index ].b <<", " << indices[ index ].c <<"\n";
        index += 3;
    }

}
