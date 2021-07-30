var materials = new Array(24);

function initMaterials() {
    for (var i = 0; i < 6; i++) {
        for (var j = 0; j < 4; j++) {
            materials[(4 * i) + j] = {
                MaterialAmbient: new Float32Array(3),
                MaterialDiffuse: new Float32Array(3),
                MaterialSpecular: new Float32Array(3),
                MaterialShininess: 0.0
            };
        }
    }

    // ***** 1st sphere on 1st column, emerald *****
    // ambient material
    materials[0].MaterialAmbient[0] = 0.0215; // r
    materials[0].MaterialAmbient[1] = 0.1745; // g
    materials[0].MaterialAmbient[2] = 0.0215; // b

    // diffuse material
    materials[0].MaterialDiffuse[0] = 0.07568; // r
    materials[0].MaterialDiffuse[1] = 0.61424; // g
    materials[0].MaterialDiffuse[2] = 0.07568; // b

    // specular material
    materials[0].MaterialSpecular[0] = 0.633;    // r
    materials[0].MaterialSpecular[1] = 0.727811; // g
    materials[0].MaterialSpecular[2] = 0.633;    // b

    // shininess
    materials[0].MaterialShininess = 0.6 * 128.0;

    // ***** 2nd sphere on 1st column, jade *****ƒ
    // ambient material
    materials[1].MaterialAmbient[0] = 0.135;  // r
    materials[1].MaterialAmbient[1] = 0.2225; // g
    materials[1].MaterialAmbient[2] = 0.1575; // b

    // diffuse material
    materials[1].MaterialDiffuse[0] = 0.54; // r
    materials[1].MaterialDiffuse[1] = 0.89; // g
    materials[1].MaterialDiffuse[2] = 0.63; // b

    // specular material
    materials[1].MaterialSpecular[0] = 0.316228; // r
    materials[1].MaterialSpecular[1] = 0.316228; // g
    materials[1].MaterialSpecular[2] = 0.316228; // b

    // shininess
    materials[1].MaterialShininess = 0.1 * 128.0;

    // ***** 3rd sphere on 1st column, obsidian *****
    // ambient material
    materials[2].MaterialAmbient[0] = 0.05375; // r
    materials[2].MaterialAmbient[1] = 0.05;    // g
    materials[2].MaterialAmbient[2] = 0.06625; // b

    // diffuse material
    materials[2].MaterialDiffuse[0] = 0.18275; // r
    materials[2].MaterialDiffuse[1] = 0.17;    // g
    materials[2].MaterialDiffuse[2] = 0.22525; // b

    // specular material
    materials[2].MaterialSpecular[0] = 0.332741; // r
    materials[2].MaterialSpecular[1] = 0.328634; // g
    materials[2].MaterialSpecular[2] = 0.346435; // b

    // shininess
    materials[2].MaterialShininess = 0.3 * 128.0;

    // ***** 4th sphere on 1st column, pearl *****
    // ambient material
    materials[3].MaterialAmbient[0] = 0.25;    // r
    materials[3].MaterialAmbient[1] = 0.20725; // g
    materials[3].MaterialAmbient[2] = 0.20725; // b

    // diffuse material
    materials[3].MaterialDiffuse[0] = 1.0;   // r
    materials[3].MaterialDiffuse[1] = 0.829; // g
    materials[3].MaterialDiffuse[2] = 0.829; // b

    // specular material
    materials[3].MaterialSpecular[0] = 0.296648; // r
    materials[3].MaterialSpecular[1] = 0.296648; // g
    materials[3].MaterialSpecular[2] = 0.296648; // b

    // shininess
    materials[3].MaterialShininess = 0.088 * 128.0;

    // ***** 5th sphere on 1st column, ruby *****
    // ambient material
    materials[4].MaterialAmbient[0] = 0.1745;  // r
    materials[4].MaterialAmbient[1] = 0.01175; // g
    materials[4].MaterialAmbient[2] = 0.01175; // b


    // diffuse material
    materials[4].MaterialDiffuse[0] = 0.61424; // r
    materials[4].MaterialDiffuse[1] = 0.04136; // g
    materials[4].MaterialDiffuse[2] = 0.04136; // b


    // specular material
    materials[4].MaterialSpecular[0] = 0.727811; // r
    materials[4].MaterialSpecular[1] = 0.626959; // g
    materials[4].MaterialSpecular[2] = 0.626959; // b

    // shininess
    materials[4].MaterialShininess = 0.6 * 128.0;

    // ***** 6th sphere on 1st column, turquoise *****
    // ambient material
    materials[5].MaterialAmbient[0] = 0.1;     // r
    materials[5].MaterialAmbient[1] = 0.18725; // g
    materials[5].MaterialAmbient[2] = 0.1745;  // b


    // diffuse material
    materials[5].MaterialDiffuse[0] = 0.396;   // r
    materials[5].MaterialDiffuse[1] = 0.74151; // g
    materials[5].MaterialDiffuse[2] = 0.69102; // b


    // specular material
    materials[5].MaterialSpecular[0] = 0.297254; // r
    materials[5].MaterialSpecular[1] = 0.30829;  // g
    materials[5].MaterialSpecular[2] = 0.306678; // b

    // shininess
    materials[5].MaterialShininess = 0.1 * 128.0;

    // ***** 1st sphere on 2nd column, brass *****
    // ambient material
    materials[6].MaterialAmbient[0] = 0.329412; // r
    materials[6].MaterialAmbient[1] = 0.223529; // g
    materials[6].MaterialAmbient[2] = 0.027451; // b


    // diffuse material
    materials[6].MaterialDiffuse[0] = 0.780392; // r
    materials[6].MaterialDiffuse[1] = 0.568627; // g
    materials[6].MaterialDiffuse[2] = 0.113725; // b


    // specular material
    materials[6].MaterialSpecular[0] = 0.992157; // r
    materials[6].MaterialSpecular[1] = 0.941176; // g
    materials[6].MaterialSpecular[2] = 0.807843; // b

    // shininess
    materials[6].MaterialShininess = 0.21794872 * 128.0;

    // ***** 2nd sphere on 2nd column, bronze *****
    // ambient material
    materials[7].MaterialAmbient[0] = 0.2125; // r
    materials[7].MaterialAmbient[1] = 0.1275; // g
    materials[7].MaterialAmbient[2] = 0.054;  // b


    // diffuse material
    materials[7].MaterialDiffuse[0] = 0.714;   // r
    materials[7].MaterialDiffuse[1] = 0.4284;  // g
    materials[7].MaterialDiffuse[2] = 0.18144; // b


    // specular material
    materials[7].MaterialSpecular[0] = 0.393548; // r
    materials[7].MaterialSpecular[1] = 0.271906; // g
    materials[7].MaterialSpecular[2] = 0.166721; // b

    // shininess
    materials[7].MaterialShininess = 0.2 * 128.0;

    // ***** 3rd sphere on 2nd column, chrome *****
    // ambient material
    materials[8].MaterialAmbient[0] = 0.25; // r
    materials[8].MaterialAmbient[1] = 0.25; // g
    materials[8].MaterialAmbient[2] = 0.25; // b


    // diffuse material
    materials[8].MaterialDiffuse[0] = 0.4;  // r
    materials[8].MaterialDiffuse[1] = 0.4;  // g
    materials[8].MaterialDiffuse[2] = 0.4;  // b


    // specular material
    materials[8].MaterialSpecular[0] = 0.774597; // r
    materials[8].MaterialSpecular[1] = 0.774597; // g
    materials[8].MaterialSpecular[2] = 0.774597; // b

    // shininess
    materials[8].MaterialShininess = 0.6 * 128.0;

    // ***** 4th sphere on 2nd column, copper *****
    // ambient material
    materials[9].MaterialAmbient[0] = 0.19125; // r
    materials[9].MaterialAmbient[1] = 0.0735;  // g
    materials[9].MaterialAmbient[2] = 0.0225;  // b


    // diffuse material
    materials[9].MaterialDiffuse[0] = 0.7038;  // r
    materials[9].MaterialDiffuse[1] = 0.27048; // g
    materials[9].MaterialDiffuse[2] = 0.0828;  // b


    // specular material
    materials[9].MaterialSpecular[0] = 0.256777; // r
    materials[9].MaterialSpecular[1] = 0.137622; // g
    materials[9].MaterialSpecular[2] = 0.086014; // b

    // shininess
    materials[9].MaterialShininess = 0.1 * 128.0;

    // ***** 5th sphere on 2nd column, gold *****
    // ambient material
    materials[10].MaterialAmbient[0] = 0.24725; // r
    materials[10].MaterialAmbient[1] = 0.1995;  // g
    materials[10].MaterialAmbient[2] = 0.0745;  // b


    // diffuse material
    materials[10].MaterialDiffuse[0] = 0.75164; // r
    materials[10].MaterialDiffuse[1] = 0.60648; // g
    materials[10].MaterialDiffuse[2] = 0.22648; // b


    // specular material
    materials[10].MaterialSpecular[0] = 0.628281; // r
    materials[10].MaterialSpecular[1] = 0.555802; // g
    materials[10].MaterialSpecular[2] = 0.366065; // b

    // shininess
    materials[10].MaterialShininess = 0.4 * 128.0;


    // ***** 6th sphere on 2nd column, silver *****
    // ambient material
    materials[11].MaterialAmbient[0] = 0.19225; // r
    materials[11].MaterialAmbient[1] = 0.19225; // g
    materials[11].MaterialAmbient[2] = 0.19225; // b


    // diffuse material
    materials[11].MaterialDiffuse[0] = 0.50754; // r
    materials[11].MaterialDiffuse[1] = 0.50754; // g
    materials[11].MaterialDiffuse[2] = 0.50754; // b


    // specular material
    materials[11].MaterialSpecular[0] = 0.508273; // r
    materials[11].MaterialSpecular[1] = 0.508273; // g
    materials[11].MaterialSpecular[2] = 0.508273; // b

    // shininess
    materials[11].MaterialShininess = 0.4 * 128.0;


    // ***** 1st sphere on 3rd column, black *****
    // ambient material
    materials[12].MaterialAmbient[0] = 0.0;  // r
    materials[12].MaterialAmbient[1] = 0.0;  // g
    materials[12].MaterialAmbient[2] = 0.0;  // b


    // diffuse material
    materials[12].MaterialDiffuse[0] = 0.01; // r
    materials[12].MaterialDiffuse[1] = 0.01; // g
    materials[12].MaterialDiffuse[2] = 0.01; // b


    // specular material
    materials[12].MaterialSpecular[0] = 0.50; // r
    materials[12].MaterialSpecular[1] = 0.50; // g
    materials[12].MaterialSpecular[2] = 0.50; // b

    // shininess
    materials[12].MaterialShininess = 0.25 * 128.0;


    // ***** 2nd sphere on 3rd column, cyan *****
    // ambient material
    materials[13].MaterialAmbient[0] = 0.0;  // r
    materials[13].MaterialAmbient[1] = 0.1;  // g
    materials[13].MaterialAmbient[2] = 0.06; // b


    // diffuse material
    materials[13].MaterialDiffuse[0] = 0.0;        // r
    materials[13].MaterialDiffuse[1] = 0.50980392; // g
    materials[13].MaterialDiffuse[2] = 0.50980392; // b


    // specular material
    materials[13].MaterialSpecular[0] = 0.50196078; // r
    materials[13].MaterialSpecular[1] = 0.50196078; // g
    materials[13].MaterialSpecular[2] = 0.50196078; // b

    // shininess
    materials[13].MaterialShininess = 0.25 * 128.0;

    // ***** 3rd sphere on 2nd column, green *****
    // ambient material
    materials[14].MaterialAmbient[0] = 0.0;  // r
    materials[14].MaterialAmbient[1] = 0.0;  // g
    materials[14].MaterialAmbient[2] = 0.0;  // b


    // diffuse material
    materials[14].MaterialDiffuse[0] = 0.1;  // r
    materials[14].MaterialDiffuse[1] = 0.35; // g
    materials[14].MaterialDiffuse[2] = 0.1;  // b


    // specular material
    materials[14].MaterialSpecular[0] = 0.45; // r
    materials[14].MaterialSpecular[1] = 0.55; // g
    materials[14].MaterialSpecular[2] = 0.45; // b

    // shininess
    materials[14].MaterialShininess = 0.25 * 128.0;

    // ***** 4th sphere on 3rd column, red *****
    // ambient material
    materials[15].MaterialAmbient[0] = 0.0;  // r
    materials[15].MaterialAmbient[1] = 0.0;  // g
    materials[15].MaterialAmbient[2] = 0.0;  // b


    // diffuse material
    materials[15].MaterialDiffuse[0] = 0.5;  // r
    materials[15].MaterialDiffuse[1] = 0.0;  // g
    materials[15].MaterialDiffuse[2] = 0.0;  // b


    // specular material
    materials[15].MaterialSpecular[0] = 0.7;  // r
    materials[15].MaterialSpecular[1] = 0.6;  // g
    materials[15].MaterialSpecular[2] = 0.6;  // b

    // shininess
    materials[15].MaterialShininess = 0.25 * 128.0;

    // ***** 5th sphere on 3rd column, white *****
    // ambient material
    materials[16].MaterialAmbient[0] = 0.0;  // r
    materials[16].MaterialAmbient[1] = 0.0;  // g
    materials[16].MaterialAmbient[2] = 0.0;  // b


    // diffuse material
    materials[16].MaterialDiffuse[0] = 0.55; // r
    materials[16].MaterialDiffuse[1] = 0.55; // g
    materials[16].MaterialDiffuse[2] = 0.55; // b


    // specular material
    materials[16].MaterialSpecular[0] = 0.70; // r
    materials[16].MaterialSpecular[1] = 0.70; // g
    materials[16].MaterialSpecular[2] = 0.70; // b

    // shininess
    materials[16].MaterialShininess = 0.25 * 128.0;

    // ***** 6th sphere on 3rd column, yellow plastic *****
    // ambient material
    materials[17].MaterialAmbient[0] = 0.0;  // r
    materials[17].MaterialAmbient[1] = 0.0;  // g
    materials[17].MaterialAmbient[2] = 0.0;  // b


    // diffuse material
    materials[17].MaterialDiffuse[0] = 0.5;  // r
    materials[17].MaterialDiffuse[1] = 0.5;  // g
    materials[17].MaterialDiffuse[2] = 0.0;  // b


    // specular material
    materials[17].MaterialSpecular[0] = 0.60; // r
    materials[17].MaterialSpecular[1] = 0.60; // g
    materials[17].MaterialSpecular[2] = 0.50; // b

    // shininess
    materials[17].MaterialShininess = 0.25 * 128.0;

    // ***** 1st sphere on 4th column, black *****
    // ambient material
    materials[18].MaterialAmbient[0] = 0.02; // r
    materials[18].MaterialAmbient[1] = 0.02; // g
    materials[18].MaterialAmbient[2] = 0.02; // b


    // diffuse material
    materials[18].MaterialDiffuse[0] = 0.01; // r
    materials[18].MaterialDiffuse[1] = 0.01; // g
    materials[18].MaterialDiffuse[2] = 0.01; // b


    // specular material
    materials[18].MaterialSpecular[0] = 0.4;  // r
    materials[18].MaterialSpecular[1] = 0.4;  // g
    materials[18].MaterialSpecular[2] = 0.4;  // b

    // shininess
    materials[18].MaterialShininess = 0.078125 * 128.0;

    // ***** 2nd sphere on 4th column, cyan *****
    // ambient material
    materials[19].MaterialAmbient[0] = 0.0;  // r
    materials[19].MaterialAmbient[1] = 0.05; // g
    materials[19].MaterialAmbient[2] = 0.05; // b


    // diffuse material
    materials[19].MaterialDiffuse[0] = 0.4;  // r
    materials[19].MaterialDiffuse[1] = 0.5;  // g
    materials[19].MaterialDiffuse[2] = 0.5;  // b


    // specular material
    materials[19].MaterialSpecular[0] = 0.04; // r
    materials[19].MaterialSpecular[1] = 0.7;  // g
    materials[19].MaterialSpecular[2] = 0.7;  // b

    // shininess
    materials[19].MaterialShininess = 0.078125 * 128.0;


    // ***** 3rd sphere on 4th column, green *****
    // ambient material
    materials[20].MaterialAmbient[0] = 0.0;  // r
    materials[20].MaterialAmbient[1] = 0.05; // g
    materials[20].MaterialAmbient[2] = 0.0;  // b


    // diffuse material
    materials[20].MaterialDiffuse[0] = 0.4;  // r
    materials[20].MaterialDiffuse[1] = 0.5;  // g
    materials[20].MaterialDiffuse[2] = 0.4;  // b


    // specular material
    materials[20].MaterialSpecular[0] = 0.04; // r
    materials[20].MaterialSpecular[1] = 0.7;  // g
    materials[20].MaterialSpecular[2] = 0.04; // b

    // shininess
    materials[20].MaterialShininess = 0.078125 * 128.0;

    // ***** 4th sphere on 4th column, red *****
    // ambient material
    materials[21].MaterialAmbient[0] = 0.05; // r
    materials[21].MaterialAmbient[1] = 0.0;  // g
    materials[21].MaterialAmbient[2] = 0.0;  // b


    // diffuse material
    materials[21].MaterialDiffuse[0] = 0.5;  // r
    materials[21].MaterialDiffuse[1] = 0.4;  // g
    materials[21].MaterialDiffuse[2] = 0.4;  // b


    // specular material
    materials[21].MaterialSpecular[0] = 0.7;  // r
    materials[21].MaterialSpecular[1] = 0.04; // g
    materials[21].MaterialSpecular[2] = 0.04; // b

    // shininess
    materials[21].MaterialShininess = 0.078125 * 128.0;


    // ***** 5th sphere on 4th column, white *****
    // ambient material
    materials[22].MaterialAmbient[0] = 0.05; // r
    materials[22].MaterialAmbient[1] = 0.05; // g
    materials[22].MaterialAmbient[2] = 0.05; // b


    // diffuse material
    materials[22].MaterialDiffuse[0] = 0.5;  // r
    materials[22].MaterialDiffuse[1] = 0.5;  // g
    materials[22].MaterialDiffuse[2] = 0.5;  // b


    // specular material
    materials[22].MaterialSpecular[0] = 0.7;  // r
    materials[22].MaterialSpecular[1] = 0.7;  // g
    materials[22].MaterialSpecular[2] = 0.7;  // b

    // shininess
    materials[22].MaterialShininess = 0.078125 * 128.0;

    // ***** 6th sphere on 4th column, yellow rubber *****
    // ambient material
    materials[23].MaterialAmbient[0] = 0.05; // r
    materials[23].MaterialAmbient[1] = 0.05; // g
    materials[23].MaterialAmbient[2] = 0.0;  // b


    // diffuse material
    materials[23].MaterialDiffuse[0] = 0.5;  // r
    materials[23].MaterialDiffuse[1] = 0.5;  // g
    materials[23].MaterialDiffuse[2] = 0.4;  // b


    // specular material
    materials[23].MaterialSpecular[0] = 0.7;  // r
    materials[23].MaterialSpecular[1] = 0.7;  // g
    materials[23].MaterialSpecular[2] = 0.04; // b

    // shininess
    materials[23].MaterialShininess = 0.078125 * 128.0;

}
