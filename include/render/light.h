#ifndef __light_h__
#define __light_h__

#include "vector.h"
#include "shaders.h"

using namespace btm;

namespace btm {
    class gl_light {
    protected:
        fvec3 position;
        fvec3 direction;
        fvec3 color;
    public:
        gl_light()
            : position(fvec3(-20, 20, 50)),
              direction(fvec3(1, 1, 1).normalize()),
              color(fvec3(1.0f)) {
        }
        ~gl_light() {
        }

        void set_color(const fvec3& a) {
            color = a;
        }

        fvec3 get_color() {
            return color;
        }

        void set_direction(const fvec3& d) {
            direction = d;
            direction.normalize();
        }

        fvec3 get_direction() {
            return direction;
        }

        void set_position(const fvec3& p) {
            position = p;
        }

        fvec3 get_position() {
            return position;
        }

        void apply(gl_shader* shdr) {
            shdr->set_vec3("light_position", position);
            shdr->set_vec3("light_direction", direction);
            shdr->set_vec3("light_color", color);
        }
    };
}
#endif // __light_h__
