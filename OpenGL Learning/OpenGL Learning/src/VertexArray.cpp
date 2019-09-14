#include "VertexArray.h"
#include "Renderer.h"

VertexArray::VertexArray()
{
	GLCall(glGenVertexArrays(1, &m_RendererID));
}

VertexArray::~VertexArray()
{
	GLCall(glDeleteVertexArrays(1, &m_RendererID));
}

void VertexArray::AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout)
{
	Bind();
	vb.Bind();
	const auto& elements = layout.GetElements();
	unsigned int offset = 0;
	for(unsigned int i = 0; i< elements.size(); i++)
	{
		const auto& element = elements[i];
		GLCall(glEnableVertexAttribArray(i)); //Enables the Vertex attributes array
		GLCall(glVertexAttribPointer(i, element.count, element.type, element.normalized, layout.GetStride(), (const void*)offset)); //This says that the Vertices are 2 floats for each vertex

		offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
		
	}	
}

void VertexArray::Bind() const
{
	GLCall(glBindVertexArray(m_RendererID))
}

void VertexArray::UnBind() const
{
		GLCall(glBindVertexArray(0))
}
