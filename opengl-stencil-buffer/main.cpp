#define STB_IMAGE_IMPLEMENTATION
#include <windows.h>
#include <GL/glut.h>
#include <vector>
#include <iostream>

// Error handling function
void checkOpenGLError(const std::string& message) {
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL Error (" << message << "): " << gluErrorString(error) << std::endl;
        // Handle the error appropriately (e.g., exit, log, etc.)
    }
}

void compositeImagesWithDepth(const unsigned char* depthData, const unsigned char* colorData, int width, int height) {
    // 1. Clear stencil buffer
    glClear(GL_STENCIL_BUFFER_BIT);
    checkOpenGLError("Clearing stencil buffer");

    // 2. Disable writing to the color buffer
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    checkOpenGLError("Disabling color writes");

    // 3. Set stencil operations
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, 1, 1);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    checkOpenGLError("Setting stencil operations");

    // 4. Enable depth testing
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    checkOpenGLError("Enabling depth test");

    // 5. Draw depth values
    glDrawPixels(width, height, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, depthData);
    checkOpenGLError("Drawing depth values");

    // 6. Set stencil function for masking
    glStencilFunc(GL_EQUAL, 1, 1);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    checkOpenGLError("Setting stencil function for masking");

    // 7. Disable depth testing
    glDisable(GL_DEPTH_TEST);
    checkOpenGLError("Disabling depth test");

    // 8. Enable writing to the color buffer
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    checkOpenGLError("Enabling color writes");

    // 9. Draw color values
    glDrawPixels(width, height, GL_RGBA, GL_UNSIGNED_BYTE, colorData);
    checkOpenGLError("Drawing color values");

    // 10. Disable stencil test (optional, but good practice)
    glDisable(GL_STENCIL_TEST);
    checkOpenGLError("Disabling stencil test");
}

// Global variables to store image data
std::vector<unsigned char> depthData1, depthData2;
std::vector<unsigned char> colorData1, colorData2;

// Global variable to control the depth of the green triangle
float greenTriangleDepth = -1.0f;

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'w':
        greenTriangleDepth += 0.1f; // Increase depth (move farther)
        std::cerr << "increase" << std::endl;
        break;
    case 's':
        greenTriangleDepth -= 0.1f; // Decrease depth (move closer)
        std::cerr << "decrease" << std::endl;
        break;
    }
    glutPostRedisplay(); // Trigger a redraw
}

void display() {
    // ... (Clear color, depth, and stencil buffers and enable depth and stencil tests)

    // 1. Draw red triangle
    glStencilFunc(GL_ALWAYS, 1, 1);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glDepthMask(GL_FALSE);
    glBegin(GL_TRIANGLES);
    glColor3f(1.0f, 0.0f, 0.0f); // Red
    glVertex3f(-0.2f, -0.2f, -1.0f);
    glVertex3f(0.2f, -0.2f, -1.0f);
    glVertex3f(0.0f, 0.2f, -1.0f);
    glEnd();
    glDepthMask(GL_TRUE); // Re-enable depth buffer writes

    // Read depth and color data for the triangle
    glReadPixels(0, 0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT), GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, depthData1.data());
    glReadPixels(0, 0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT), GL_RGBA, GL_UNSIGNED_BYTE, colorData1.data());
    compositeImagesWithDepth(depthData1.data(), colorData1.data(), glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));

    // 2. Draw green triangle (behind the red triangle)
    // DO NOT CLEAR DEPTH AND STENCIL BUFFERS HERE
    glStencilFunc(GL_EQUAL, 0, 1); // Draw only where the stencil value is 1 (red triangle)
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    glBegin(GL_TRIANGLES);
    glColor3f(0.0f, 1.0f, 0.0f); // Green
    glVertex3f(-0.4f, -0.4f, greenTriangleDepth); // Use the global depth variable
    glVertex3f(0.4f, -0.4f, greenTriangleDepth);
    glVertex3f(0.0f, 0.4f, greenTriangleDepth);
    glEnd();

    // Read depth and color data for the square (now a triangle)
    glReadPixels(0, 0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT), GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, depthData2.data());
    glReadPixels(0, 0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT), GL_RGBA, GL_UNSIGNED_BYTE, colorData2.data());

    // Composite the images
    compositeImagesWithDepth(depthData2.data(), colorData2.data(), glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));

    glutSwapBuffers();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
    glutInitWindowSize(512, 512);
    glutCreateWindow("Image Compositing");

    // Allocate memory for depth and color data after window creation
    depthData1.resize(glutGet(GLUT_WINDOW_WIDTH) * glutGet(GLUT_WINDOW_HEIGHT));
    colorData1.resize(glutGet(GLUT_WINDOW_WIDTH) * glutGet(GLUT_WINDOW_HEIGHT) * 4); // RGBA
    depthData2.resize(glutGet(GLUT_WINDOW_WIDTH) * glutGet(GLUT_WINDOW_HEIGHT));
    colorData2.resize(glutGet(GLUT_WINDOW_WIDTH) * glutGet(GLUT_WINDOW_HEIGHT) * 4); // RGBA

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard); // Register the keyboard function
    glutMainLoop();

    return 0;
}
