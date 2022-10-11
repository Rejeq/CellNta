#shader vertex
#if GL_ES
precision mediump float;
#endif

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 offset;

uniform mat4 u_projView;

void main()
{
  gl_Position = u_projView * vec4(position + offset, 1.0f);
}

#shader fragment
#if GL_ES
precision mediump float;
//#extension GL_OES_texture_buffer : require
#endif

out vec4 f_outColor;

uniform samplerBuffer u_facesColorTex;

void main()
{
  f_outColor = texelFetch(u_facesColorTex, gl_PrimitiveID).rgba;
}
