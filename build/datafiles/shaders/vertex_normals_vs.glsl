
void main() {
  gl_Position = view * model * vec4(aPos, 1.0);
  mat3 normalMatrix = mat3(transpose(inverse(view * model)));
  vs_inout.normal = normalize(vec3(vec4(normalMatrix * aNormal, 0.0)));
}