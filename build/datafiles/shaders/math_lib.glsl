float compatible_pow(float x, float y)
{
  if (y == 0.0) { /* x^0 -> 1, including 0^0 */
    return 1.0;
  }

  /* glsl pow doesn't accept negative x */
  if (x < 0.0) {
    if (mod(-y, 2.0) == 0.0) {
      return pow(-x, y);
    }
    else {
      return -pow(-x, y);
    }
  }
  else if (x == 0.0) {
    return 0.0;
  }

  return pow(x, y);
}

vec3 transform_point(mat4 m, vec3 v) {
  return (m * vec4(v, 1.0)).xyz;
}
