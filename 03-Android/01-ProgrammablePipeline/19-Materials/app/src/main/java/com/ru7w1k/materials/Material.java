package com.ru7w1k.materials;

public class Material {
    public float[] MaterialAmbient;
    public float[] MaterialDiffuse;
    public float[] MaterialSpecular;
    public float MaterialShininess;

    public Material() {
        MaterialAmbient  = new float[4];
        MaterialDiffuse  = new float[4];
        MaterialSpecular = new float[4];
        MaterialShininess = 0.0f;
    }

    public static Material[][] InitMaterials() {
        Material[][] Materials = new Material[6][4];

        for(int i = 0; i < 6; i++) {
            for(int j = 0; j < 4; j++) {
                Materials[i][j] = new Material();
            }
        }

        // ***** 1st sphere on 1st column, emerald *****
        // ambient material
        Materials[0][0].MaterialAmbient[0] = 0.0215f; // r
        Materials[0][0].MaterialAmbient[1] = 0.1745f; // g
        Materials[0][0].MaterialAmbient[2] = 0.0215f; // b
        Materials[0][0].MaterialAmbient[3] = 1.0f;   // a

        // diffuse material
        Materials[0][0].MaterialDiffuse[0] = 0.07568f; // r
        Materials[0][0].MaterialDiffuse[1] = 0.61424f; // g
        Materials[0][0].MaterialDiffuse[2] = 0.07568f; // b
        Materials[0][0].MaterialDiffuse[3] = 1.0f;    // a

        // specular material
        Materials[0][0].MaterialSpecular[0] = 0.633f;    // r
        Materials[0][0].MaterialSpecular[1] = 0.727811f; // g
        Materials[0][0].MaterialSpecular[2] = 0.633f;    // b
        Materials[0][0].MaterialSpecular[3] = 1.0f;     // a

        // shininess
        Materials[0][0].MaterialShininess = 0.6f * 128.0f;

        // ***** 2nd sphere on 1st column, jade *****
        // ambient material
        Materials[1][0].MaterialAmbient[0] = 0.135f;  // r
        Materials[1][0].MaterialAmbient[1] = 0.2225f; // g
        Materials[1][0].MaterialAmbient[2] = 0.1575f; // b
        Materials[1][0].MaterialAmbient[3] = 1.0f;   // a

        // diffuse material
        Materials[1][0].MaterialDiffuse[0] = 0.54f; // r
        Materials[1][0].MaterialDiffuse[1] = 0.89f; // g
        Materials[1][0].MaterialDiffuse[2] = 0.63f; // b
        Materials[1][0].MaterialDiffuse[3] = 1.0f; // a

        // specular material
        Materials[1][0].MaterialSpecular[0] = 0.316228f; // r
        Materials[1][0].MaterialSpecular[1] = 0.316228f; // g
        Materials[1][0].MaterialSpecular[2] = 0.316228f; // b
        Materials[1][0].MaterialSpecular[3] = 1.0f;     // a

         // shininess
        Materials[1][0].MaterialShininess = 0.1f * 128.0f;

        // ***** 3rd sphere on 1st column, obsidian *****
        // ambient material
        Materials[2][0].MaterialAmbient[0] = 0.05375f; // r
        Materials[2][0].MaterialAmbient[1] = 0.05f;    // g
        Materials[2][0].MaterialAmbient[2] = 0.06625f; // b
        Materials[2][0].MaterialAmbient[3] = 1.0f;    // a

        // diffuse material
        Materials[2][0].MaterialDiffuse[0] = 0.18275f; // r
        Materials[2][0].MaterialDiffuse[1] = 0.17f;    // g
        Materials[2][0].MaterialDiffuse[2] = 0.22525f; // b
        Materials[2][0].MaterialDiffuse[3] = 1.0f;    // a

        // specular material
        Materials[2][0].MaterialSpecular[0] = 0.332741f; // r
        Materials[2][0].MaterialSpecular[1] = 0.328634f; // g
        Materials[2][0].MaterialSpecular[2] = 0.346435f; // b
        Materials[2][0].MaterialSpecular[3] = 1.0f;     // a

        // shininess
        Materials[2][0].MaterialShininess = 0.3f * 128.0f;

        // ***** 4th sphere on 1st column, pearl *****
        // ambient material
        Materials[3][0].MaterialAmbient[0] = 0.25f;    // r
        Materials[3][0].MaterialAmbient[1] = 0.20725f; // g
        Materials[3][0].MaterialAmbient[2] = 0.20725f; // b
        Materials[3][0].MaterialAmbient[3] = 1.0f;    // a

        // diffuse material
        Materials[3][0].MaterialDiffuse[0] = 1.0f;   // r
        Materials[3][0].MaterialDiffuse[1] = 0.829f; // g
        Materials[3][0].MaterialDiffuse[2] = 0.829f; // b
        Materials[3][0].MaterialDiffuse[3] = 1.0f;  // a

        // specular material
        Materials[3][0].MaterialSpecular[0] = 0.296648f; // r
        Materials[3][0].MaterialSpecular[1] = 0.296648f; // g
        Materials[3][0].MaterialSpecular[2] = 0.296648f; // b
        Materials[3][0].MaterialSpecular[3] = 1.0f;     // a

        // shininess
        Materials[3][0].MaterialShininess = 0.088f * 128.0f;

        // ***** 5th sphere on 1st column, ruby *****
        // ambient material
        Materials[4][0].MaterialAmbient[0] = 0.1745f;  // r
        Materials[4][0].MaterialAmbient[1] = 0.01175f; // g
        Materials[4][0].MaterialAmbient[2] = 0.01175f; // b
        Materials[4][0].MaterialAmbient[3] = 1.0f;    // a


        // diffuse material
        Materials[4][0].MaterialDiffuse[0] = 0.61424f; // r
        Materials[4][0].MaterialDiffuse[1] = 0.04136f; // g
        Materials[4][0].MaterialDiffuse[2] = 0.04136f; // b
        Materials[4][0].MaterialDiffuse[3] = 1.0f;    // a


        // specular material
        Materials[4][0].MaterialSpecular[0] = 0.727811f; // r
        Materials[4][0].MaterialSpecular[1] = 0.626959f; // g
        Materials[4][0].MaterialSpecular[2] = 0.626959f; // b
        Materials[4][0].MaterialSpecular[3] = 1.0f;     // a

        // shininess
        Materials[4][0].MaterialShininess = 0.6f * 128.0f;

        // ***** 6th sphere on 1st column, turquoise *****
        // ambient material
        Materials[5][0].MaterialAmbient[0] = 0.1f;     // r
        Materials[5][0].MaterialAmbient[1] = 0.18725f; // g
        Materials[5][0].MaterialAmbient[2] = 0.1745f;  // b
        Materials[5][0].MaterialAmbient[3] = 1.0f;    // a


        // diffuse material
        Materials[5][0].MaterialDiffuse[0] = 0.396f;   // r
        Materials[5][0].MaterialDiffuse[1] = 0.74151f; // g
        Materials[5][0].MaterialDiffuse[2] = 0.69102f; // b
        Materials[5][0].MaterialDiffuse[3] = 1.0f;    // a


        // specular material
        Materials[5][0].MaterialSpecular[0] = 0.297254f; // r
        Materials[5][0].MaterialSpecular[1] = 0.30829f;  // g
        Materials[5][0].MaterialSpecular[2] = 0.306678f; // b
        Materials[5][0].MaterialSpecular[3] = 1.0f;     // a

        // shininess
        Materials[5][0].MaterialShininess = 0.1f * 128.0f;

        // ***** 1st sphere on 2nd column, brass *****
        // ambient material
        Materials[0][1].MaterialAmbient[0] = 0.329412f; // r
        Materials[0][1].MaterialAmbient[1] = 0.223529f; // g
        Materials[0][1].MaterialAmbient[2] = 0.027451f; // b
        Materials[0][1].MaterialAmbient[3] = 1.0f;     // a


        // diffuse material
        Materials[0][1].MaterialDiffuse[0] = 0.780392f; // r
        Materials[0][1].MaterialDiffuse[1] = 0.568627f; // g
        Materials[0][1].MaterialDiffuse[2] = 0.113725f; // b
        Materials[0][1].MaterialDiffuse[3] = 1.0f;     // a


        // specular material
        Materials[0][1].MaterialSpecular[0] = 0.992157f; // r
        Materials[0][1].MaterialSpecular[1] = 0.941176f; // g
        Materials[0][1].MaterialSpecular[2] = 0.807843f; // b
        Materials[0][1].MaterialSpecular[3] = 1.0f;     // a

        // shininess
        Materials[0][1].MaterialShininess = 0.21794872f * 128.0f;

        // ***** 2nd sphere on 2nd column, bronze *****
        // ambient material
        Materials[1][1].MaterialAmbient[0] = 0.2125f; // r
        Materials[1][1].MaterialAmbient[1] = 0.1275f; // g
        Materials[1][1].MaterialAmbient[2] = 0.054f;  // b
        Materials[1][1].MaterialAmbient[3] = 1.0f;   // a


        // diffuse material
        Materials[1][1].MaterialDiffuse[0] = 0.714f;   // r
        Materials[1][1].MaterialDiffuse[1] = 0.4284f;  // g
        Materials[1][1].MaterialDiffuse[2] = 0.18144f; // b
        Materials[1][1].MaterialDiffuse[3] = 1.0f;    // a


        // specular material
        Materials[1][1].MaterialSpecular[0] = 0.393548f; // r
        Materials[1][1].MaterialSpecular[1] = 0.271906f; // g
        Materials[1][1].MaterialSpecular[2] = 0.166721f; // b
        Materials[1][1].MaterialSpecular[3] = 1.0f;     // a

        // shininess
        Materials[1][1].MaterialShininess = 0.2f * 128.0f;

        // ***** 3rd sphere on 2nd column, chrome *****
        // ambient material
        Materials[2][1].MaterialAmbient[0] = 0.25f; // r
        Materials[2][1].MaterialAmbient[1] = 0.25f; // g
        Materials[2][1].MaterialAmbient[2] = 0.25f; // b
        Materials[2][1].MaterialAmbient[3] = 1.0f; // a


        // diffuse material
        Materials[2][1].MaterialDiffuse[0] = 0.4f;  // r
        Materials[2][1].MaterialDiffuse[1] = 0.4f;  // g
        Materials[2][1].MaterialDiffuse[2] = 0.4f;  // b
        Materials[2][1].MaterialDiffuse[3] = 1.0f; // a


        // specular material
        Materials[2][1].MaterialSpecular[0] = 0.774597f; // r
        Materials[2][1].MaterialSpecular[1] = 0.774597f; // g
        Materials[2][1].MaterialSpecular[2] = 0.774597f; // b
        Materials[2][1].MaterialSpecular[3] = 1.0f;     // a

        // shininess
        Materials[2][1].MaterialShininess = 0.6f * 128.0f;

        // ***** 4th sphere on 2nd column, copper *****
        // ambient material
        Materials[3][1].MaterialAmbient[0] = 0.19125f; // r
        Materials[3][1].MaterialAmbient[1] = 0.0735f;  // g
        Materials[3][1].MaterialAmbient[2] = 0.0225f;  // b
        Materials[3][1].MaterialAmbient[3] = 1.0f;    // a


        // diffuse material
        Materials[3][1].MaterialDiffuse[0] = 0.7038f;  // r
        Materials[3][1].MaterialDiffuse[1] = 0.27048f; // g
        Materials[3][1].MaterialDiffuse[2] = 0.0828f;  // b
        Materials[3][1].MaterialDiffuse[3] = 1.0f;    // a


        // specular material
        Materials[3][1].MaterialSpecular[0] = 0.256777f; // r
        Materials[3][1].MaterialSpecular[1] = 0.137622f; // g
        Materials[3][1].MaterialSpecular[2] = 0.086014f; // b
        Materials[3][1].MaterialSpecular[3] = 1.0f;     // a

        // shininess
        Materials[3][1].MaterialShininess = 0.1f * 128.0f;

        // ***** 5th sphere on 2nd column, gold *****
        // ambient material
        Materials[4][1].MaterialAmbient[0] = 0.24725f; // r
        Materials[4][1].MaterialAmbient[1] = 0.1995f;  // g
        Materials[4][1].MaterialAmbient[2] = 0.0745f;  // b
        Materials[4][1].MaterialAmbient[3] = 1.0f;    // a


        // diffuse material
        Materials[4][1].MaterialDiffuse[0] = 0.75164f; // r
        Materials[4][1].MaterialDiffuse[1] = 0.60648f; // g
        Materials[4][1].MaterialDiffuse[2] = 0.22648f; // b
        Materials[4][1].MaterialDiffuse[3] = 1.0f;    // a


        // specular material
        Materials[4][1].MaterialSpecular[0] = 0.628281f; // r
        Materials[4][1].MaterialSpecular[1] = 0.555802f; // g
        Materials[4][1].MaterialSpecular[2] = 0.366065f; // b
        Materials[4][1].MaterialSpecular[3] = 1.0f;     // a

        // shininess
        Materials[4][1].MaterialShininess = 0.4f * 128.0f;


        // ***** 6th sphere on 2nd column, silver *****
        // ambient material
        Materials[5][1].MaterialAmbient[0] = 0.19225f; // r
        Materials[5][1].MaterialAmbient[1] = 0.19225f; // g
        Materials[5][1].MaterialAmbient[2] = 0.19225f; // b
        Materials[5][1].MaterialAmbient[3] = 1.0f;    // a


        // diffuse material
        Materials[5][1].MaterialDiffuse[0] = 0.50754f; // r
        Materials[5][1].MaterialDiffuse[1] = 0.50754f; // g
        Materials[5][1].MaterialDiffuse[2] = 0.50754f; // b
        Materials[5][1].MaterialDiffuse[3] = 1.0f;    // a


        // specular material
        Materials[5][1].MaterialSpecular[0] = 0.508273f; // r
        Materials[5][1].MaterialSpecular[1] = 0.508273f; // g
        Materials[5][1].MaterialSpecular[2] = 0.508273f; // b
        Materials[5][1].MaterialSpecular[3] = 1.0f;     // a

        // shininess
        Materials[5][1].MaterialShininess = 0.4f * 128.0f;


        // ***** 1st sphere on 3rd column, black *****
        // ambient material
        Materials[0][2].MaterialAmbient[0] = 0.0f;  // r
        Materials[0][2].MaterialAmbient[1] = 0.0f;  // g
        Materials[0][2].MaterialAmbient[2] = 0.0f;  // b
        Materials[0][2].MaterialAmbient[3] = 1.0f; // a


        // diffuse material
        Materials[0][2].MaterialDiffuse[0] = 0.01f; // r
        Materials[0][2].MaterialDiffuse[1] = 0.01f; // g
        Materials[0][2].MaterialDiffuse[2] = 0.01f; // b
        Materials[0][2].MaterialDiffuse[3] = 1.0f; // a


        // specular material
        Materials[0][2].MaterialSpecular[0] = 0.50f; // r
        Materials[0][2].MaterialSpecular[1] = 0.50f; // g
        Materials[0][2].MaterialSpecular[2] = 0.50f; // b
        Materials[0][2].MaterialSpecular[3] = 1.0f; // a

        // shininess
        Materials[0][2].MaterialShininess = 0.25f * 128.0f;


        // ***** 2nd sphere on 3rd column, cyan *****
        // ambient material
        Materials[1][2].MaterialAmbient[0] = 0.0f;  // r
        Materials[1][2].MaterialAmbient[1] = 0.1f;  // g
        Materials[1][2].MaterialAmbient[2] = 0.06f; // b
        Materials[1][2].MaterialAmbient[3] = 1.0f; // a


        // diffuse material
        Materials[1][2].MaterialDiffuse[0] = 0.0f;        // r
        Materials[1][2].MaterialDiffuse[1] = 0.50980392f; // g
        Materials[1][2].MaterialDiffuse[2] = 0.50980392f; // b
        Materials[1][2].MaterialDiffuse[3] = 1.0f;       // a


        // specular material
        Materials[1][2].MaterialSpecular[0] = 0.50196078f; // r
        Materials[1][2].MaterialSpecular[1] = 0.50196078f; // g
        Materials[1][2].MaterialSpecular[2] = 0.50196078f; // b
        Materials[1][2].MaterialSpecular[3] = 1.0f;       // a

        // shininess
        Materials[1][2].MaterialShininess = 0.25f * 128.0f;

        // ***** 3rd sphere on 2nd column, green *****
        // ambient material
        Materials[2][2].MaterialAmbient[0] = 0.0f;  // r
        Materials[2][2].MaterialAmbient[1] = 0.0f;  // g
        Materials[2][2].MaterialAmbient[2] = 0.0f;  // b
        Materials[2][2].MaterialAmbient[3] = 1.0f; // a


        // diffuse material
        Materials[2][2].MaterialDiffuse[0] = 0.1f;  // r
        Materials[2][2].MaterialDiffuse[1] = 0.35f; // g
        Materials[2][2].MaterialDiffuse[2] = 0.1f;  // b
        Materials[2][2].MaterialDiffuse[3] = 1.0f; // a


        // specular material
        Materials[2][2].MaterialSpecular[0] = 0.45f; // r
        Materials[2][2].MaterialSpecular[1] = 0.55f; // g
        Materials[2][2].MaterialSpecular[2] = 0.45f; // b
        Materials[2][2].MaterialSpecular[3] = 1.0f; // a

        // shininess
        Materials[2][2].MaterialShininess = 0.25f * 128.0f;

        // ***** 4th sphere on 3rd column, red *****
        // ambient material
        Materials[3][2].MaterialAmbient[0] = 0.0f;  // r
        Materials[3][2].MaterialAmbient[1] = 0.0f;  // g
        Materials[3][2].MaterialAmbient[2] = 0.0f;  // b
        Materials[3][2].MaterialAmbient[3] = 1.0f; // a


        // diffuse material
        Materials[3][2].MaterialDiffuse[0] = 0.5f;  // r
        Materials[3][2].MaterialDiffuse[1] = 0.0f;  // g
        Materials[3][2].MaterialDiffuse[2] = 0.0f;  // b
        Materials[3][2].MaterialDiffuse[3] = 1.0f; // a


        // specular material
        Materials[3][2].MaterialSpecular[0] = 0.7f;  // r
        Materials[3][2].MaterialSpecular[1] = 0.6f;  // g
        Materials[3][2].MaterialSpecular[2] = 0.6f;  // b
        Materials[3][2].MaterialSpecular[3] = 1.0f; // a

        // shininess
        Materials[3][2].MaterialShininess = 0.25f * 128.0f;

        // ***** 5th sphere on 3rd column, white *****
        // ambient material
        Materials[4][2].MaterialAmbient[0] = 0.0f;  // r
        Materials[4][2].MaterialAmbient[1] = 0.0f;  // g
        Materials[4][2].MaterialAmbient[2] = 0.0f;  // b
        Materials[4][2].MaterialAmbient[3] = 1.0f; // a


        // diffuse material
        Materials[4][2].MaterialDiffuse[0] = 0.55f; // r
        Materials[4][2].MaterialDiffuse[1] = 0.55f; // g
        Materials[4][2].MaterialDiffuse[2] = 0.55f; // b
        Materials[4][2].MaterialDiffuse[3] = 1.0f; // a


        // specular material
        Materials[4][2].MaterialSpecular[0] = 0.70f; // r
        Materials[4][2].MaterialSpecular[1] = 0.70f; // g
        Materials[4][2].MaterialSpecular[2] = 0.70f; // b
        Materials[4][2].MaterialSpecular[3] = 1.0f; // a

        // shininess
        Materials[4][2].MaterialShininess = 0.25f * 128.0f;

        // ***** 6th sphere on 3rd column, yellow plastic *****
        // ambient material
        Materials[5][2].MaterialAmbient[0] = 0.0f;  // r
        Materials[5][2].MaterialAmbient[1] = 0.0f;  // g
        Materials[5][2].MaterialAmbient[2] = 0.0f;  // b
        Materials[5][2].MaterialAmbient[3] = 1.0f; // a


        // diffuse material
        Materials[5][2].MaterialDiffuse[0] = 0.5f;  // r
        Materials[5][2].MaterialDiffuse[1] = 0.5f;  // g
        Materials[5][2].MaterialDiffuse[2] = 0.0f;  // b
        Materials[5][2].MaterialDiffuse[3] = 1.0f; // a


        // specular material
        Materials[5][2].MaterialSpecular[0] = 0.60f; // r
        Materials[5][2].MaterialSpecular[1] = 0.60f; // g
        Materials[5][2].MaterialSpecular[2] = 0.50f; // b
        Materials[5][2].MaterialSpecular[3] = 1.0f; // a

        // shininess
        Materials[5][2].MaterialShininess = 0.25f * 128.0f;

        // ***** 1st sphere on 4th column, black *****
        // ambient material
        Materials[0][3].MaterialAmbient[0] = 0.02f; // r
        Materials[0][3].MaterialAmbient[1] = 0.02f; // g
        Materials[0][3].MaterialAmbient[2] = 0.02f; // b
        Materials[0][3].MaterialAmbient[3] = 1.0f; // a


        // diffuse material
        Materials[0][3].MaterialDiffuse[0] = 0.01f; // r
        Materials[0][3].MaterialDiffuse[1] = 0.01f; // g
        Materials[0][3].MaterialDiffuse[2] = 0.01f; // b
        Materials[0][3].MaterialDiffuse[3] = 1.0f; // a


        // specular material
        Materials[0][3].MaterialSpecular[0] = 0.4f;  // r
        Materials[0][3].MaterialSpecular[1] = 0.4f;  // g
        Materials[0][3].MaterialSpecular[2] = 0.4f;  // b
        Materials[0][3].MaterialSpecular[3] = 1.0f; // a

        // shininess
        Materials[0][3].MaterialShininess = 0.078125f * 128.0f;

        // ***** 2nd sphere on 4th column, cyan *****
        // ambient material
        Materials[1][3].MaterialAmbient[0] = 0.0f;  // r
        Materials[1][3].MaterialAmbient[1] = 0.05f; // g
        Materials[1][3].MaterialAmbient[2] = 0.05f; // b
        Materials[1][3].MaterialAmbient[3] = 1.0f; // a


        // diffuse material
        Materials[1][3].MaterialDiffuse[0] = 0.4f;  // r
        Materials[1][3].MaterialDiffuse[1] = 0.5f;  // g
        Materials[1][3].MaterialDiffuse[2] = 0.5f;  // b
        Materials[1][3].MaterialDiffuse[3] = 1.0f; // a


        // specular material
        Materials[1][3].MaterialSpecular[0] = 0.04f; // r
        Materials[1][3].MaterialSpecular[1] = 0.7f;  // g
        Materials[1][3].MaterialSpecular[2] = 0.7f;  // b
        Materials[1][3].MaterialSpecular[3] = 1.0f; // a

        // shininess
        Materials[1][3].MaterialShininess = 0.078125f * 128.0f;


        // ***** 3rd sphere on 4th column, green *****
        // ambient material
        Materials[2][3].MaterialAmbient[0] = 0.0f;  // r
        Materials[2][3].MaterialAmbient[1] = 0.05f; // g
        Materials[2][3].MaterialAmbient[2] = 0.0f;  // b
        Materials[2][3].MaterialAmbient[3] = 1.0f; // a


        // diffuse material
        Materials[2][3].MaterialDiffuse[0] = 0.4f;  // r
        Materials[2][3].MaterialDiffuse[1] = 0.5f;  // g
        Materials[2][3].MaterialDiffuse[2] = 0.4f;  // b
        Materials[2][3].MaterialDiffuse[3] = 1.0f; // a


        // specular material
        Materials[2][3].MaterialSpecular[0] = 0.04f; // r
        Materials[2][3].MaterialSpecular[1] = 0.7f;  // g
        Materials[2][3].MaterialSpecular[2] = 0.04f; // b
        Materials[2][3].MaterialSpecular[3] = 1.0f; // a

        // shininess
        Materials[2][3].MaterialShininess = 0.078125f * 128.0f;

        // ***** 4th sphere on 4th column, red *****
        // ambient material
        Materials[3][3].MaterialAmbient[0] = 0.05f; // r
        Materials[3][3].MaterialAmbient[1] = 0.0f;  // g
        Materials[3][3].MaterialAmbient[2] = 0.0f;  // b
        Materials[3][3].MaterialAmbient[3] = 1.0f; // a


        // diffuse material
        Materials[3][3].MaterialDiffuse[0] = 0.5f;  // r
        Materials[3][3].MaterialDiffuse[1] = 0.4f;  // g
        Materials[3][3].MaterialDiffuse[2] = 0.4f;  // b
        Materials[3][3].MaterialDiffuse[3] = 1.0f; // a


        // specular material
        Materials[3][3].MaterialSpecular[0] = 0.7f;  // r
        Materials[3][3].MaterialSpecular[1] = 0.04f; // g
        Materials[3][3].MaterialSpecular[2] = 0.04f; // b
        Materials[3][3].MaterialSpecular[3] = 1.0f; // a

        // shininess
        Materials[3][3].MaterialShininess = 0.078125f * 128.0f;


        // ***** 5th sphere on 4th column, white *****
        // ambient material
        Materials[4][3].MaterialAmbient[0] = 0.05f; // r
        Materials[4][3].MaterialAmbient[1] = 0.05f; // g
        Materials[4][3].MaterialAmbient[2] = 0.05f; // b
        Materials[4][3].MaterialAmbient[3] = 1.0f; // a


        // diffuse material
        Materials[4][3].MaterialDiffuse[0] = 0.5f;  // r
        Materials[4][3].MaterialDiffuse[1] = 0.5f;  // g
        Materials[4][3].MaterialDiffuse[2] = 0.5f;  // b
        Materials[4][3].MaterialDiffuse[3] = 1.0f; // a


        // specular material
        Materials[4][3].MaterialSpecular[0] = 0.7f;  // r
        Materials[4][3].MaterialSpecular[1] = 0.7f;  // g
        Materials[4][3].MaterialSpecular[2] = 0.7f;  // b
        Materials[4][3].MaterialSpecular[3] = 1.0f; // a

        // shininess
        Materials[4][3].MaterialShininess = 0.078125f * 128.0f;

        // ***** 6th sphere on 4th column, yellow rubber *****
        // ambient material
        Materials[5][3].MaterialAmbient[0] = 0.05f; // r
        Materials[5][3].MaterialAmbient[1] = 0.05f; // g
        Materials[5][3].MaterialAmbient[2] = 0.0f;  // b
        Materials[5][3].MaterialAmbient[3] = 1.0f; // a


        // diffuse material
        Materials[5][3].MaterialDiffuse[0] = 0.5f;  // r
        Materials[5][3].MaterialDiffuse[1] = 0.5f;  // g
        Materials[5][3].MaterialDiffuse[2] = 0.4f;  // b
        Materials[5][3].MaterialDiffuse[3] = 1.0f; // a


        // specular material
        Materials[5][3].MaterialSpecular[0] = 0.7f;  // r
        Materials[5][3].MaterialSpecular[1] = 0.7f;  // g
        Materials[5][3].MaterialSpecular[2] = 0.04f; // b
        Materials[5][3].MaterialSpecular[3] = 1.0f; // a

        // shininess
        Materials[5][3].MaterialShininess = 0.078125f * 128.0f;

        return Materials;
    }
}
