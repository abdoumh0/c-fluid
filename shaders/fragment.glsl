#version 330 core

out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D texture;
uniform vec2 u_resolution;

void main() {
    vec2 uv = gl_FragCoord.xy / u_resolution.xy;
    float densityValue = texture(texture, TexCoord).r;
    FragColor = vec4(densityValue * uv.y, densityValue * uv.x, densityValue, 1.0);
};
