#version 450 core

in vec2 uv;
out vec4 FragColor;

uniform sampler2D dicomTexture;

// ✅ 接收实际像素值范围
uniform float minPixelValue;
uniform float maxPixelValue;

void main()
{
    // 1. 读取归一化的灰度值 [0.0, 1.0]
    float normalizedValue = texture(dicomTexture, uv).r;
    
    // 2. 转换回原始像素值范围 [0, 65535]
    float rawValue = normalizedValue * 65535.0;
    
    // 3. ✅ 对比度拉伸：将 [minPixelValue, maxPixelValue] 映射到 [0.0, 1.0]
    // 对于您的数据：[32768, 34332] -> [0.0, 1.0]
    float range = maxPixelValue - minPixelValue;
    float stretchedValue = (rawValue - minPixelValue) / range;
    stretchedValue = clamp(stretchedValue, 0.0, 1.0);
    
    // 4. 输出灰度图像
    FragColor = vec4(stretchedValue, stretchedValue, stretchedValue, 1.0);
}