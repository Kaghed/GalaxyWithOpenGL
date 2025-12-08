in vec2 TexCoord;
uniform sampler2D planetTexture;
out vec4 FragColor;

void main() {
    FragColor = texture(planetTexture, TexCoord);
}
