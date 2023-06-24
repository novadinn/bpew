void main() {
    gl_Position = projection * view * vec4(aPos, 1.0f);
    color = aColor;
}
