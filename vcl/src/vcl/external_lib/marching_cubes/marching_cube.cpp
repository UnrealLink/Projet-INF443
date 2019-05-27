#include "marching_cube.hpp"

mesh create_chunk(vec3 origin, const int nb_cubes, const float cube_size, float f(vec3), float isolevel){
    mesh chunk;
    int last=0;
    std::map<vec3, int> coordsToIdx;
    Cube cube;
    int ntriangles=0;
    for (int i=0; i<nb_cubes; i++){
        for (int j=0; j<nb_cubes; j++){
            for (int k=0; k<nb_cubes; k++){
                for (int l=0; l<8; l++){
                    cube.p[l] = origin + vec3(i*cube_size, j*cube_size, k*cube_size);
                    cube.val[l] = f(cube.p[l]);
                }
                ntriangles += compute_cube(cube, isolevel, chunk, coordsToIdx, last);
            }
        }
    }

    return chunk;

}


int compute_cube(Cube cube, float isolevel, mesh &chunk, std::map<vec3, int> &coordsToIdx, int &last){

    vec3 points[12];

    /*
        Determine the index into the edge table which
        tells us which vertices are inside of the surface
    */
    int cubeindex = 0;
    if (cube.val[0] < isolevel) cubeindex |= 1;
    if (cube.val[1] < isolevel) cubeindex |= 2;
    if (cube.val[2] < isolevel) cubeindex |= 4;
    if (cube.val[3] < isolevel) cubeindex |= 8;
    if (cube.val[4] < isolevel) cubeindex |= 16;
    if (cube.val[5] < isolevel) cubeindex |= 32;
    if (cube.val[6] < isolevel) cubeindex |= 64;
    if (cube.val[7] < isolevel) cubeindex |= 128;

    /* Cube is entirely in/out of the surface */
    if (edgeTable[cubeindex] == 0)
        return(0);

    /* Find the vertices where the surface intersects the cube */
    if (edgeTable[cubeindex] & 1)
        points[0] =
            interpolate(isolevel,cube.p[0],cube.p[1],cube.val[0],cube.val[1]);
    if (edgeTable[cubeindex] & 2)
        points[1] =
            interpolate(isolevel,cube.p[1],cube.p[2],cube.val[1],cube.val[2]);
    if (edgeTable[cubeindex] & 4)
        points[2] =
            interpolate(isolevel,cube.p[2],cube.p[3],cube.val[2],cube.val[3]);
    if (edgeTable[cubeindex] & 8)
        points[3] =
            interpolate(isolevel,cube.p[3],cube.p[0],cube.val[3],cube.val[0]);
    if (edgeTable[cubeindex] & 16)
        points[4] =
            interpolate(isolevel,cube.p[4],cube.p[5],cube.val[4],cube.val[5]);
    if (edgeTable[cubeindex] & 32)
        points[5] =
            interpolate(isolevel,cube.p[5],cube.p[6],cube.val[5],cube.val[6]);
    if (edgeTable[cubeindex] & 64)
        points[6] =
            interpolate(isolevel,cube.p[6],cube.p[7],cube.val[6],cube.val[7]);
    if (edgeTable[cubeindex] & 128)
        points[7] =
            interpolate(isolevel,cube.p[7],cube.p[4],cube.val[7],cube.val[4]);
    if (edgeTable[cubeindex] & 256)
        points[8] =
            interpolate(isolevel,cube.p[0],cube.p[4],cube.val[0],cube.val[4]);
    if (edgeTable[cubeindex] & 512)
        points[9] =
            interpolate(isolevel,cube.p[1],cube.p[5],cube.val[1],cube.val[5]);
    if (edgeTable[cubeindex] & 1024)
        points[10] =
            interpolate(isolevel,cube.p[2],cube.p[6],cube.val[2],cube.val[6]);
    if (edgeTable[cubeindex] & 2048)
        points[11] =
            interpolate(isolevel,cube.p[3],cube.p[7],cube.val[3],cube.val[7]);

    /* Create the triangle */
    int ntriang = 0;
    for (int i=0; triTable[cubeindex][i] != -1; i+=3) {
        vec3 p1 = points[triTable[cubeindex][i  ]];
        vec3 p2 = points[triTable[cubeindex][i+1]];
        vec3 p3 = points[triTable[cubeindex][i+2]];
        unsigned int i1, i2, i3;
        std::map<vec3, int>::iterator it;
        it = coordsToIdx.find(p1);
        if(it != coordsToIdx.end()) {i1 = it->second;} else {i1 = last++; chunk.position.push_back(p1); coordsToIdx[p1]=i1;}
        it = coordsToIdx.find(p2);
        if(it != coordsToIdx.end()) {i2 = it->second;} else {i2 = last++; chunk.position.push_back(p2); coordsToIdx[p2]=i2;}
        it = coordsToIdx.find(p3);
        if(it != coordsToIdx.end()) {i3 = it->second;} else {i3 = last++; chunk.position.push_back(p3); coordsToIdx[p3]=i3;}
        const index3 triangle = {i1, i2, i3};
        chunk.connectivity.push_back(triangle);
        ntriang++;
    }

   return(ntriang);
}

vec3 interpolate(float isolevel, vec3 p1, vec3 p2, float val1, float val2)
{
    if (p2 < p1)
    {
        vec3 temp;
        temp = p1;
        p1 = p2;
        p2 = temp;
    }

    vec3 p;
    if(fabs(val1 - val2) > 0.00001)
        p = (vec3)p1 + ((vec3)p2 - (vec3)p1)/(val2 - val1)*(isolevel - val1);
    else
        p = (vec3)p1;
    return p;
}
