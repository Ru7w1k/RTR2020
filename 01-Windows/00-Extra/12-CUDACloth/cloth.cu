
__global__ void sinewave_vbo_kernel(float4 *pos, unsigned int width, unsigned int height, float animTime)
{
	unsigned int x = (blockIdx.x*blockDim.x) + threadIdx.x;
	unsigned int y = (blockIdx.y*blockDim.y) + threadIdx.y;

	float u = x / (float)width;
	float v = y / (float)height;
	u = (u * 2.0f) - 1.0f;
	v = (v * 2.0f) - 1.0f;

	float freq = 4.0f;
	float w = sinf(freq*u + animTime) * cosf(freq*v + animTime) * 0.5f;

	pos[y*width + x] = make_float4(u, w, v, 1.0f);
	return;
}

void launchCUDAKernel(float4 *pos, unsigned int meshWidth, unsigned int meshHeight, float time)
{
	dim3 block(8, 1, 1);
	dim3 grid(meshWidth / block.x, meshHeight / block.y, 1);
	sinewave_vbo_kernel<<<grid, block>>>(pos, meshWidth, meshHeight, time);
}

