#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "pix.h"
#include "pixShader.h"
#include <fcntl.h>

char* textfile(const char* path)
{
    int fd = open(path, O_RDONLY);
    off_t len = lseek(fd, 0, SEEK_END);
    char* txt = calloc(len + 1, 1);

    if(!fd) printf("File '%s' not opened!\n", path);

    printf("Reading file '%s' (%lldB)\n", path, len);

    lseek(fd, 0, SEEK_SET);
    read(fd, txt, len);
    close(fd);

    return txt;
}

int main(void)
{
    struct PixDesc settings = {
        .width  = 800,
        .height = 600,
        .name   = "PIX",
    };
    struct PixShader shader;
    char* shaderSrcs[] = {
        textfile("./tests/shaders/color.vert"),
        textfile("./tests/shaders/color.frag"),
    };

    pixInit(settings);

    float t = 0;
    int ret = pixShaderFromSource((const char**)shaderSrcs, &shader);

    while (pixShouldBegin())
    {
        glClearColor(
            fabs(sinf(t)), 
            fabs(sinf(t + M_PI / 2)), 
            0,
            1
        );
        t += 0.01;

        pixPresent();
    }

    pixDestroy();
}
