// NOTE: Place in djinn/src/demos/ and adjust CMakeLists.txt accordingly

#include "djinn/numerical.h"
#include "math.h"
#include <cmath>
#include <iostream>

// Generic function (f') to integrate via RK4
djinn::Vec3 func(djinn::Vec3 pos, djinn::real t) {
    pos.x = sin(pos.x);
    pos.y = sin(pos.y);
    pos.z = sin(pos.z);

    return pos;
}

int main() {
    // Define initial conditions, timesteps, etc.
    djinn::Vec3 pos = djinn::Vec3(1.0, 1.1, 1.2);
    const djinn::real t0 = 0.0;
    const djinn::real dt = 0.1;
    const djinn::real tf = 20.0;

    djinn::Vec3 LTE;

    // Integrate
    for (djinn::real t = t0; t <= tf; t += dt) {
        // Output data (can be piped to a file for analysis)
        std::cout << t << "," << pos.x << "," << pos.y << "," << pos.z << std::endl;

        // Update position according to RK4
        pos = djinn::rungeKutta4(func, pos, t, dt);
    }

    return 0;
}
