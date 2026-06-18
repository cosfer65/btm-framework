#ifndef __line_h__
#define __line_h__

#include "vector.h"

// Line, ray, and segment structures and related functions

namespace btm {
    template <typename T>
    struct line {
        basepoint3<T> origin;
        basevec3<T>   direction; // not necessarily normalized
    };

    template <typename T>
    basepoint3<T> evaluate(const line<T>& l, float t) {
        return l.origin + l.direction * t;
    }

    template <typename T>
    struct ray {
        basepoint3<T> origin;
        basevec3<T>   direction; // assumed normalized for many uses
    };

    template <typename T>
    basepoint3<T> evaluate(const ray<T>& r, float t) {
        return r.origin + r.direction * t;
    }

    template <typename T>
    struct segment {
        basepoint3<T> a;
        basepoint3<T> b;
    };

    template <typename T>
    float length(const segment<T>& s) {
        return distance(s.a, s.b);
    }

    template <typename T>
    float lengthSquared(const segment<T>& s) {
        return distanceSquared(s.a, s.b);
    }

    template <typename T>
    basepoint3<T> evaluate(const segment<T>& s, float t) {
        // t in [0,1]
        basevec3<T> ab = s.b - s.a;
        return s.a + ab * t;
    }
}

#endif // !__line_h__
