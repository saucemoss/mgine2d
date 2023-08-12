#version 330

in vec2 fragTexCoord;
uniform float scale;
uniform vec2 offset;
out vec4 finalColor;

vec2 random2(vec2 p) {
    return fract(sin(vec2(dot(p, vec2(127.1, 311.7)), dot(p, vec2(269.5, 183.3)))) * 43758.5453);
}

float gradientNoise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    
    float a = dot(random2(i), f - vec2(0.0, 0.0));
    float b = dot(random2(i + vec2(1.0, 0.0)), f - vec2(1.0, 0.0));
    float c = dot(random2(i + vec2(0.0, 1.0)), f - vec2(0.0, 1.0));
    float d = dot(random2(i + vec2(1.0, 1.0)), f - vec2(1.0, 1.0));
    
    vec2 u = f * f * (3.0 - 2.0 * f);
    return mix(mix(a, b, u.x), mix(c, d, u.x), u.y);
}

float layeredNoise(vec2 p) {
    float totalNoise = 0.5;
    float layerScale = 2.0;
    float layerStrength = 0.7; // Controls the contribution of each layer
    
    for (int i = 0; i < 10; i++) { // You can adjust the number of layers
        totalNoise += gradientNoise(p * layerScale) * layerStrength;
        layerScale *= 1.5; // Increase the scale for each layer
        layerStrength *= 0.65; // Reduce the strength for each layer
    }
    
    return totalNoise;
}

void main() {
    vec2 uv = (fragTexCoord + offset) * 4.0f;
    float noiseValue = layeredNoise(uv);
    
    // Set transparency and color based on noise value
    float alpha = 1.0; // black background
    vec3 color = vec3(noiseValue); // Grayscale color transitioning to white
    
    finalColor = vec4(color, alpha);
}
