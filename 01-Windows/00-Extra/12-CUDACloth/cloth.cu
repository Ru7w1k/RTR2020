
/* helper functions for float3 */
__host__ __device__ float3 operator+(const float3 &a, const float3 &b) 
{
	return make_float3(a.x+b.x, a.y+b.y, a.z+b.z);
}

__host__ __device__ float3 operator-(const float3 &a, const float3 &b) 
{
	return make_float3(a.x-b.x, a.y-b.y, a.z-b.z);
}

__host__ __device__ float3 operator*(const float3 &a, float b) 
{
	return make_float3(a.x*b, a.y*b, a.z*b);
}

__host__ __device__ float3 operator*(float b, const float3 &a) 
{
	return make_float3(a.x*b, a.y*b, a.z*b);
}

__host__ __device__ float3 operator/(const float3 &a, float b) 
{
	return make_float3(a.x/b, a.y/b, a.z/b);
}

__host__ __device__ float3 operator/(float b, const float3 &a) 
{
	return make_float3(a.x/b, a.y/b, a.z/b);
}

__host__ __device__ float length(const float3 &a) 
{
	return sqrt(a.x*a.x + a.y*a.y + a.z*a.z);
}

__host__ __device__ float3 normalize(const float3 &a) 
{
	return a/length(a);
}

__host__ __device__ float3 cross(const float3 &a, const float3 &b)
{
	return make_float3(
		(a.y*b.z - a.z*b.y),
		(-(a.x*b.z - a.z*b.x)),
		(a.x*b.y - a.y*b.x)
	);
}

__host__ __device__ float3 make_float3(const float4 &b)
{
	return make_float3(b.x, b.y, b.z);
}


// cloth update
__global__ void cloth_kernel(float4 *pos1, float4 *pos2, float4 *vel1, float4 *vel2, unsigned int width, unsigned int height, float3 wind, float xOffset)
{
	unsigned int x = (blockIdx.x*blockDim.x) + threadIdx.x;
	unsigned int y = (blockIdx.y*blockDim.y) + threadIdx.y;

	unsigned int idx = (y*width) + x;

	if (idx >= width*height) return;

	const float m = 1.0f;
	const float t = 0.000005 * 4;
	const float k = 6000.0;
	const float c = 0.95;
	const float rest_length = 1.00;
	const float rest_length_diag = 1.41;

	float3 p = make_float3(pos1[idx].x, pos1[idx].y, pos1[idx].z);
	float3 u = make_float3(vel1[idx].x, vel1[idx].y, vel1[idx].z);
	float3 F = make_float3(0.0f, -10.0f, 0.0f) * m - c * u;
	int i = 0;

	F = F + wind;

	if (true) // (vel1[idx].w >= 0.0f)
	{
		// calculate 8 connections
		// up
		if (y < height-1)
		{
			i = idx+width;
			float3 q = make_float3(pos1[i].x, pos1[i].y, pos1[i].z);
			float3 d = q - p;
			float x = length(d);
			F = F + -k * (rest_length - x) * normalize(d);
		}
		// down
		if (y > 0)
		{
			i = idx-width;
			float3 q = make_float3(pos1[i].x, pos1[i].y, pos1[i].z);
			float3 d = q - p;
			float x = length(d);
			F = F + -k * (rest_length - x) * normalize(d);
		}
		// left
		if (x > 0)
		{
			i = idx-1;
			float3 q = make_float3(pos1[i].x, pos1[i].y, pos1[i].z);
			float3 d = q - p;
			float x = length(d);
			F = F + -k * (rest_length - x) * normalize(d);
		}
		// right
		if (x < width-1)
		{
			i = idx+1;
			float3 q = make_float3(pos1[i].x, pos1[i].y, pos1[i].z);
			float3 d = q - p;
			float x = length(d);
			F = F + -k * (rest_length - x) * normalize(d);
		}

		// lower left
		if (x > 0 && y > 0)
		{
			i = idx-1-width;
			float3 q = make_float3(pos1[i].x, pos1[i].y, pos1[i].z);
			float3 d = q - p;
			float x = length(d);
			F = F + -k * (rest_length_diag - x) * normalize(d);
		}
		// upper right
		if (x < (width-1) && y < (height-1))
		{
			i = idx+1+width;
			float3 q = make_float3(pos1[i].x, pos1[i].y, pos1[i].z);
			float3 d = q - p;
			float x = length(d);
			F = F + -k * (rest_length_diag - x) * normalize(d);
		}
		// lower right
		if (x < (width-1) && y > 0)
		{
			i = idx+1-width;
			float3 q = make_float3(pos1[i].x, pos1[i].y, pos1[i].z);
			float3 d = q - p;
			float x = length(d);
			F = F + -k * (rest_length_diag - x) * normalize(d);
		}
		// upper left
		if (x > 0 && y < (height-1))
		{
			i = idx-1+width;
			float3 q = make_float3(pos1[i].x, pos1[i].y, pos1[i].z);
			float3 d = q - p;
			float x = length(d);
			F = F + -k * (rest_length_diag - x) * normalize(d);
		}

	}
	else
	{
		F = make_float3(0.0f, 0.0f, 0.0f);
	}

	// self collision!
	//int nbrs[] = {idx+width,idx-width,idx-1,idx+1,idx-1-width,idx+1+width,idx+1-width,idx-1+width};
	

	float3 a = F/m;
	float3 s = u * t + 0.5f * a * t * t;
	float3 v = u + a * t;

	
	// else if (vec3(p+s).y <= -4.0 && abs(vec3(p+s).x) < 5.5 && abs(vec3(p+s).z) < 5.5)
	// {	
	// 	s = vec3(0.0);
	// 	v = vec3(0.0);
	// }	
	

	// float force = length(F);
	// for(int i = 0; i < width*height && i!=idx; i++)
	// {
	// 	float3 q = make_float3(pos1[i].x, pos1[i].y, pos1[i].z);
	// 	float3 d = q - pos;
	// 	if(length(d) < 0.4)
	// 		v = v-force*normalize(d);
		
	// }

	// if (pos.y <= -2.0 && abs(pos.x) < 10.5 && abs(pos.z) < 10.5)
	// {	
	// 	pos = p;
	// 	v = make_float3(0.0f, 0.0f, 0.0f);
	// }
	// else 


	
	float3 op = p-make_float3(-15.0f,-4.0f,-15.0f);
	float lop = length(op);
	if (lop < 8.0)
	{	
		s.y = 0.0f;
		v.y = 0.0f;
	}

	op = p-make_float3(15.0f,-4.0f,-15.0f);
	lop = length(op);
	if (lop < 8.0)
	{	
		s.y = 0.0f;
		v.y = 0.0f;
	}

	op = p-make_float3(-15.0f,-4.0f,15.0f);
	lop = length(op);
	if (lop < 8.0)
	{	
		s.y = 0.0f;
		v.y = 0.0f;
	}

	op = p-make_float3(15.0f,-4.0f,15.0f);
	lop = length(op);
	if (lop < 8.0)
	{	
		s.y = 0.0f;
		v.y = 0.0f;
	}

	
	// if (p.y <= -4.0 && abs(p.x) < 15.5 && abs(p.z) < 15.5)
	// {
	// 	s.y = 0.0f;
	// 	v.y = 0.0f;
	// }

	

	float3 pos = p + s;

	pos2[idx] = make_float4(pos.x, pos.y, pos.z, 1.0f);
	vel2[idx] = make_float4(v.x, v.y, v.z, vel1[idx].w);

	return;
}

__global__ void cloth_normals(float4 *pos, float3 *norm, unsigned int width, unsigned int height)
{
	unsigned int x = (blockIdx.x*blockDim.x) + threadIdx.x;
	unsigned int y = (blockIdx.y*blockDim.y) + threadIdx.y;

	unsigned int idx = (y*width) + x;

	if (idx >= width*height) return;

	float3 p = make_float3(pos[idx].x, pos[idx].y, pos[idx].z);
	float3 n = make_float3(0.0f, 0.0f, 0.0f);
	float3 a, b, c;

	if (y < height-1)
	{
		c = make_float3(pos[idx+width]) - p;
		if (x < width-1)
		{
			a = make_float3(pos[idx+1]) - p;
			b = make_float3(pos[idx+width+1]) - p;
			n = n + cross(a, b);
			n = n + cross(b, c);
		}
		if (x > 0)
		{
			a = c;
			b = make_float3(pos[idx+width-1]) - p;
			c = make_float3(pos[idx-1]) - p;
			n = n + cross(a, b);
			n = n + cross(b, c);
		}
	}

	if (y > 0)
	{
		c = make_float3(pos[idx-width]) - p;
		if (x > 0)
		{
			a = make_float3(pos[idx-1]) - p;
			b = make_float3(pos[idx-width-1]) - p;
			n = n + cross(a, b);
			n = n + cross(b, c);
		}
		if (x < width-1)
		{
			a = c;
			b = make_float3(pos[idx-width+1]) - p;
			c = make_float3(pos[idx+1]) - p;
			n = n + cross(a, b);
			n = n + cross(b, c);
		}
	}

	norm[idx] = n;
}

void launchCUDAKernel(float4 *pos1, float4 *pos2, float4 *vel1, float4 *vel2, unsigned int meshWidth, unsigned int meshHeight, float3 *norm, float3 wind, float xOffset)
{
	dim3 block(16, 16, 1);
	dim3 grid(meshWidth / block.x, meshHeight / block.y, 1);

	for(int i = 0; i < 500; i++)
	{
		cloth_kernel<<<grid, block>>>(pos1, pos2, vel1, vel2, meshWidth, meshHeight, wind, xOffset);
		//cudaDeviceSynchronize();
		cloth_kernel<<<grid, block>>>(pos2, pos1, vel2, vel1, meshWidth, meshHeight, wind, xOffset);
		//cudaDeviceSynchronize();

	}
	//cudaDeviceSynchronize();
	cloth_normals<<<grid, block>>>(pos1, norm, meshWidth, meshHeight);
}

