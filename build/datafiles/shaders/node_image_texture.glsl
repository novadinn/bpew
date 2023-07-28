void node_image_texture(sampler2D tex, vec2 pos, out vec3 color, out float alpha) {
    color = vec3(texture(tex, pos));
    alpha = texture(tex, pos).a;
}
