// geometry shader example usage
void main() {
	gl_Position = gl_in[0].gl_Position + vec4(0.1, 0.1, 0.1, 0.0);
	vs_inout_fs.fragPos = vs_inout[0].fragPos;
	vs_inout_fs.normal = vs_inout[0].normal;
	EmitVertex();

	gl_Position = gl_in[1].gl_Position + vec4(0.1, 0.1, 0.1, 0.0);
	vs_inout_fs.fragPos = vs_inout[1].fragPos;
	vs_inout_fs.normal = vs_inout[1].normal;
	EmitVertex();

	gl_Position = gl_in[2].gl_Position + vec4(0.1, 0.1, 0.1, 0.0);
	vs_inout_fs.fragPos = vs_inout[2].fragPos;
	vs_inout_fs.normal = vs_inout[2].normal;
	EmitVertex();

	gl_Position = gl_in[0].gl_Position;
	vs_inout_fs.fragPos = vs_inout[0].fragPos;
	vs_inout_fs.normal = vs_inout[0].normal;
	EmitVertex();

	gl_Position = gl_in[1].gl_Position;
	vs_inout_fs.fragPos = vs_inout[1].fragPos;
	vs_inout_fs.normal = vs_inout[1].normal;
	EmitVertex();

	gl_Position = gl_in[2].gl_Position;
	vs_inout_fs.fragPos = vs_inout[2].fragPos;
	vs_inout_fs.normal = vs_inout[2].normal;
	EmitVertex();
	
	EndPrimitive();
}
