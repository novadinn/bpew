const float fadeOffset = 30.0;

float inverseLerp(float from, float to, float t) {
  return (t - from) / (to - from);
}

void main() {
  float dstToCamera = distance(cameraPosition, vs_inout.vertexPosition);
  float t = inverseLerp(far - fadeOffset, far, dstToCamera);

  fragColor = vec4(color, 1.0);
  fragColor.a = mix(0, 1, 1 - t);
}
