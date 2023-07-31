#include "lights_lib.glsl"

void node_principled_bsdf(
    vec4 base_color,
    float alpha,
    out vec4 color) {
    color = vec4(base_color.rgb, alpha);    
}
