#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <windows.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <iostream>



// Image dimensions (adjust as needed)
const int imageWidth = 512;
const int imageHeight = 512;

// Pixel storage (replace with your image loading/saving)
unsigned char image1Color[imageWidth * imageHeight * 3]; // RGB for image 1
float image1Depth[imageWidth * imageHeight];
unsigned char image2Color[imageWidth * imageHeight * 3]; // RGB for image 2
float image2Depth[imageWidth * imageHeight];


// ... (other includes and variables)

void init_load_images () {
    int x, y, n;
    unsigned char* data;

    // Load Image 1 (Color and Depth)
    data = stbi_load("resources/image1.png", &x, &y, &n, 3); // 3 for RGB
    if (data) {
        memcpy(image1Color, data, imageWidth * imageHeight * 3);
        stbi_image_free(data);
    }
    else {
        std::cerr << "Error loading image1.png" << std::endl;
    }

    data = stbi_load("resources/image1_depth.png", &x, &y, &n, 1); // 1 for grayscale depth
    if (data) {
        for (int i = 0; i < imageWidth * imageHeight; ++i) {
            image1Depth[i] = data[i] / 255.0f; // Normalize to [0, 1]
        }
        stbi_image_free(data);
    }
    else {
        std::cerr << "Error loading image1_depth.png" << std::endl;
    }

    // Load Image 2 (Color and Depth) 
    data = stbi_load("resources/image2.png", &x, &y, &n, 3);
    if (data) {
        memcpy(image2Color, data, imageWidth * imageHeight * 3);
        stbi_image_free(data);
    }
    else {
        std::cerr << "Error loading image2.png" << std::endl;
    }

    data = stbi_load("resources/image2_depth.png", &x, &y, &n, 1);
    if (data) {
        for (int i = 0; i < imageWidth * imageHeight; ++i) {
            image2Depth[i] = data[i] / 255.0f;
        }
        stbi_image_free(data);
    }
    else {
        std::cerr << "Error loading image2_depth.png" << std::endl;
    }
}

void init_draw_images() {
    // Create Image 1 (Red Triangle)
    for (int i = 0; i < imageWidth * imageHeight; ++i) {
        image1Color[i * 3] = 255;     // Red
        image1Color[i * 3 + 1] = 0;   // Green
        image1Color[i * 3 + 2] = 0;   // Blue
        image1Depth[i] = 0.8f;        // Mid-depth for triangle
    }

    // Create Image 2 (Green Square)
    for (int i = 0; i < imageWidth * imageHeight; ++i) {
        image2Color[i * 3] = 0;       // Red
        image2Color[i * 3 + 1] = 255; // Green
        image2Color[i * 3 + 2] = 0;   // Blue
        image2Depth[i] = 0.5f;        // Farther depth for square
    }
}



void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, 1, 1);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE); // Disable color writes
    glDepthMask(GL_TRUE); // Make sure the depth mask is enabled

    // 1. Composite Depth (Image 1 & 2)
    glDrawPixels(imageWidth, imageHeight, GL_DEPTH_COMPONENT, GL_FLOAT, image1Depth);
    glDrawPixels(imageWidth, imageHeight, GL_DEPTH_COMPONENT, GL_FLOAT, image2Depth);

    // 2. Composite Color (Image 1 & 2)
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE); // Enable color writes
    glStencilFunc(GL_EQUAL, 1, 1);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    glDisable(GL_DEPTH_TEST);
    glDrawPixels(imageWidth, imageHeight, GL_RGB, GL_UNSIGNED_BYTE, image1Color);
    glDrawPixels(imageWidth, imageHeight, GL_RGB, GL_UNSIGNED_BYTE, image2Color);

    glDisable(GL_STENCIL_TEST);

    glutSwapBuffers();
}

void reshape(int width, int height) {
    glViewport(0, 0, width, height); // Update the viewport
    glMatrixMode(GL_PROJECTION);    // Switch to projection matrix mode
    glLoadIdentity();                // Reset the projection matrix

    // Set up your projection matrix here (e.g., using gluOrtho2D or gluPerspective)
    // ... based on the new width and height

    glMatrixMode(GL_MODELVIEW);     // Switch back to modelview matrix mode
}

// ... (rest of your GLUT setup: init, reshape, main, etc.)
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STENCIL);
    glutInitWindowSize(imageWidth, imageHeight);
    glutCreateWindow("Image Compositing with Depth");

    // ... (Your initialization code: load images, set OpenGL parameters, etc.)

    init_draw_images();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape); // Add reshape callback if needed
    glutMainLoop();
    return 0;
}