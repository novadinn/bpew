void main() {
    /* round edges */
    vec2 centered = gl_PointCoord - vec2(0.5);
    float distSquared = dot(centered, centered);
    const float radSquared = 0.25;
    if (distSquared > radSquared) {
	discard;
	return;
    }

    fragColor = vec4(color, 1.0);
    vertexID = vec4(float(currentEntityID), float(vs_inout.vertexID), 0.0f, 0.0f);
}
