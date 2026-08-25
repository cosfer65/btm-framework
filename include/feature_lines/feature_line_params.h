#pragma once

namespace btm {
    template <typename T>
    struct FeatureLineParameters
    {
        // --- Curvature-based feature lines ---
        T ridge_threshold = 0.005;   // |k1| > threshold
        T valley_threshold = 0.005;   // |k1| > threshold

        // Minimum length (in number of vertices) to keep a line
        int minLineSize = 3;

        // Maximum number of steps when tracing a ridge/valley
        int maxTraceSteps = 200;

        // --- Dihedral-based creases ---
        T dihedralAngleThreshold = 10.0;  // degrees

        // Optional smoothing of traced lines
        bool smoothLines = false;
        int  smoothingIterations = 3;
    };
}
