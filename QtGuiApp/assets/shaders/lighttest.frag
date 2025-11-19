#version 450 core
out vec4 FragColor;

in vec2 uv;
in vec3 normal;
in vec3 worldPosition;

uniform sampler2D sampler;

//光源参数
uniform vec3 lightDirection;
uniform vec3 lightColor;

uniform vec3 ambientColor;
//相机世界位置
uniform vec3 cameraPosition;

uniform float specularIntensity;

vec3 calculateDiffuse(vec3 lightDirN, vec3 lightCol, vec3 normalizedNormal, vec3 objectCol)
{
    // 计算漫反射强度 (Lambert 模型)
    float diffuseIntensity = clamp(dot(-lightDirN, normalizedNormal), 0.0, 1.0);
    
    // 返回最终的漫反射颜色
    return lightCol * diffuseIntensity * objectCol;
}

vec3 calculateSpecualColor(vec3 lightDirN, vec3 lightCol, vec3 normalizedNormal, vec3 viewDirN)
{  
    //防止背面光照
    float dotresult = dot(-lightDirN, normalizedNormal);
    float flag =step(0.0, dotresult);

     // 计算反射光线方向
    vec3 lightreflect = normalize(reflect(lightDirN, normalizedNormal));
    
    // 计算镜面反射强度
    float specular = clamp(dot(lightreflect, -viewDirN), 0.0, 1.0);

    //控制光斑大小
    specular = pow(specular, 64.0);

    return lightCol*specular*flag*specularIntensity;
}
void main()
{

    vec3 normalizedNormal = normalize(normal);
    vec3 lightDirN = normalize(lightDirection);
    vec3 viewDirN = normalize(worldPosition - cameraPosition);
   //1.获取物体的颜色
    vec3 objectcolor =texture(sampler, uv).xyz;
   // 2. 计算漫反射光照
    vec3 diffuseColor = calculateDiffuse(lightDirN, lightColor, normalizedNormal, objectcolor);
   
   //3.计算高光反射
   vec3 specularColor = calculateSpecualColor(lightDirN, lightColor, normalizedNormal, viewDirN);
 
   vec3 ambientColor = objectcolor * ambientColor;
   vec3 finalcolor = diffuseColor + specularColor+ambientColor;
   FragColor = vec4(finalcolor, 1.0);
}

