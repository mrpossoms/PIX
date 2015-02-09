#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include "pix.h"
#include <math.h>

int main(void)
{
    struct PixDesc settings = {
        .width  = 800,
        .height = 600,
        .name   = "PIX",
    };

    pixInit(settings);

    float t = 0;

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
