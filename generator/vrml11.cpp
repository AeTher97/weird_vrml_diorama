#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <vector>
#include <string>

#define N1 200
#define N2 200
#define N3 160
#define N4 130

float mod(int a, int n) {
    return a - floor((float) a / (float) n) * n;
}

float toDegrees(float radians) {
    return radians / 6.28 * 360;
}

class Point {
public:
    float x;
    float y;
    float rotation;

    Point(float x, float y, float rotation) {
        this->x = x;
        this->y = y;
        this->rotation = rotation;
    }
};

class BlockedLocation {
public:
    float x;
    float y;
    float width;
    float height;

    BlockedLocation(float x, float y, float height, float width) {
        this->x = x;
        this->y = y;
        this->width = width;
        this->height = height;
    }
};

enum WagonType {
    CIUCHCIA,
    CYSTERNA,
    WAGUN
};

void createWagon(FILE *file, int i, std::vector<Point> points, WagonType wagonType) {
    int separation = i * 22;
    fprintf(file, "DEF Y%d PositionInterpolator {\n", i);
    fprintf(file, "key [");
    for (int g = 0; g < points.size() - 1; g++) {
        fprintf(file, "%f,", (float) g * (1.0f / (float) points.size()));
    }
    fprintf(file, "%f]\n", 1.0);

    fprintf(file, "keyValue [");
    for (int g = 0; g < points.size() - 1; g++) {
        int u = g + separation;
        if (u >= points.size()) {
            u -= points.size();
        }
        fprintf(file, "%f %f %f,", points.at(u).x, 0.0f, points.at(u).y);
    }
    int f = points.size() - 1 + separation;
    if (f >= points.size()) {
        f -= points.size();
    }
    fprintf(file, "%f %f %f]}\n", points.at(f).x, 0.0f, points.at(f).y);

    fprintf(file, "DEF R%d OrientationInterpolator {\n", i);
    fprintf(file, "key [");
    for (int g = 0; g < points.size() - 1; g++) {
        fprintf(file, "%f,", (float) (g) * (1.0f / (float) points.size()));
    }
    fprintf(file, "%f]\n", 1.0);
    fprintf(file, "keyValue [");
    for (int g = 0; g < points.size() - 1; g++) {
        int u = g + separation;
        if (u >= points.size()) {
            u -= points.size();
        }
        fprintf(file, "0 1 0 %f,", points.at(u).rotation);
    }
    f = points.size() - 1 + separation;
    if (f >= points.size()) {
        f -= points.size();
    }
    fprintf(file, "0 1 0 %f]}\n", points.at(f).rotation);

    fprintf(file, "Transform {\n");
    fprintf(file, "translation 0 0.9 0\n");
    fprintf(file, "rotation 0 1 0 0\n");
    fprintf(file, "children [\n");
    fprintf(file, "DEF Z%d Transform {\n", i);
    fprintf(file, "children [\n");

    if (wagonType == CIUCHCIA) {
        fprintf(file, "Inline { url \"ciuchcia.wrl\"}]}]}\n");
    } else if (wagonType == CYSTERNA) {
        fprintf(file, "Inline { url \"wagon1.wrl\"}]}]}\n");
    } else {
        fprintf(file, "Inline { url \"wagun.wrl\"}]}]}\n");
    }

    fprintf(file, "ROUTE X.fraction_changed TO Y%d.set_fraction\n", i);
    fprintf(file, "ROUTE X.fraction_changed TO R%d.set_fraction\n", i);
    fprintf(file, "ROUTE Y%d.value_changed TO Z%d.set_translation\n", i, i);
    fprintf(file, "ROUTE R%d.value_changed TO Z%d.set_rotation\n", i, i);
}

void createObject(FILE *file, char *name, float x, float y, float z, float rotation) {
    fprintf(file, "Transform {\n");
    fprintf(file, "translation %f %f %f\n", x, y, z);
    fprintf(file, "rotation 0 1 0 %f\n", rotation);
    fprintf(file, "children [\n");
    fprintf(file, "Transform {\n");
    fprintf(file, "children [\n");

    fprintf(file, "Inline { url \"%s.wrl\"}]}]}\n", name);

}

bool checkIfInBlocked(float x, float y, BlockedLocation blockedLocation, float border) {
    if ((x > blockedLocation.x - border && x < blockedLocation.x + blockedLocation.height + border) &&
        (y > blockedLocation.y - border && y < blockedLocation.y + blockedLocation.width + border)) {
        return true;
    } else {
        return false;
    }
}

bool checkIfBlocked(float x, float y, std::vector<BlockedLocation> blocked, float border) {
    bool blockedBool = false;
    for (int i = 0; i < blocked.size(); i++) {
        if (checkIfInBlocked(x, y, blocked.at(i), border)) {
            blockedBool = true;
        }
    }
    return blockedBool;
}


void spawnOutsideBlockedArea(FILE *file, char *name, std::vector<BlockedLocation> blocked, float border = 0.0) {

    float x = (float) (rand() % 1000) / 5.0f - 100.0f;
    float y = (float) (rand() % 1000) / 5.0f - 100.0f;
    while (checkIfBlocked(x, y, blocked, border)) {
        x = (float) (rand() % 1000) / 5.0f - 100.0f;
        y = (float) (rand() % 1000) / 5.0f - 100.0f;
    }

    createObject(file, name, x, 0, y, 0);
}


std::vector<Point>
createStraightTrack(FILE *file, float startX, float startY, int direction1, int direction2, float length) {
    float separation = 0.5f;
    std::vector<Point> points = std::vector<Point>();
    for (int i = 0; i < (int) (length / 0.5f); i++) {
        fprintf(file, "Transform {\n");
        fprintf(file, "translation %f %f %f\n", startX + ((float) direction1 * ((float) i * separation)), 0.1,
                startY + ((float) direction2 * (float) i * separation));
        float rotation;
        if (direction2 == -1) {
            rotation = 1.57f;
        } else if (direction2 == 1) {
            rotation = -1.57f;
        } else {
            if (direction1 == 1) {
                rotation = 0.0f;
            } else {
                rotation = 3.14f;
            }
        }
        fprintf(file, "rotation 0 1 0 %f\n", rotation);
        points.push_back(Point(startX + ((float) direction1 * ((float) i * separation)),
                               startY + ((float) direction2 * (float) i * separation), rotation));
        fprintf(file, "children [\n");
        fprintf(file, "Shape {\n");
        fprintf(file, "appearance Appearance { material Material { diffuseColor 1 0.5 0.25 } }\n");
        fprintf(file, "geometry Box { size 0.3 0.2 2.5  } } ] }\n");
    }

    float lTrackStartX = 0;
    float lTrackStartY = 0;
    if (direction1 != 0) {
        lTrackStartX = startX + direction1 * length / 2 - 0.15;
        lTrackStartY = startY - 1;
    } else {
        lTrackStartX = startX - 1;
        lTrackStartY = startY + direction2 * length / 2 - 0.15;
    }

    fprintf(file, "Transform {\n");
    fprintf(file, "translation %f 0.3 %f\n", lTrackStartX, lTrackStartY);
    fprintf(file, "rotation 0 1 0 %f\n", direction2 != 0 ? 1.57f : 0);
    fprintf(file, "children [\n");
    fprintf(file, "Shape {\n");
    fprintf(file, "appearance Appearance { material Material { diffuseColor 0.25 0.25 0.25 } }\n");
    fprintf(file, "geometry Box { size %f 0.2 0.1  } } ] }\n", length);


    float rTrackStartX = 0;
    float rTrackStartY = 0;
    if (direction1 != 0) {
        rTrackStartX = startX + direction1 * length / 2 - 0.15;
        rTrackStartY = startY + 1;
    } else {
        rTrackStartX = startX + 1;
        rTrackStartY = startY + direction2 * length / 2 - 0.15;
    }
    fprintf(file, "Transform {\n");
    fprintf(file, "translation %f 0.3 %f\n", rTrackStartX, rTrackStartY);
    fprintf(file, "rotation 0 1 0 %f\n", direction2 != 0 ? 1.57f : 0);
    fprintf(file, "children [\n");
    fprintf(file, "Shape {\n");
    fprintf(file, "appearance Appearance { material Material { diffuseColor 0.25 0.25 0.25 } }\n");
    fprintf(file, "geometry Box { size %f 0.2 0.1  } } ] }\n", length);
    return points;
}

std::vector<Point>
createCorner(FILE *file, float startX, float startY, int direction1, int direction2, int horizontal) {
    std::vector<Point> points = std::vector<Point>();

    float separation = 0.5f;
    int number = 60;
    float currentX = startX;
    float currentY = startY;
    for (int i = 0; i < number + 2; i++) {
        fprintf(file, "Transform {\n");
        fprintf(file, "translation %f 0.1 %f\n", currentX, currentY);

        float rotation;
        if ((direction1 == 1 && direction2 == 0) || (direction1 == 0 && direction2 == 1)) {
            if (horizontal == 0) {
                fprintf(file, "rotation 0 1 0 %f\n", +1.57 / (float) number * i);
                rotation = +1.57f / (float) number * i;
            } else {
                fprintf(file, "rotation 0 1 0 %f\n", 1.57 - 1.57 / (float) number * i);
                rotation = 1.57f - 1.57f / (float) number * i;

            }
        } else {
            if (horizontal == 0) {
                fprintf(file, "rotation 0 1 0 %f\n", -1.57 / (float) number * i);
                rotation = -1.57f / (float) number * i;
            } else {
                fprintf(file, "rotation 0 1 0 %f\n", 1.57 + 1.57 / (float) number * i);
                rotation = 1.57f + 1.57f / (float) number * i;

            }

        }

        points.push_back(Point(currentX,
                               currentY, rotation));

        fprintf(file, "children [\n");
        fprintf(file, "Shape {\n");
        fprintf(file, "appearance Appearance { material Material { diffuseColor 1 0.5 0.25 } }\n");
        fprintf(file, "geometry Box { size 0.3 0.2 2.5  } } ] }\n");
        float angle = 90.0f / (float) number * (float) i;
        float angleInRad = angle / 360.0f * 6.28f;
        if (direction1 == 0) {
            if (horizontal == 0)
                currentX += separation * cos(angleInRad);
            else
                currentX += separation * sin(angleInRad);
        } else {
            if (horizontal == 0)
                currentX -= separation * cos(angleInRad);
            else
                currentX -= separation * sin(angleInRad);

        }
        if (direction2 == 0) {
            if (horizontal == 0)
                currentY += separation * sin(angleInRad);
            else
                currentY += separation * cos(angleInRad);

        } else {
            if (horizontal == 0)
                currentY -= separation * sin(angleInRad);
            else
                currentY -= separation * cos(angleInRad);
        }
    }
    separation = 0.475f;
    if (horizontal == 0) {
        currentX = startX;
    } else {
        if (direction1 == 0) {
            currentX = startX + 1;
        } else {
            currentX = startX - 1;
        }
    }
    if (horizontal == 0) {
        if (direction2 == 0) {
            currentY = startY + 1;
        } else {
            currentY = startY - 1;
        }
    } else {
        currentY = startY;
    }
    for (int i = 0; i < number + 2; i++) {
        fprintf(file, "Transform {\n");
        fprintf(file, "translation %f 0.3 %f\n", currentX, currentY);
        if ((direction1 == 1 && direction2 == 0) || (direction1 == 0 && direction2 == 1)) {
            if (horizontal == 0)
                fprintf(file, "rotation 0 1 0 %f\n", +1.57 / (float) number * i);
            else
                fprintf(file, "rotation 0 1 0 %f\n", 1.57 - 1.57 / (float) number * i);
        } else {
            if (horizontal == 0)
                fprintf(file, "rotation 0 1 0 %f\n", -1.57 / (float) number * i);
            else
                fprintf(file, "rotation 0 1 0 %f\n", 1.57 + 1.57 / (float) number * i);

        }
        fprintf(file, "children [\n");
        fprintf(file, "Shape {\n");
        fprintf(file, "appearance Appearance { material Material { diffuseColor 0.25 0.25 0.25 } }\n");
        fprintf(file, "geometry Box { size %f 0.2 0.1  } } ] }\n", 0.5);
        float angle = 90.0f / (float) number * (float) i;
        float angleInRad = angle / 360.0f * 6.28f;
        if (direction1 == 0) {
            if (horizontal == 0)
                currentX += separation * cos(angleInRad);
            else
                currentX += separation * sin(angleInRad);
        } else {
            if (horizontal == 0)
                currentX -= separation * cos(angleInRad);
            else
                currentX -= separation * sin(angleInRad);

        }
        if (direction2 == 0) {
            if (horizontal == 0)
                currentY += separation * sin(angleInRad);
            else
                currentY += separation * cos(angleInRad);

        } else {
            if (horizontal == 0)
                currentY -= separation * sin(angleInRad);
            else
                currentY -= separation * cos(angleInRad);
        }
    }
    separation = 0.525f;

    if (horizontal == 0) {
        currentX = startX;
    } else {
        if (direction1 == 0) {
            currentX = startX - 1;
        } else {
            currentX = startX + 1;
        }
    }
    if (horizontal == 0) {
        if (direction2 == 0) {
            currentY = startY - 1;
        } else {
            currentY = startY + 1;
        }
    } else {
        currentY = startY;
    }
    for (int i = 0; i < number + 2; i++) {
        fprintf(file, "Transform {\n");
        fprintf(file, "translation %f 0.3 %f\n", currentX, currentY);
        if ((direction1 == 1 && direction2 == 0) || (direction1 == 0 && direction2 == 1)) {
            if (horizontal == 0)
                fprintf(file, "rotation 0 1 0 %f\n", +1.57 / (float) number * i);
            else
                fprintf(file, "rotation 0 1 0 %f\n", 1.57 - 1.57 / (float) number * i);
        } else {
            if (horizontal == 0)
                fprintf(file, "rotation 0 1 0 %f\n", -1.57 / (float) number * i);
            else
                fprintf(file, "rotation 0 1 0 %f\n", 1.57 + 1.57 / (float) number * i);

        }
        fprintf(file, "children [\n");
        fprintf(file, "Shape {\n");
        fprintf(file, "appearance Appearance { material Material { diffuseColor 0.25 0.25 0.25 } }\n");
        fprintf(file, "geometry Box { size %f 0.2 0.1  } } ] }\n", 0.5);
        float angle = 90.0f / (float) number * (float) i;
        float angleInRad = angle / 360.0f * 6.28f;
        if (direction1 == 0) {
            if (horizontal == 0)
                currentX += separation * cos(angleInRad);
            else
                currentX += separation * sin(angleInRad);
        } else {
            if (horizontal == 0)
                currentX -= separation * cos(angleInRad);
            else
                currentX -= separation * sin(angleInRad);

        }
        if (direction2 == 0) {
            if (horizontal == 0)
                currentY += separation * sin(angleInRad);
            else
                currentY += separation * cos(angleInRad);

        } else {
            if (horizontal == 0)
                currentY -= separation * sin(angleInRad);
            else
                currentY -= separation * cos(angleInRad);
        }
    }

    return points;
}

int main(void) {
    std::vector<Point> points = std::vector<Point>();

    int i, j;
    float x, y, z;

    srand((unsigned) time(NULL));

    FILE *PlikVRML = fopen("E:\\Dysk Google\\AGH\\Systemy Programowania w Grafice\\output.wrl", "w");

    fprintf(PlikVRML, "#VRML V2.0 utf8\n");

    fprintf(PlikVRML, "Shape {\n");
    fprintf(PlikVRML, "appearance Appearance { material Material { diffuseColor 0 1 0 } }\n");
    fprintf(PlikVRML, "geometry Box { size 200 0.05 200 } }\n");

    fprintf(PlikVRML, "Transform {\n");
    fprintf(PlikVRML, "translation -20 0.1 0\n");
    fprintf(PlikVRML, "children [\n");
    fprintf(PlikVRML, "Shape {\n");
    fprintf(PlikVRML, "appearance Appearance { material Material { diffuseColor 0.41 0.41 0.41 } }\n");
    fprintf(PlikVRML, "geometry Box { size 10 0.1 200 } } ] }\n");
    for (int l = 0; l < 50; l++) {
        fprintf(PlikVRML, "Transform {\n");
        fprintf(PlikVRML, "translation -20 0.2 %d\n", -100 + l * 4);
        fprintf(PlikVRML, "children [\n");
        fprintf(PlikVRML, "Shape {\n");
        fprintf(PlikVRML, "appearance Appearance { material Material { diffuseColor 1 1 1 } }\n");
        fprintf(PlikVRML, "geometry Box { size 0.2 0.1 2 } } ] }\n");
    }

    x = -70;
    y = 0.1;
    z = 0;

    std::vector<Point> newPoints = std::vector<Point>();
    std::vector<BlockedLocation> blockedAreas = std::vector<BlockedLocation>();


    newPoints = createStraightTrack(PlikVRML, -50, -70, 1, 0, 100);
    blockedAreas.push_back(BlockedLocation(-30, -100, 20, 200));

    blockedAreas.push_back(BlockedLocation(100, -100, 10, 200));
    blockedAreas.push_back(BlockedLocation(-100, -110, 200, 10));
    blockedAreas.push_back(BlockedLocation(-100, 100, 200, 10));
    blockedAreas.push_back(BlockedLocation(-110, -100, 10, 200));

    blockedAreas.push_back(BlockedLocation(-50, -75, 100, 10));
    blockedAreas.push_back(BlockedLocation(50, -75, 25, 25));
    blockedAreas.push_back(BlockedLocation(65, -50, 10, 30));
    blockedAreas.push_back(BlockedLocation(45, -30, 25, 25));
    blockedAreas.push_back(BlockedLocation(25, -15, 30, 30));
    blockedAreas.push_back(BlockedLocation(25, 15, 30, 30));
    blockedAreas.push_back(BlockedLocation(50, 25, 30, 30));
    blockedAreas.push_back(BlockedLocation(50, 40, 30, 30));
    blockedAreas.push_back(BlockedLocation(-50, 60, 100, 10));
    blockedAreas.push_back(BlockedLocation(-75, -55, 10, 100));
    blockedAreas.push_back(BlockedLocation(-75, -75, 30, 30));
    blockedAreas.push_back(BlockedLocation(-75, 40, 30, 30));


    points.insert(points.end(), newPoints.begin(), newPoints.end());
    newPoints = createCorner(PlikVRML, 50, -70, 0, 0, 0);
    points.insert(points.end(), newPoints.begin(), newPoints.end());
    newPoints = createStraightTrack(PlikVRML, 69.35f, -50.25f, 0, 1, 20);
    points.insert(points.end(), newPoints.begin(), newPoints.end());
    newPoints = createCorner(PlikVRML, 69.35f, -30.25f, 1, 0, 1);
    points.insert(points.end(), newPoints.begin(), newPoints.end());
    newPoints = createCorner(PlikVRML, 50.0f, -10.9f, 1, 0, 0);
    points.insert(points.end(), newPoints.begin(), newPoints.end());
    newPoints = createCorner(PlikVRML, 30.65f, 8.45f, 0, 0, 1);
    points.insert(points.end(), newPoints.begin(), newPoints.end());
    newPoints = createCorner(PlikVRML, 50.0f, 27.8f, 0, 0, 0);
    points.insert(points.end(), newPoints.begin(), newPoints.end());
    newPoints = createCorner(PlikVRML, 69.35f, 47.15f, 1, 0, 1);
    points.insert(points.end(), newPoints.begin(), newPoints.end());
    newPoints = createStraightTrack(PlikVRML, 50.0f, 66.5f, -1, 0, 100);
    points.insert(points.end(), newPoints.begin(), newPoints.end());
    newPoints = createCorner(PlikVRML, -50.0f, 66.5f, 1, 1, 0);
    points.insert(points.end(), newPoints.begin(), newPoints.end());
    newPoints = createStraightTrack(PlikVRML, -69.35f, 47.15f, 0, -1, 97.85f);
    points.insert(points.end(), newPoints.begin(), newPoints.end());
    newPoints = createCorner(PlikVRML, -69.35f, -50.65f, 0, 1, 1);
    points.insert(points.end(), newPoints.begin(), newPoints.end());


    for (int o = 0; o < points.size() - 1; o++) {
        if (abs(points.at(o).rotation - points.at(o + 1).rotation) > 0.5f) {
            if (points.at(o + 1).rotation > 0) {
                points.at(o + 1).rotation -= 3.14;
            } else {
                points.at(o + 1).rotation += 3.14;
            }
        }
    }


    x = -100;
    i = 0;


    fprintf(PlikVRML, "Viewpoint {\n");
    fprintf(PlikVRML, "position 0 80 130\n");
    fprintf(PlikVRML, "orientation 1 0 0 -0.7\n");
    fprintf(PlikVRML, "description \"nr_%d\"}\n", i);


    char drzewo[] = "model1";
    char drzewo2[] = "model2";
    char drzewo3[] = "model3";
    char drzewo4[] = "model4";

    for (i = 0; i < N1; i++) {
        spawnOutsideBlockedArea(PlikVRML, drzewo, blockedAreas);
    }

    for (i = 0; i < N2; i++) {
        spawnOutsideBlockedArea(PlikVRML, drzewo2, blockedAreas);

    }

    for (i = 0; i < N3; i++) {
        spawnOutsideBlockedArea(PlikVRML, drzewo3, blockedAreas);

    }

    for (i = 0; i < N4; i++) {
        spawnOutsideBlockedArea(PlikVRML, drzewo4, blockedAreas, 8.0f);
    }

    fprintf(PlikVRML, "DEF X TimeSensor {loop TRUE cycleInterval 20}\n");


    fprintf(PlikVRML, "DEF AUTKOP PositionInterpolator {\n");
    fprintf(PlikVRML, "key [0,1]\n");
    fprintf(PlikVRML, "keyValue [-100, 0 -17, 100 0 -17]}\n");
    fprintf(PlikVRML, "Transform {\n");
    fprintf(PlikVRML, "translation 0 0.9 0\n");
    fprintf(PlikVRML, "rotation 0 1 0 1.57\n");
    fprintf(PlikVRML, "children [\n");
    fprintf(PlikVRML, "DEF AUTKO Transform {\n");
    fprintf(PlikVRML, "children [\n");

    fprintf(PlikVRML, "Inline { url \"autko.wrl\"}]}]}\n");


    fprintf(PlikVRML, "ROUTE X.fraction_changed TO AUTKOP.set_fraction\n");
    fprintf(PlikVRML, "ROUTE AUTKOP.value_changed TO AUTKO.set_translation\n");

    createWagon(PlikVRML, 0, points, CYSTERNA);
    createWagon(PlikVRML, 1, points, CYSTERNA);
    createWagon(PlikVRML, 2, points, CYSTERNA);
    createWagon(PlikVRML, 3, points, WAGUN);
    createWagon(PlikVRML, 4, points, WAGUN);
    createWagon(PlikVRML, 5, points, CYSTERNA);
    createWagon(PlikVRML, 6, points, WAGUN);
    createWagon(PlikVRML, 7, points, WAGUN);
    createWagon(PlikVRML, 8, points, CIUCHCIA);

    char object[] = "domek";
    createObject(PlikVRML, object, 50, 0, 10, 0);

    char krowa[] = "cow";
    createObject(PlikVRML, krowa, -50, 0, 54, 3.14);
    createObject(PlikVRML, krowa, -48, 0, 62, 1.3);
    createObject(PlikVRML, krowa, -57, 0, 54, -0.4);
    createObject(PlikVRML, krowa, -52, 0, 61, 0.5);
    createObject(PlikVRML, krowa, -51, 0, 58, 1.2);

    fclose(PlikVRML);

    return 0;
}
