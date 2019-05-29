#include "perlin3DTexture.hpp"

GLuint createPerlin3DTexture()
{
    GLuint id = 0;
    glGenTextures(1,&id);
    glBindTexture(GL_TEXTURE_3D,id);

    const int n = 16;
    GLfloat data[n*n*n*4];
    for (size_t i = 0; i < n*n*n*4; i = i+4)
    {
        float u = (i%n)/float(n);
        float v = ((i/n)%n)/float(n);
        float w = i/(n*n);
        data[i] = perlin(u,v,w);
        data[i+1] = perlin(w, u, v);
        data[i+2] = perlin(v, w, u);
        data[i+3] = 1.f;
    }

    // Send texture on GPU
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA8, n, n, n, 0, GL_RGBA,  GL_FLOAT, &data[0]); opengl_debug();
    glGenerateMipmap(GL_TEXTURE_3D); opengl_debug();

    // Set default texture behavior
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT); opengl_debug();
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT); opengl_debug();

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); opengl_debug();
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); opengl_debug();

    glBindTexture(GL_TEXTURE_3D,0);

    std::cout << "created" << std::endl;
    return id;
}
