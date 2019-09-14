#include "VertexBuffer.h"
#include "Renderer.h"

VertexBuffer::VertexBuffer(const void* data, unsigned size)
{
	GLCall(glGenBuffers(1, &m_RendererID)); //This generates a buffer that the GPU can use to draw to the screen
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID)); //This Selects the buffer that we just created	
	GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW)); //This added the data to the buffer as STATIC meaning that the data won't change but can be called multiple times

}

VertexBuffer::~VertexBuffer()
{
	GLCall(glDeleteBuffers(1, &m_RendererID));
}

void VertexBuffer::Bind() const
{
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
}

void VertexBuffer::UnBind() const
{
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}