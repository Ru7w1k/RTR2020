#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

#include "XNAMath\xnamath.h"

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

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
void getPose(Animation&, Bone&, float, vector<XMMATRIX>&, XMMATRIX&, XMMATRIX&);
void getEncodedFrame(vector<XMMATRIX>& , vector<XMMATRIX>& );
uint CreateAnimationTexture(Animation&, Bone&, int, int, ID3D11ShaderResourceView**);


XMMATRIX assimpToXMMATRIX(aiMatrix4x4 m)
{
	/*return XMMATRIX(
		m.a1, m.a2, m.a3, m.a4,
		m.b1, m.b2, m.b3, m.b4,
		m.c1, m.c2, m.c3, m.c4,
		m.d1, m.d2, m.d3, m.d4
	);*/

	return XMMATRIX(
		m.a1, m.b1, m.c1, m.d1,
		m.a2, m.b2, m.c2, m.d2,
		m.a3, m.b3, m.c3, m.d3,
		m.a4, m.b4, m.c4, m.d4
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

		for (uint i = 0; i < node->mNumChildren; i++)
		{
			Bone child;
			readSkeleton(&child, node->mChildren[i], boneInfoTable);
			boneOutput->child.push_back(child);
		}
		return true;
	}
	else // find bones in childer
	{
		for (uint i = 0; i < node->mNumChildren; i++)
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
	for (uint i = 0; i < *nBoneCount; i++)
	{
		aiBone *bone = mesh->mBones[i];
		XMMATRIX m = assimpToXMMATRIX(bone->mOffsetMatrix);
		boneInfo[bone->mName.C_Str()] = { i, m };

		// loop through each vertex that have that bone
		for (uint j = 0; j < bone->mNumWeights; j++)
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
	for (uint i = 0; i < verticesOutput->size(); i++)
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
	for (uint i = 0; i < mesh->mNumFaces; i++)
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
		animation.ticksPerSecond = (float)anim->mTicksPerSecond;
	else
		animation.ticksPerSecond = 1;

	animation.duration = (float)anim->mDuration * (float)anim->mTicksPerSecond;
	animation.boneTransform = {};

	// load position, rotation and scale for each bone
	// each channel represents each bone
	for (uint i = 0; i < anim->mNumChannels; i++)
	{
		aiNodeAnim *channel = anim->mChannels[i];
		BoneTransformTrack track;
		
		for (uint j = 0; j < channel->mNumPositionKeys; j++)
		{
			track.positionTimestamps.push_back((float)channel->mPositionKeys[j].mTime);
			track.positions.push_back(assimpToXMFLOAT3(channel->mPositionKeys[j].mValue));
		}

		for (uint j = 0; j < channel->mNumRotationKeys; j++)
		{
			track.rotationTimestamps.push_back((float)channel->mRotationKeys[j].mTime);
			track.rotations.push_back(assimpToXMVECTOR(channel->mRotationKeys[j].mValue));
		}

		for (uint j = 0; j < channel->mNumScalingKeys; j++)
		{
			track.scaleTimestamps.push_back((float)channel->mScalingKeys[j].mTime);
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

	XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	XMMATRIX localTransform = XMMatrixAffineTransformation(XMLoadFloat3(&scale), zero, rotation, XMLoadFloat3(&position));
	
	XMMATRIX globalTransform = localTransform * parentTransform;
	output[skeleton.id] = ((XMMATRIX)skeleton.offset) * ((XMMATRIX)globalTransform) * ((XMMATRIX)globalInverseTransform);
	
	// update value for child bones
	for (Bone& child: skeleton.child)
	{
		getPose(animation, child, dt, output, globalTransform, globalInverseTransform);
	}
}

// generate texture with all animation frames x all bone matrices
uint CreateAnimationTexture(Animation& animation, Bone& skeleton, int fps, int boneCount, ID3D11ShaderResourceView **srv)
{
	extern ID3D11Device* gpID3D11Device;
	extern XMMATRIX globalInverseTransform;

	int maxBones = 0;
	int frameCount = animation.duration * fps;
	maxBones = frameCount * boneCount;
	ID3D11Texture2D* texture;

	XMMATRIX identity = XMMatrixIdentity();
	vector<XMMATRIX> currentPose = {};
	currentPose.resize(boneCount, identity);
	vector<XMMATRIX> encodedFrame = {};
	encodedFrame.resize(boneCount, identity);


	uint texelsPerBone = 4; // 3 texels for actual matrix data and 1 for padding..
	uint pixelCount = maxBones * texelsPerBone;
	uint texWidth = 0, texHeight = 0;
	
	// calculate texture width and height to form a 2D square texture
	texWidth = (uint)sqrt(pixelCount);
	texHeight = 1;
	while (texHeight < texWidth)
		texHeight = texHeight << 1; // 2s powers
	texWidth = texHeight;

	// create texture
	HRESULT hr = S_OK;
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
	desc.MipLevels = 1;
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.ArraySize = 1;
	desc.Width = texWidth;
	desc.Height = texHeight;
	desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

	// make buffer that is full size of texture
	uint bufferSize = texHeight * texWidth * sizeof(XMFLOAT4);
	XMFLOAT4* pData = new XMFLOAT4[desc.Width * desc.Height];
	memset((void*)pData, 0, bufferSize);

	XMFLOAT4* pCurrentDataPtr = pData;

	// iterate over each frame of animation
	float timeStep = animation.duration / (float)frameCount;
	float dt = 0.00001f;
	for (int frame = 0; frame < frameCount; frame++)
	{
		// get current time bone animation matrices
		//currentPose.resize(boneCount, XMMatrixScaling(1.0f, 15.0f, 1.0f));
		//currentPose.assign(boneCount, XMMatrixTranslation(0.0f, 15.0f, 0.0f));
		//encodedFrame.resize(boneCount, identity);
		getPose(animation, skeleton, dt, currentPose, identity, globalInverseTransform);
		getEncodedFrame(currentPose, encodedFrame);
		XMFLOAT4 *encData = (XMFLOAT4*) encodedFrame.data();
		for (int i = 0; i < boneCount; i++)
		{
			// copy matrix as float4x4 into texture memory
			memcpy((void*)pCurrentDataPtr++, (void*)&encData[i*4 + 0], sizeof(XMFLOAT4));
			memcpy((void*)pCurrentDataPtr++, (void*)&encData[i*4 + 1], sizeof(XMFLOAT4));
			memcpy((void*)pCurrentDataPtr++, (void*)&encData[i*4 + 2], sizeof(XMFLOAT4));
			memcpy((void*)pCurrentDataPtr++, (void*)&encData[i*4 + 3], sizeof(XMFLOAT4));
		}
		dt += timeStep;
	}

	// create a Texture2D from above data
	D3D11_SUBRESOURCE_DATA srd;
	srd.pSysMem = (void*)pData;
	srd.SysMemPitch = texWidth * (sizeof(XMFLOAT4));
	srd.SysMemSlicePitch = 1;
	gpID3D11Device->CreateTexture2D(&desc, &srd, &texture);
	
	delete[] pData;

	// create a shader resource view
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	srvDesc.Format = desc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	gpID3D11Device->CreateShaderResourceView(texture, &srvDesc, srv);

	texture->Release();
	texture = NULL;

	return texWidth;
}

// frame contains all bone matrices for one animation frame
// convert it into compressed XMFLOAT4 data to store as texture
void getEncodedFrame(vector<XMMATRIX>& frame, vector<XMMATRIX>& encodedFrame)
{
	for (int boneId = 0; boneId < frame.size(); boneId++)
	{
		encodedFrame[boneId] = frame[boneId];
		encodedFrame[boneId]._14 = frame[boneId]._41;
		encodedFrame[boneId]._24 = frame[boneId]._42;
		encodedFrame[boneId]._34 = frame[boneId]._43;
	}
}














