#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;

// Input uniform values
uniform sampler2D texture0;
uniform vec2 textureSize;
uniform float outlineSize;
uniform vec4 outlineColor;

// Output fragment color
out vec4 finalColor;

void main()
{
    vec4 texel = texture(texture0, fragTexCoord);   // Get texel color
    vec2 texelScale = vec2(0.0);
    texelScale.x = outlineSize / textureSize.x;
    texelScale.y = outlineSize / textureSize.y;

    // Sample the alpha channel of the texture to determine if it's inside the shape
    float alpha = texel.a;

    // We sample four corner texels, but only for the alpha channel
    vec4 corners = vec4(0.0);
    corners.x = texture(texture0, fragTexCoord + vec2(texelScale.x, texelScale.y)).a;
    corners.y = texture(texture0, fragTexCoord + vec2(texelScale.x, -texelScale.y)).a;
    corners.z = texture(texture0, fragTexCoord + vec2(-texelScale.x, texelScale.y)).a;
    corners.w = texture(texture0, fragTexCoord + vec2(-texelScale.x, -texelScale.y)).a;

    // Determine if the current pixel is on the shape's outline
    float outline = step(alpha, 0.5) * step(0.5, dot(corners, vec4(1.0)));

    // Output either the outline color or transparent if not on the outline
    finalColor = mix(vec4(0.0), outlineColor, outline);
}