void main() {
    float mult = (sign(currentEntityID) + 1) * 0.5f;
    int hit = int(float(currentEntityID == selectedEntityID) *
		  float(vs_inout.vertexID == selectedVertexID) *
		  mult);

    /* round edges */
    vec2 centered = gl_PointCoord - vec2(0.5);
    float dist_squared = dot(centered, centered);
    const float rad_squared = 0.25;
    if (dist_squared > rad_squared) {
	discard;
	return;
    }

    fragColor = mix(vec4(color, 1.0), vec4(selectionColor, 1.0), mixFactor * hit);
}
