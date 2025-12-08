#version 460 core
out vec4 FragColor;

in vec3 fragPos;
in vec3 fragNormal;
in vec2 texCoord;

uniform vec3 objectColor;
uniform sampler2D texture_diffuse1;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform bool blinn;

void main()
{
    //color
    vec3 color = texture(texture_diffuse1, texCoord).rgb * vec3(objectColor);
    //ambient
    vec3 ambient = 0.1 * color;
    //diffuse
    vec3 lightDir = normalize(lightPos - fragPos);
    vec3 normal = normalize(fragNormal);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;
    //specular
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    if(blinn)
    {
        vec3 halfwayDir = normalize(lightDir + viewDir);  
        spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    }
    else
    {
        vec3 reflectDir = reflect(-lightDir, normal);
        spec = pow(max(dot(viewDir, reflectDir), 0.0), 8.0);
    }
    vec3 specular = vec3(0.5) * spec; // assuming bright white light color
    
    FragColor = vec4(ambient + diffuse + specular, 1.0);
}