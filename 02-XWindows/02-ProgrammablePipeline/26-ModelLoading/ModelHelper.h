#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <vector>

#define ERRORBOX1(lpszErrorMsg, lpszCaption) {							\
												printf(lpszErrorMsg)	\
											 }

#define ERRORBOX2(lpszErrorMsg, lpszCaption) {							\
												printf(lpszErrorMsg)	\
											 }

#define BUFFERSIZE		    256
#define S_EQUAL             0
#define NR_NORMAL_QUADS     3
#define NR_POINTS_QUADS     3
#define NR_TEXTURE_QUADS    2
#define MIN_NR_FACE_TOKENS  3
#define MAX_NR_FACE_TOKENS  3

FILE *gfpMeshFile = NULL;
FILE *gfpLogFile = NULL;

char line[BUFFERSIZE];

std::vector<std::vector<float>> gVertices;
std::vector<std::vector<float>> gTexture;
std::vector<std::vector<float>> gNormals;
std::vector<std::vector<int>> gFaceTri, gFaceTexture, gFaceNormals;

// load mesh data from obj file
float *LoadMeshData(void);

float *LoadMeshData(void)
{
	// function declarations
	void uninitialize(void);

	// code
	gfpMeshFile = fopen("MonkeyHead.obj", "r");
	if (!gfpMeshFile) uninitialize();

	// log file for model loading verification
	gfpLogFile = fopen("MeshLoading.log", "w");
	if (!gfpLogFile) uninitialize();

	char *sepSpace = (char *)" ";
	char *sepFwdSlash = (char *)"/";
	char *firstToken = NULL;
	char *token = NULL;
	char *nextToken = NULL;
	char *faceToken[MAX_NR_FACE_TOKENS];
	int nrTokens = 0;
	char *tokenVertexIndex = NULL;
	char *tokenTextureIndex = NULL;
	char *tokenNormalIndex = NULL;

	while (
		(fgets(line, BUFFERSIZE, gfpMeshFile) != NULL) &&
		!feof(gfpMeshFile))
	{
		firstToken = strtok(line, sepSpace);

		if (strcmp(firstToken, "v") == S_EQUAL)
		{
			std::vector<float> vecPointQuad(NR_POINTS_QUADS);

			for (int i = 0; i != NR_POINTS_QUADS; i++)
				vecPointQuad[i] = (float)atof(strtok(NULL, sepSpace));

			gVertices.push_back(vecPointQuad);
		}
		else if (strcmp(firstToken, "vt") == S_EQUAL)
		{
			std::vector<float> vecTextureQuad(NR_TEXTURE_QUADS);

			for (int i = 0; i != NR_TEXTURE_QUADS; i++)
				vecTextureQuad[i] = (float)atof(strtok(NULL, sepSpace));

			gTexture.push_back(vecTextureQuad);
		}
		else if (strcmp(firstToken, "vn") == S_EQUAL)
		{
			std::vector<float> vecNormalQuad(NR_NORMAL_QUADS);

			for (int i = 0; i != NR_NORMAL_QUADS; i++)
				vecNormalQuad[i] = (float)atof(strtok(NULL, sepSpace));

			gNormals.push_back(vecNormalQuad);
		}
		else if (strcmp(firstToken, "f") == S_EQUAL)
		{
			std::vector<int> triangleVertexIndices(3), textureVertexIndices(3), normalVertexIndices(3);
			memset((void *)faceToken, 0, MAX_NR_FACE_TOKENS);
			nrTokens = 0;
			while (token = strtok(NULL, sepSpace))
			{
				if (strlen(token) < 3) break;

				faceToken[nrTokens++] = token;
			}

			for (int i = 0; i != MIN_NR_FACE_TOKENS; ++i)
			{
				tokenVertexIndex = strtok(faceToken[i], sepFwdSlash);
				tokenTextureIndex = strtok(NULL, sepFwdSlash);
				//tokenNormalIndex = strtok(NULL, sepFwdSlash);

				triangleVertexIndices[i] = atoi(tokenVertexIndex);
				textureVertexIndices[i] = atoi(tokenTextureIndex);
				//normalVertexIndices[i] = atoi(tokenNormalIndex);
			}

			gFaceTri.push_back(triangleVertexIndices);
			gFaceTexture.push_back(textureVertexIndices);
			gFaceNormals.push_back(normalVertexIndices);

			/* Special case where face is consist of 4 vertiecs */
			if (nrTokens == MAX_NR_FACE_TOKENS)
			{
				triangleVertexIndices[1] = triangleVertexIndices[2];
				triangleVertexIndices[2] = atoi(tokenVertexIndex);

				textureVertexIndices[1] = textureVertexIndices[2];
				textureVertexIndices[2] = atoi(tokenTextureIndex);

				//normalVertexIndices[1] = normalVertexIndices[2];
				//normalVertexIndices[2] = atoi(tokenNormalIndex);

				tokenVertexIndex = strtok(faceToken[MAX_NR_FACE_TOKENS - 1], sepFwdSlash);
				tokenTextureIndex = strtok(NULL, sepFwdSlash);
				//tokenNormalIndex = strtok(NULL, sepFwdSlash);

				gFaceTri.push_back(triangleVertexIndices);
				gFaceTexture.push_back(textureVertexIndices);
				//gFaceNormals.push_back(normalVertexIndices);

			}
		}
		memset((void *)line, 0, BUFFERSIZE);
	}

	fclose(gfpMeshFile);
	gfpMeshFile = NULL;

	fprintf(gfpLogFile, "gVertices: %zu \ngTexture: %zu \ngNormals: %zu \ngFaceTri: %zu",
		gVertices.size(), gTexture.size(), gNormals.size(), gFaceTri.size());

	int idx = 0;
	float *coords = (float *)malloc(gFaceTri.size()*3*3*sizeof(float));
	for (int i = 0; i != gFaceTri.size(); ++i)
	{
		for (int j = 0; j != gFaceTri[i].size(); j++)
		{
			int vi = gFaceTri[i][j] - 1;
			coords[idx++] = gVertices[vi][0];
			coords[idx++] = gVertices[vi][1];
			coords[idx++] = gVertices[vi][2];
		}
	}
	return coords;
}