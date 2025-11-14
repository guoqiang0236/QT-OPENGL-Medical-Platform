#include "Object.h"

Object::Object()
{
	m_Type = ObjectType::Object;
}

Object::~Object()
{
}

void Object::setPosition(glm::vec3 pos)
{
	m_Position = pos;
}

void Object::rotateX(float angle)
{
	m_AngleX += angle;
}

void Object::rotateY(float angle)
{
	m_AngleY += angle;
}

void Object::rotateZ(float angle)
{
	m_AngleZ += angle;
}

void Object::setAngleX(float angle)
{
	m_AngleX = angle;
}

void Object::setAngleY(float angle)
{
	m_AngleY = angle;
}

void Object::setAngleZ(float angle)
{
	m_AngleZ = angle;
}

void Object::setScale(glm::vec3 scale)
{
	m_Scale = scale;
	//qDebug() << "setScale:" << m_Scale.x << m_Scale.y << m_Scale.z;
}

glm::mat4 Object::getModelMatrix()const
{
	//首先获取父亲的变换矩阵
	glm::mat4 parentMatrix{ 1.0f };
	if (m_Parent != nullptr)
	{
		parentMatrix = m_Parent->getModelMatrix();
	}


	//unity:缩放，旋转，平移
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::scale(model, m_Scale);//缩放
	model = glm::rotate(model, glm::radians(m_AngleX), glm::vec3(1.0f, 0.0f, 0.0f));//pitch 
	model = glm::rotate(model, glm::radians(m_AngleY), glm::vec3(0.0f, 1.0f, 0.0f));//yaw 
	model = glm::rotate(model, glm::radians(m_AngleZ), glm::vec3(0.0f, 0.0f, 1.0f));//roll


	model = parentMatrix * glm::translate(glm::mat4(1.0f), m_Position) * model;//平移

	return model;
}

void Object::addChild(Object* obj)
{
	//1. 检查是否加入过这个子对象
	auto iter = std::find(m_Children.begin(), m_Children.end(), obj);
	if (iter != m_Children.end())
	{
		return;
	}
	//2. 添加子对象到子对象列表
	m_Children.push_back(obj);

	//3. 设置父对象指针
	obj->m_Parent = this;
}

std::vector<Object*> Object::getChildren()
{
	return m_Children;
}

Object* Object::getParent()
{
	return m_Parent;
}
