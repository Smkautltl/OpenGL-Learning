#include <glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "shader.h"


int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

	//This sets the OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(800, 800, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	//This sets up GLEW 
	if(glewInit() != GLEW_OK)
	{
		std::cout << "Glew Error!" << std::endl;
	}
	std::cout << glGetString(GL_VERSION) << std::endl;

	//This Scope to fix the application not terminating properly
	{
    	//Magic number values to have a cube be rendered to the screen
		float Positions[] = {
		-0.5f, -0.5f,
		 0.5f, -0.5f,
		 0.5f,  0.5f,
		-0.5f,  0.5f,
	};
		unsigned int indices[] = {
		0,1,2,
		2,3,0
	};

		//This code generates and binds a vertex array object and vertex buffer
    	VertexArray va;
		VertexBuffer vb(Positions, 4 * 2 * sizeof(float));
    	VertexBufferLayout layout;
    	layout.Push<float>(2);
    	va.AddBuffer(vb, layout);

    	//Sets up the index buffer 
		IndexBuffer ib(indices, 6);

    	//Sets up the shader that will be used
		Shader shader("res/shaders/Basic.shader");
    	shader.Bind();
    	shader.SetUniform4f("u_Colour", 0.7f, 0.3f, 0.5f, 1.0f);

		//These unbinds the buffers
		va.UnBind();
    	shader.UnBind();
		vb.UnBind();
		ib.UnBind();

    	Renderer renderer;
		
		float r = 0.0f;
		float increment = 0.05f;
		/* Loop until the user closes the window */
		while (!glfwWindowShouldClose(window))
		{
		    /* Render here */
			renderer.Clear();
			shader.Bind();
			shader.SetUniform4f("u_Colour", r, 0.3f, 0.5f, 1.0f);
		
			renderer.Draw(va, ib, shader);
			
			if (r > 1.0f)
				increment = -0.01f;
			else if (r < 0.0f)
				increment = 0.01f;

			r+=increment;
			
		    /* Swap front and back buffers */
		    GLCall(glfwSwapBuffers(window));

		    /* Poll for and process events */
		    GLCall(glfwPollEvents());
			
		}
	}
	
    glfwTerminate();
	return 0;
}