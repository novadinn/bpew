void main() {
    vs_inout.texCoord = aTexCoord;
    vs_inout.texCoordNoPInterp = aTexCoord;
    gl_Position = vec4(aPos.x, aPos.y, 0.0f, 1.0f);
}
