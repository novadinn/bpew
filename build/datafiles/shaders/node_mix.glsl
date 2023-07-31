void node_mix(
    int dataType,
    float factor,
    float af,
    float bf,
    vec3 av,
    vec3 bv,
    vec4 ac,
    vec4 bc,
    out float resf,
    out vec3 resv,
    out vec4 resc) {
    resf = mix(af, bf, factor);
    resv = mix(av, bv, factor);
    resc = mix(ac, bc, factor);
}
