#version 330

in vec2 fragTexCoord;
uniform sampler2D textureSampler; // Input texture
uniform float darknessFactor; // Darkening factor
out vec4 finalColor;

void main() {
    // Sample the texture
    vec4 texColor = texture(textureSampler, fragTexCoord);
    
    // If the alpha value is not fully transparent
    if (texColor.a > 0.0) {
        // Darken the color
        texColor.rgb *= darknessFactor;
    }
    
    finalColor = texColor;
}