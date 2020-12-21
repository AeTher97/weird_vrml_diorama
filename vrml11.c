#include <stdio.h>
#include <math.h>
#include <time.h>

#define N1 100
#define N2 90
#define N3 160
#define N4 130

int main(void) {

    int i, j;
    float x, y, z;

    srand((unsigned) time(NULL));

    FILE *PlikVRML = fopen("c:/VRML/makieta.wrl", "w");

    fprintf(PlikVRML, "#VRML V2.0 utf8\n");

    fprintf(PlikVRML, "Shape {\n");
    fprintf(PlikVRML, "appearance Appearance { material Material { diffuseColor 0 1 0 } }\n");
    fprintf(PlikVRML, "geometry Box { size 200 0.05 50 } }\n");

    x = -100;
    y = 0.1;
    z = 0;

    do {
        fprintf(PlikVRML, "Transform {\n");
        fprintf(PlikVRML, "translation %f %f %f\n", x, y, z);
        fprintf(PlikVRML, "children [\n");
        fprintf(PlikVRML, "Shape {\n");
        fprintf(PlikVRML, "appearance Appearance { material Material { diffuseColor 1 0.5 0.25 } }\n");
        fprintf(PlikVRML, "geometry Box { size 0.3 0.2 2.5  } } ] }\n");
        x += 0.5;
    } while (x <= 100);

    fprintf(PlikVRML, "Transform {\n");
    fprintf(PlikVRML, "translation 0 0.3 -1\n");
    fprintf(PlikVRML, "children [\n");
    fprintf(PlikVRML, "Shape {\n");
    fprintf(PlikVRML, "appearance Appearance { material Material { diffuseColor 0.25 0.25 0.25 } }\n");
    fprintf(PlikVRML, "geometry Box { size 200 0.2 0.1  } } ] }\n");

    fprintf(PlikVRML, "Transform {\n");
    fprintf(PlikVRML, "translation 0 0.3 1\n");
    fprintf(PlikVRML, "children [\n");
    fprintf(PlikVRML, "Shape {\n");
    fprintf(PlikVRML, "appearance Appearance { material Material { diffuseColor 0.25 0.25 0.25 } }\n");
    fprintf(PlikVRML, "geometry Box { size 200 0.2 0.1  } } ] }\n");

    x = -100;
    i = 0;

    do {
        fprintf(PlikVRML, "Viewpoint {\n");
        fprintf(PlikVRML, "position %f 40 40\n", x);
        fprintf(PlikVRML, "orientation 1 0 0 -0.785\n");
        fprintf(PlikVRML, "description \"nr_%d\"}\n", i);
        x += 10;
        i++;
    } while (x <= 100);

    for (i = 0; i < N1; i++) {
        j = rand() % 2;
        x = -100 + rand() % 200;
        if (j)
            z = 5 + rand() % 20;
        else
            z = -5 - rand() % 20;
        fprintf(PlikVRML, "Transform {\n");
        fprintf(PlikVRML, "translation %f 0 %f\n", x, z);
        fprintf(PlikVRML, "children [\n");
        fprintf(PlikVRML, "Inline { url \"model1.wrl\"}]}\n");
    }

    for (i = 0; i < N2; i++) {
        j = rand() % 2;
        x = -100 + rand() % 200;
        if (j)
            z = 5 + rand() % 20;
        else
            z = -5 - rand() % 20;
        fprintf(PlikVRML, "Transform {\n");
        fprintf(PlikVRML, "translation %f 0 %f\n", x, z);
        fprintf(PlikVRML, "children [\n");
        fprintf(PlikVRML, "Inline { url \"model2.wrl\"}]}\n");
    }

    for (i = 0; i < N3; i++) {
        j = rand() % 2;
        x = -100 + rand() % 200;
        if (j)
            z = 5 + rand() % 20;
        else
            z = -5 - rand() % 20;
        fprintf(PlikVRML, "Transform {\n");
        fprintf(PlikVRML, "translation %f 0 %f\n", x, z);
        fprintf(PlikVRML, "children [\n");
        fprintf(PlikVRML, "Inline { url \"model3.wrl\"}]}\n");
    }

    for (i = 0; i < N4; i++) {
        j = rand() % 2;
        x = -90 + rand() % 180;
        if (j)
            z = 10 + rand() % 5;
        else
            z = -10 - rand() % 5;
        fprintf(PlikVRML, "Transform {\n");
        fprintf(PlikVRML, "translation %f 0 %f\n", x, z);
        fprintf(PlikVRML, "children [\n");
        fprintf(PlikVRML, "Inline { url \"model4.wrl\"}]}\n");
    }

    fprintf(PlikVRML, "DEF X TimeSensor {loop TRUE cycleInterval 10}\n");

    fprintf(PlikVRML, "DEF Y PositionInterpolator {\n");
    fprintf(PlikVRML, "key [0, 0.5, 1]\n");
    fprintf(PlikVRML, "keyValue [90 0 0, -90 0 0, 90 0 0]}\n");

    fprintf(PlikVRML, "Transform {\n");
    fprintf(PlikVRML, "translation 0 0.9 0\n");
    fprintf(PlikVRML, "children [\n");
    fprintf(PlikVRML, "DEF Z Transform {\n");
    fprintf(PlikVRML, "children [\n");
    óó
    fprintf(PlikVRML, "Inline { url \"wagon1.wrl\"}]}]}\n");

    fprintf(PlikVRML, "ROUTE X.fraction_changed TO Y.set_fraction\n");
    fprintf(PlikVRML, "ROUTE Y.value_changed TO Z.set_translation\n");

    fclose(PlikVRML);

    return 0;
}
