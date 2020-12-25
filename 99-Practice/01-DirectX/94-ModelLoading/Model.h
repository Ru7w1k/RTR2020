#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

#include "XNAMath\xnamath.h"

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;

typedef unsigned int  uint;
typedef unsigned char byte;

// types for parsing model
// vertex data of animated model
struct Vertex 
{
	XMFLOAT3 position;
	XMFLOAT3 normal;
	XMFLOAT2 uv;
	XMINT4 boneIDs = {0, 0, 0, 0};
	XMFLOAT4 boneWeights = {0.0f, 0.0f, 0.0f, 0.0f};
};

// struct to hold bone tree i.e. skeleton
struct Bone
{
	int id = 0;
	string name = "";
	XMMATRIX offset = XMMatrixIdentity();
	vector<Bone> child = {};
};

// struct to represent an animation track
struct BoneTransformTrack
{
	vector<float> positionTimestamps = {};
	vector<float> rotationTimestamps = {};
	vector<float> scaleTimestamps = {};

	vector<XMFLOAT3> positions = {};
	vector<XMVECTOR> rotations = {};
	vector<XMFLOAT3> scales = {};
};

// struct to contain the animation information
struct Animation
{
	float duration = 0.0f;
	float ticksPerSecond = 1.0f;
	unordered_map<string, BoneTransformTrack> boneTransform = {};
};

// helper functions
XMMATRIX assimpToXMMATRIX(aiMatrix4x4);
XMFLOAT3 assimpToXMFLOAT3(aiVector3D);
XMVECTOR assimpToXMVECTOR(aiQuaternion);
void Lerp(XMFLOAT3, XMFLOAT3, float, XMFLOAT3*);
void loadModel(const aiScene *, aiMesh *, vector<Vertex> *, vector<uint> *, Bone *, uint *);
void loadAnimation(const aiScene *, Animation&);


XMMATRIX assimpToXMMATRIX(aiMatrix4x4 m)
{
	return XMMATRIX(
		m.a1, m.a2, m.a3, m.a4,
		m.b1, m.b2, m.b3, m.b4,
		m.c1, m.c2, m.c3, m.c4,
		m.d1, m.d2, m.d3, m.d4
	);
}

XMFLOAT3 assimpToXMFLOAT3(aiVector3D v)
{
	return XMFLOAT3(v.x, v.y, v.z);
}

XMVECTOR assimpToXMVECTOR(aiQuaternion q)
{
	return XMVectorSet(q.x, q.y, q.z, q.w);
}

void Lerp(XMFLOAT3 a, XMFLOAT3 b, float t, XMFLOAT3 *res)
{
	XMVECTOR v1 = XMLoadFloat3(&a);
	XMVECTOR v2 = XMLoadFloat3(&b);
	XMStoreFloat3(res, XMVectorLerp(v1,v2, t));
}

// a recursive function to read all bones and form skeleton
bool readSkeleton(Bone *boneOutput, aiNode *node, unordered_map<string, pair<int, XMMATRIX>> *boneInfoTable)
{
	// check if the node is bone
	if (boneInfoTable->find(node->mName.C_Str()) != boneInfoTable->end())
	{
		boneOutput->name = node->mName.C_Str();
		boneOutput->id = (*boneInfoTable)[boneOutput->name].first;
		boneOutput->offset = (*boneInfoTable)[boneOutput->name].second;

		for (int i = 0; i < node->mNumChildren; i++)
		{
			Bone child;
			readSkeleton(&child, node->mChildren[i], boneInfoTable);
			boneOutput->child.push_back(child);
		}
		return true;
	}
	else // find bones in childer
	{
		for (int i = 0; i < node->mNumChildren; i++)
		{
			if (readSkeleton(boneOutput, node->mChildren[i], boneInfoTable))
			{
				return true;
			}
		}
	}
	return false;
}

void loadModel(const aiScene *scene, aiMesh *mesh, vector<Vertex> *verticesOutput, vector<uint> *indicesOutput, Bone *skeletonOutput, uint *nBoneCount)
{
	*verticesOutput = {};
	*indicesOutput = {};

	// load position, normal, texcoord
	for (uint i = 0; i < mesh->mNumVertices; i++)
	{
		// process position
		Vertex vertex;
		XMFLOAT3 vec;
		vec.x = mesh->mVertices[i].x;
		vec.y = mesh->mVertices[i].y;
		vec.z = mesh->mVertices[i].z;
		vertex.position = vec;

		// process normal
		vec.x = mesh->mNormals[i].x;
		vec.y = mesh->mNormals[i].y;
		vec.z = mesh->mNormals[i].z;
		vertex.normal = vec;

		// process uv
		XMFLOAT2 v;
		v.x = mesh->mTextureCoords[0][i].x;
		v.y = mesh->mTextureCoords[0][i].y;
		vertex.uv = v;

		vertex.boneIDs = XMINT4(0, 0, 0, 0);
		vertex.boneWeights = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);

		verticesOutput->push_back(vertex);
	}

	// load boneData to vertices
	unordered_map<string, pair<int, XMMATRIX>> boneInfo = {};
	vector<uint> boneCounts;
	boneCounts.resize(verticesOutput->size(), 0);
	*nBoneCount = mesh->mNumBones;

	// loop through each bone
	for (int i = 0; i < *nBoneCount; i++)
	{
		aiBone *bone = mesh->mBones[i];
		XMMATRIX m = assimpToXMMATRIX(bone->mOffsetMatrix);
		boneInfo[bone->mName.C_Str()] = { i, m };

		// loop through each vertex that have that bone
		for (int j = 0; j < bone->mNumWeights; j++)
		{
			uint id = bone->mWeights[j].mVertexId;
			float weight = bone->mWeights[j].mWeight;
			boneCounts[id]++;

			switch (boneCounts[id])
			{
			case 1:
				(*verticesOutput)[id].boneIDs.x = i;
				(*verticesOutput)[id].boneWeights.x = weight;
				break;

			case 2:
				(*verticesOutput)[id].boneIDs.y = i;
				(*verticesOutput)[id].boneWeights.y = weight;
				break;

			case 3:
				(*verticesOutput)[id].boneIDs.z = i;
				(*verticesOutput)[id].boneWeights.z = weight;
				break;

			case 4:
				(*verticesOutput)[id].boneIDs.w = i;
				(*verticesOutput)[id].boneWeights.w = weight;
				break;
			
			default:
				break;
			}
		}
	}

	// normalize weights to make all weights sum 1
	for (int i = 0; i < verticesOutput->size(); i++)
	{
		XMFLOAT4 &boneWeights = (*verticesOutput)[i].boneWeights;
		float totalWeight = boneWeights.x + boneWeights.y + boneWeights.z + boneWeights.w;
		if (totalWeight > 0.0f) 
		{
			(*verticesOutput)[i].boneWeights.x /= totalWeight;
			(*verticesOutput)[i].boneWeights.y /= totalWeight;
			(*verticesOutput)[i].boneWeights.z /= totalWeight;
			(*verticesOutput)[i].boneWeights.w /= totalWeight;
		}
	}

	// load indices
	for (int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace &face = mesh->mFaces[i];
		for (uint j = 0; j < face.mNumIndices; j++)
			indicesOutput->push_back(face.mIndices[j]);
	}

	// create bone tree
	readSkeleton(skeletonOutput, scene->mRootNode, &boneInfo);
}

void loadAnimation(const aiScene *scene, Animation& animation)
{
	// loading first animation
	aiAnimation *anim = scene->mAnimations[0];

	if (anim->mTicksPerSecond != 0.0f)
		animation.ticksPerSecond = anim->mTicksPerSecond;
	else
		animation.ticksPerSecond = 1;

	animation.duration = anim->mDuration * anim->mTicksPerSecond;
	animation.boneTransform = {};

	// load position, rotation and scale for each bone
	// each channel represents each bone
	for (int i = 0; i < anim->mNumChannels; i++)
	{
		aiNodeAnim *channel = anim->mChannels[i];
		BoneTransformTrack track;

		for (int j = 0; j < channel->mNumPositionKeys; j++)
		{
			track.positionTimestamps.push_back(channel->mPositionKeys[j].mTime);
			track.positions.push_back(assimpToXMFLOAT3(channel->mPositionKeys[j].mValue));
		}

		for (int j = 0; j < channel->mNumRotationKeys; j++)
		{
			track.rotationTimestamps.push_back(channel->mRotationKeys[j].mTime);
			track.rotations.push_back(assimpToXMVECTOR(channel->mRotationKeys[j].mValue));
		}

		for (int j = 0; j < channel->mNumScalingKeys; j++)
		{
			track.scaleTimestamps.push_back(channel->mScalingKeys[j].mTime);
			track.scales.push_back(assimpToXMFLOAT3(channel->mScalingKeys[j].mValue));
		}

		animation.boneTransform[channel->mNodeName.C_Str()] = track;		
	}
}

pair<uint, float> getTimeFraction(vector<float>& times, float& dt)
{
	uint segment = 0;
	while (dt > times[segment])
		segment++;

	float start = times[segment - 1];
	float end = times[segment];
	float frac = (dt - start) / (end - start);
	return {segment, frac};
}

void getPose(Animation& animation, Bone& skeleton, float dt, vector<XMMATRIX>& output, XMMATRIX &parentTransform, XMMATRIX &globalInverseTransform)
{
	BoneTransformTrack &btt = animation.boneTransform[skeleton.name];
	
	// timestamp for which pose is required
	dt = fmod(dt, animation.duration);
	pair<uint, float> fp;

	// calculate interpolated position
	fp = getTimeFraction(btt.positionTimestamps, dt);
	XMFLOAT3 position1 = btt.positions[fp.first - 1];
	XMFLOAT3 position2 = btt.positions[fp.first];
	XMFLOAT3 position;
	Lerp(position1, position2, fp.second, &position);

	// calculate interpolated rotation
	fp = getTimeFraction(btt.rotationTimestamps, dt);
	XMVECTOR rotation1 = btt.rotations[fp.first - 1];
	XMVECTOR rotation2 = btt.rotations[fp.first];
	XMVECTOR rotation = XMQuaternionSlerp(rotation1, rotation2, fp.second);

	// calculate interpolated scale
	fp = getTimeFraction(btt.scaleTimestamps, dt);
	XMFLOAT3 scale1 = btt.scales[fp.first - 1];
	XMFLOAT3 scale2 = btt.scales[fp.first];
	XMFLOAT3 scale;
	Lerp(scale1, scale2, fp.second, &scale);

	// calculate local transfomation
	XMMATRIX positionM = XMMatrixIdentity();
	XMMATRIX rotationM = XMMatrixIdentity();
	XMMATRIX scaleM = XMMatrixIdentity();

	positionM = XMMatrixTranslation(position.x, position.y, position.z);
	rotationM = XMMatrixRotationQuaternion(rotation);
	scaleM = XMMatrixScaling(scale.x, scale.y, scale.z);

	XMMATRIX localTransform = scaleM * rotationM * positionM;
	XMMATRIX globalTransform = parentTransform * localTransform;

	output[skeleton.id] = globalInverseTransform * globalTransform * skeleton.offset;
	
	// update value for child bones
	for (Bone& child: skeleton.child)
	{
		getPose(animation, child, dt, output, globalTransform, globalInverseTransform);
	}
}


















