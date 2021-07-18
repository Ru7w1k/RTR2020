// OpenCL kernel
__kernel void sinewave(__global float4 *pos, unsigned int width,
                       unsigned int height, float animTime) {

  unsigned int x = get_global_id(0);
  unsigned int y = get_global_id(1);

  float u = x / (float)width;
  float v = y / (float)height;

  u = (u * 2.0) - 1.0;
  v = (v * 2.0) - 1.0;

  float freq = 4.0f;
  float w = sin(freq * u + animTime) * cos(freq * v + animTime) * 0.5f;

  pos[y * width + x] = (float4)(u, w, v, 1.0);
  return;
}
