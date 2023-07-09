void node_image_texture(in sampler2D tex, in vec2 pos, out vec4 color, out float alpha) {
    color = vec4(texture(tex, pos));
    alpha = texture(tex, pos).a;
}
