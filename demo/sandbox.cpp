#include <minigl.hpp>
#include <iostream>
using namespace std;
using namespace minigl;

int main(void)
{
    window my_window = window(1024_px, 768_px, "Sandbox"); 
    if (!my_window.ok()) {return 1;}
    GLint max_texs;
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &max_texs);
    cout << "max texes: " << max_texs << endl;
}