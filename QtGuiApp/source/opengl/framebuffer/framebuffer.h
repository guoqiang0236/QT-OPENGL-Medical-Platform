#pragma once 
#include "../../pch.h"
#include "../Texture.h"

class Framebuffer:protected QOpenGLFunctions_4_5_Core
{
public:
	Framebuffer(unsigned int width, unsigned int height);
	~Framebuffer();

	unsigned int mWidth{ 0 };
	unsigned int mHeight{ 0 };
	unsigned int mFBO{ 0 };
	Texture* mColorAttachment{ nullptr };
	Texture* mDepthStencilAttachment{ nullptr };

private:

};
