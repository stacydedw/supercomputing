#include "minirt/minirt.h"
#include <iostream>
#include <cmath>
#include <string>
#include <omp.h>

using namespace minirt;

void initScene(Scene &scene) {
    Color red {1, 0.2, 0.2};
    Color blue {0.2, 0.2, 1};
    Color green {0.2, 1, 0.2};
    Color white {0.8, 0.8, 0.8};
    Color yellow {1, 1, 0.2};

    Material metallicRed {red, white, 50};
    Material mirrorBlack {Color {0.0}, Color {0.9}, 1000};
    Material matteWhite {Color {0.7}, Color {0.3}, 1};
    Material metallicYellow {yellow, white, 250};

    Material transparentGreen {green, 0.8, 0.2};
    transparentGreen.makeTransparent(1.0, 1.03);
    Material transparentBlue {blue, 0.4, 0.6};
    transparentBlue.makeTransparent(0.9, 0.7);

    scene.addSphere(Sphere {{0, -2, 7}, 1, transparentBlue});
    scene.addSphere(Sphere {{-3, 2, 11}, 2, metallicRed});
    scene.addSphere(Sphere {{0, 2, 8}, 1, mirrorBlack});
    scene.addSphere(Sphere {{1.5, -0.5, 7}, 1, transparentGreen});
    scene.addSphere(Sphere {{-2, -1, 6}, 0.7, metallicYellow});
    scene.addSphere(Sphere {{2.2, 0.5, 9}, 1.2, matteWhite});
    scene.addSphere(Sphere {{4, -1, 10}, 0.7, metallicRed});

    scene.addLight(PointLight {{-15, 0, -15}, white});
    scene.addLight(PointLight {{1, 1, 0}, blue});
    scene.addLight(PointLight {{0, -10, 6}, red});

    scene.setBackground({0.05, 0.05, 0.08});
    scene.setAmbient({0.1, 0.1, 0.1});
    scene.setRecursionLimit(20);

    scene.setCamera(Camera {{0, 0, -20}, {0, 0, 0}});
}

int main(int argc, char **argv) {
    const int width = (argc > 1 ? std::stoi(argv[1]) : 600);
    const int height = (argc > 2 ? std::stoi(argv[2]) : 600);
    const int samples = (argc > 3 ? std::stoi(argv[3]) : 1);
    const std::string sceneFile = (argc > 4 ? argv[4] : "-");
    const std::string mode = (argc > 5 ? argv[5] : "plain");

    Scene scene;
    if (sceneFile == "-") {
        initScene(scene);
    } else {
        scene.loadFromFile(sceneFile);
    }


    const double backgroundSizeX = 4;
    const double backgroundSizeY = 4;
    const double backgroundDistance = 15;
    const double viewPlaneDistance = 5;
    const double viewPlaneSizeX = backgroundSizeX * viewPlaneDistance / backgroundDistance;
    const double viewPlaneSizeY = backgroundSizeY * viewPlaneDistance / backgroundDistance;

    ViewPlane viewPlane {width, height, viewPlaneSizeX, viewPlaneSizeY, viewPlaneDistance};
    Image image(width, height);

    double start = omp_get_wtime();
    if (mode == "collapse") {
        #pragma omp parallel for collapse(2) schedule(runtime)
        for (int x = 0; x < width; x++)
        for (int y = 0; y < height; y++) {
            const auto color = viewPlane.computePixel(scene, x, y, samples);
            image.set(x, y, color);
        }
    } else {
        #pragma omp parallel for schedule(runtime)
        for (int x = 0; x < width; x++)
        for (int y = 0; y < height; y++) {
            const auto color = viewPlane.computePixel(scene, x, y, samples);
            image.set(x, y, color);
        }
    }
    double end = omp_get_wtime();
    double execution_time = end - start;
    std::cout << "Time = " << execution_time << std::endl;

    image.saveJPEG("raytracing.jpg");
    return 0;
}