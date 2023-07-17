void node_mix(
    int dataType,
    float factor,
    float af,
    float bf,
    vec3 av,
    vec3 bv,
    vec3 ac,
    vec3 bc,
    out float resf,
    out vec3 resv,
    out vec3 resc) {
    resf = mix(af, bf, factor);
    resv = mix(av, bv, factor);
    resc = mix(ac, bc, factor);
}
