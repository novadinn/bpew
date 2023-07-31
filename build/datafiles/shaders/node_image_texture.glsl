void node_image_texture(sampler2D tex, vec3 pos, out vec4 color, out float alpha) {
    color = vec4(vec3(texture(tex, pos.xy)), 1.0f);
    alpha = texture(tex, pos.xy).a;
}
