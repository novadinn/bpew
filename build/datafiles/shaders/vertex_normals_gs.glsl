void main() {
  for (int i = 0; i < gl_in.length(); ++i) {
    gl_Position = projection * gl_in[i].gl_Position;
    EmitVertex();
    gl_Position = projection * (gl_in[i].gl_Position +
                                vec4(vs_inout[i].normal, 0.0) * normalLength);
    EmitVertex();
    EndPrimitive();
  }
}