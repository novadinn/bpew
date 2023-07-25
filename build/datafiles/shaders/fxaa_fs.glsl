void main() {
  fragColor = FxaaPixelShader(vs_inout.texCoordNoPInterp, colorTexture,
                              vec2(1 / viewportSize.x, 1 / viewportSize.y), 1.0,
                              0.125, 0.0625);
}
