#version 450 core
out vec4 FragColor;

in vec2 uv;
in vec3 normal;

uniform sampler2D sampler;

//光源参数
uniform vec3 lightDirection;
uniform vec3 lightColor;


void main()
{
   //1.获取物体的颜色
    vec3 objectcolor =texture(sampler, uv).xyz;

   //2.准备diffuse
   vec3 normalizedNormal = normalize(normal);
   vec3 lightdirN =normalize(lightDirection);

   //3.计算漫反射
   float diffuse = clamp(dot(-lightdirN, normalizedNormal), 0.0, 1.0);
   vec3 finalcolor =lightColor  * diffuse * objectcolor;

   FragColor = vec4(finalcolor, 1.0);
}

//void main()
//{
//    vec3 normalizedNormal = normalize(normal);
//	vec3 normalcolor =clamp(normalizedNormal , 0.0, 1.0);
//
//	FragColor = vec4(normalcolor, 1.0);
//	//FragColor = texture(sampler, uv);
//}