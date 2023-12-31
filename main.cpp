/**
@file main.cpp
*/

#include "image.h"
#include "lights/light.h"
#include "loaders/loader.h"
#include "material.h"
#include "objects/mesh.h"
#include "objects/object.h"
#include "objects/plane.h"
#include "ray.h"
#include "raytracer.h"
#include "scene.h"
#include "textures.h"

#include "tracers/kdtree.h"

#include "animation.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/glm.hpp"
#include "lights/point.h"
#include "lights/surface.h"
#include "loaders/obj-loader.h"
#include "objects/cone.h"
#include "objects/square.h"
#include <cmath>
#include <ctime>
#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <vector>

#define VOID_COLOR_RGB 0, 0, 0
#define SCENE_Z 1.0f
#define MAX_RAY_DEPTH 10

using namespace std;


static Scene scene;


/**
 Function defining the scene
 */
void sceneDefinition(SceneBuilder &builder)
{
    Mesh *const bunny = OBJMeshLoader().load("../../meshes/bunny_small.obj", MaterialFactory().build());
    bunny->transform(glm::translate(glm::mat4(1.0f), glm::vec3(0, -3, 8)));
    bunny->initializeTracer();
    builder.addObject(bunny);

    builder.addObject(new Plane(glm::vec3(0, -3, 0), glm::vec3(0, 1, 0)));
    builder.addObject(new Plane(glm::vec3(0, 27, 0), glm::vec3(0, -1, 0)));
    builder.addObject(new Plane(glm::vec3(-15, 0, 0), glm::vec3(1, 0, 0), Material()));
    builder.addObject(new Plane(glm::vec3(15, 0, 0), glm::vec3(-1, 0, 0), Material()));
    builder.addObject(new Plane(glm::vec3(0, 0, 30), glm::vec3(0, 0, -1)));
    builder.addObject(new Plane(glm::vec3(0, 0, -0.01), glm::vec3(0, 0, 1), Material()));

    //    auto lightSphere = new Sphere(MaterialFactory().build());
    //    lightSphere->transform(glm::translate(glm::mat4(1.0f), glm::vec3(0, 8, 16)));
    //    builder.addLightObject(lightSphere, glm::vec3(1.0f, 0.0f, 0.0f));
    const float squarez = 14.5f;
    auto square_obj = new Square({squarez, 4, 12}, {squarez, 6, 12}, {squarez, 6, 18}, {squarez, 4, 18},
                                 MaterialFactory().build());
    builder.addLightObject(square_obj, glm::vec3(0.1f, 0.1f, 0.0f));

    // ========= LIGHTS =========
//    builder.addLight(new PointLight(glm::vec3(0, 26, 5), glm::vec3(1.0f)));
    //    builder.addLight(new PointLight(glm::vec3(0, 1, 12), glm::vec3(0.1f)));
//        builder.addLight(new PointLight(glm::vec3(0, 5, 1), glm::vec3(0.4f)));
}

int main(int argc, const char *argv[])
{
    Raytracer tracer = Raytracer(1024, 768, 90).setAntiAliasing(1);

    // Compute the size of each pixel given the FOV
    scene.setup<NaiveTracer>(sceneDefinition);

    if (argc >= 2) {
        tracer.setOutputFile(argv[1]);
    }

    tracer.render(scene);

    return 0;
}
