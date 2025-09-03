#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

out vec4 finalColor;

uniform vec2 lightPos;
uniform vec2 resolution;
uniform sampler2D lightTexture; // Your render texture

void main()
{
    vec2 flippedTexCoord = vec2(fragTexCoord.x, 1.0 - fragTexCoord.y);
    float lightValue = texture(lightTexture, flippedTexCoord).r;

    // if (lightValue > 0.5) {
    //     finalColor = vec4(0,0,0,0);
    // } else {
    //     finalColor = vec4(0,0,0,1);
    // }

    int screenHeight = 450;
    int screenWidth = 800;
    float ambientDarkness = 0.3;
    
    vec2 fragPos = vec2(gl_FragCoord.x, screenHeight - gl_FragCoord.y);
    float distance = length(fragPos - lightPos);
    
    float innerRadius = 50.0;
    float outerRadius = 150.0;
    
    // Light color (warm orange/yellow tint)
    vec3 lightColor = vec3(1.0, 0.8, 0.4);  // Orange/yellow
    vec3 ambientColor = vec3(0.0, 0.0, 0.0); // Black for dark areas
    
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
    
    finalColor = vec4(finalColorRGB, alpha);


}