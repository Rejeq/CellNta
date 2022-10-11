#shader vertex
#if GL_ES
precision mediump float;
#endif

out vec4 v_pos;

uniform mat4 u_projView;

vec4 gridPlane[12] = vec4[]
(
  vec4(-1.0f, 0.0f,-1.0f, 0.0f),
  vec4(-1.0f, 0.0f, 1.0f, 0.0f),
  vec4( 0.0f, 0.0f, 0.0f, 1.0f),
  vec4(-1.0f, 0.0f, 1.0f, 0.0f),
  vec4( 1.0f, 0.0f, 1.0f, 0.0f),
  vec4( 0.0f, 0.0f, 0.0f, 1.0f),

  vec4( 1.0f, 0.0f, 1.0f, 0.0f),
  vec4( 1.0f, 0.0f,-1.0f, 0.0f),
  vec4( 0.0f, 0.0f, 0.0f, 1.0f),
  vec4( 1.0f, 0.0f,-1.0f, 0.0f),
  vec4(-1.0f, 0.0f,-1.0f, 0.0f),
  vec4( 0.0f, 0.0f, 0.0f, 1.0f)
);

void main()
{
  vec4 pos = gridPlane[gl_VertexID];
  v_pos = pos;

  gl_Position = u_projView * pos;
}

#shader fragment
#if GL_ES
precision mediump float;
#endif

out vec4 f_outColor;

in vec4 v_pos;

uniform mat4  u_projView;
uniform float u_near;
uniform float u_far;

uniform vec4  u_gridColor;
uniform vec4  u_xAxisColor;
uniform vec4  u_zAxisColor;

vec4 grid(vec3 pos, float scale)
{
  vec2 coord = pos.xz * scale;
  vec2 derivative = fwidth(coord);
  vec2 grid = abs(fract(coord - 0.5f) - 0.5f) / derivative;
  float line = min(grid.x, grid.y);

  float alpha = 1.0f - min(line, 1.0f);
  vec4 gridColor = vec4(u_gridColor.rgb, u_gridColor.a * alpha);

  const float thickness = 0.5f;
  vec2 minPos = abs(min(derivative.xy, 1.0f));
  vec2 t = step(thickness * minPos, abs(pos.xz));

  vec4 outColor = u_xAxisColor * (1.0f - t.y);
  outColor += u_zAxisColor * (1.0f - t.x);
  outColor += gridColor * t.x * t.y;
  return outColor;
}

float computeDepth(vec3 pos)
{
  vec4 clip_space_pos = u_projView * vec4(pos.xyz, 1.0f);
  return clip_space_pos.z / clip_space_pos.w;
}

float computeLinearDepth(float depth)
{
  float z = depth * 2.0f - 1.0f;
  float linearDepth = (2.0f * u_near * u_far) / (u_far + u_near - z * (u_far - u_near));
  return linearDepth / u_far; // normalize
}

void main()
{
  const float shift = 0.5f;
  vec3 pos = v_pos.xyz / v_pos.w - shift;

  float depth = computeDepth(pos);
  float linearDepth = computeLinearDepth(depth);
  float fading = max(0.0f, (0.5f - linearDepth));

  vec4 color = grid(pos, 1.0f / 16.0f) + grid(pos, 1.0f);

  f_outColor = color;
  f_outColor.a *= fading;
}
