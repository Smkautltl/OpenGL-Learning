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
#include "Texture.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"


int main(void)
{
    GLFWwindow* window;
	float ViewWidth = 1280.f;
	float ViewHeight = 720.f;
	
    /* Initialize the library */
    if (!glfwInit())
        return -1;

	//This sets the OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(ViewWidth, ViewHeight, "Hello World", NULL, NULL);
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
		 100.f,  100.f,  0.0f, 0.0f,
		 200.0f, 100.0f, 1.0f, 0.0f,
		 200.f,  200.f,  1.0f, 1.0f,
		 100.0f, 200.f,  0.0f, 1.0f
	};
		unsigned int indices[] = {
		0,1,2,
		2,3,0
	};
    	
		//This enables blending
    	GLCall(glEnable(GL_BLEND));
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

		//This code generates and binds a vertex array object and vertex buffer
    	VertexArray va;
		VertexBuffer vb(Positions, 4 * 4 * sizeof(float));
    	
    	VertexBufferLayout layout;
    	layout.Push<float>(2);
    	layout.Push<float>(2);
    	va.AddBuffer(vb, layout);

    	//Sets up the index buffer 
		IndexBuffer ib(indices, 6);

    	//Sets up a Orthographic projection
		glm::mat4 proj = glm::ortho(0.f, ViewWidth, 0.f, ViewHeight,-1.0f,1.0f);
		glm::mat4 view = glm::translate(glm::mat4(1.f), glm::vec3(-100, 0 ,0));
    	glm::mat4 model = glm::translate(glm::mat4(1.f), glm::vec3(200,200,0));

		glm::mat4 mvp = proj * view * model;
    	
    	//Sets up the shader that will be used
		Shader shader("res/shaders/Basic.shader");
    	shader.Bind();
    	shader.SetUniform4f("u_Colour", 0.7f, 0.3f, 0.5f, 1.0f);
    	shader.SetUniformMat4f("u_MVP", mvp);

		Texture texture("res/textures/marble.png");
		texture.Bind();
    	shader.SetUniform1i("u_Texture", 0);
    	
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