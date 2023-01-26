#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 uv;

layout(location = 0) out vec3 fragColor;

layout(push_constant) uniform Push {
  mat4 transform; // projection * view * model
  mat4 normalMatrix;
} push;

const vec3 DIRECTION_TO_LIGHT = normalize(vec3(10.0, -30.0, -10.0));
const vec3 DIRECTION_TO_LIGHT2 = normalize(vec3(-0.5, 30.0, 10.0));
const float AMBIENT = 0.01;

void main() {
  gl_Position = push.transform * vec4(position, 1.0);

  vec3 normalWorldSpace = (mat3(push.normalMatrix) * normal);

  float lightIntensity = AMBIENT + 0.74 *max(dot(normalWorldSpace, DIRECTION_TO_LIGHT), 0) + 0.26 * max(dot(normalWorldSpace, DIRECTION_TO_LIGHT2), 0) ;

  fragColor = lightIntensity  * color;
}