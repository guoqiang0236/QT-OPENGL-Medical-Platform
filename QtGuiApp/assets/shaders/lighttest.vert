#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;
layout (location = 2) in vec3 aNormal;

out vec2 uv;
out vec3 normal;
out vec3 worldPosition;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;


void main()
{
	// 1. 将顶点位置转换为齐次坐标 (添加 w=1.0 分量)
	vec4 transformPosition = vec4(aPos, 1.0);
	
	// 2. 模型变换: 将顶点从局部空间变换到世界空间
	transformPosition = modelMatrix * transformPosition;
	
	// 3. 保存世界空间坐标 (用于片段着色器中的光照计算)
	worldPosition = transformPosition.xyz;
	
	// 4. 完整的 MVP 变换: 世界空间 -> 观察空间 -> 裁剪空间
	//    gl_Position 是顶点着色器的最终输出位置
	gl_Position = projectionMatrix * viewMatrix * transformPosition;
	
	// 5. 直接传递 UV 纹理坐标到片段着色器
	uv = aUV;
	
	// 6. 传递法线向量到片段着色器 (注意: 这里未进行法线变换,可能需要改进)
	normal = aNormal;
}