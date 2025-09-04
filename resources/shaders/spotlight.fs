#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

out vec4 finalColor;

uniform vec2 lightPos;
uniform vec2 resolution;
uniform sampler2D lightTexture; // Your render texture
uniform float time;

// Simple pseudo-random hash based on position and time
float rand(vec2 co, float t)
{
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233)) + t*100.0) * 43758.5453);
}

void main()
{
    vec2 flippedTexCoord = vec2(fragTexCoord.x, 1.0 - fragTexCoord.y);
    float lightValue = texture(lightTexture, flippedTexCoord).r;

    // if (lightValue > 0.5) {
    //     finalColor = vec4(0,0,0,0);
    // } else {
    //     finalColor = vec4(0,0,0,1);
    // }

    int screenHeight = 800;
    int screenWidth = 800;
    float ambientDarkness = 0.7;
    
    vec2 fragPos = vec2(gl_FragCoord.x, screenHeight - gl_FragCoord.y);
    
    // float distance = length(fragPos - lightPos);
    // light oval
    vec2 ovalScale = vec2(1, 0.7); // X stays the same, Y is stretched
    vec2 ovalDelta = (fragPos - lightPos) / ovalScale;
    float distance = length(ovalDelta);
    
    // float flicker = rand(lightPos, time) * 5.0;
    // float outerRadius = 250.0+flicker;
    float innerRadius = 5.0;
    float outerRadius = 250.0;
    
    // Light color (warm orange/yellow tint)
    vec3 lightColor = vec3(1.0, 0.8, 0.4);  // Orange/yellow
    vec3 ambientColor = vec3(0.1, 0.1, 0.2); // Black for dark areas
    
    float lightIntensity;
    vec3 finalColorRGB;
    float alpha;
    
    if (lightValue > 0.5 && distance <= innerRadius)
    {
        // Inside inner radius - full light color
        lightIntensity = 1.0;
        finalColorRGB = lightColor;
        alpha = 0.3; // Some transparency to blend with background
    }
    else if ( lightValue > 0.5 && distance <= outerRadius)
    {
        // Fade from light to ambient
        float fadeAmount = (distance - innerRadius) / (outerRadius - innerRadius);
        lightIntensity = 1.0 - fadeAmount;
        finalColorRGB = mix(lightColor, ambientColor, fadeAmount);
        alpha = mix(0.3, ambientDarkness, fadeAmount);
    }
    else
    {
        // Outside light - dark ambient
        lightIntensity = 0.0;
        finalColorRGB = ambientColor;
        alpha = ambientDarkness;
    }

    // Film grain
    // float grain = rand(fragTexCoord * resolution * 3 +time, time) * 0.03 - 0.02;
    // finalColorRGB += grain;
    // finalColorRGB = clamp(finalColorRGB, 0.0, 1.0);
    // // scan lines
    // float scanline = sin((gl_FragCoord.y + time * 60.0) * 0.25) * 0.05; // 0.25 controls frequency, 0.1 controls strength
    // finalColorRGB -= scanline * 0.1; // Subtract for dark lines, add for bright lines
    // finalColorRGB = clamp(finalColorRGB, 0.0, 1.0);

    finalColor = vec4(finalColorRGB, alpha);


}