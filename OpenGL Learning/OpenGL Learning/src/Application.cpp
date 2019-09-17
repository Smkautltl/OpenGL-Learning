#include <iostream>
#include <fstream>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "shader.h"
#include "Texture.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <imgui/imgui.h>
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_glfw.h"

#include <glew.h>
#include <GLFW/glfw3.h>


int main(void)
{
    GLFWwindow* window;
	float ViewWidth = 1280.f;
	float ViewHeight = 720.f;
	
    /* Initialize the library */
    if (!glfwInit())
        return -1;

	//This sets the OpenGL version
	const char* glsl_version = "#version 130";
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
    	
    	//Sets up the shader that will be used
		Shader shader("res/shaders/Basic.shader");
    	shader.Bind();
    	shader.SetUniform4f("u_Colour", 0.7f, 0.3f, 0.5f, 1.0f);
    	

		Texture texture("res/textures/marble.png");
		texture.Bind();
    	shader.SetUniform1i("u_Texture", 0);
    	
		//These unbinds the buffers
		va.UnBind();
    	shader.UnBind();
		vb.UnBind();
		ib.UnBind();

    	Renderer renderer;

    	//Sets up imgui
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		
		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsClassic();

		// Setup Platform/Renderer bindings
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init(glsl_version);
    	
		bool show_demo_window = true;
		bool show_another_window = false;
		ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

		glm::vec3 translation(200.f,200.f,0.f);
    	
		float r = 0.0f;
		float increment = 0.05f;
		/* Loop until the user closes the window */
		while (!glfwWindowShouldClose(window))
		{
		    /* Render here */
			renderer.Clear();

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			glm::mat4 model = glm::translate(glm::mat4(1.f), translation);
			glm::mat4 mvp = proj * view * model;
				
			shader.Bind();
			shader.SetUniformMat4f("u_MVP", mvp);
			
			renderer.Draw(va, ib, shader);
			
			if (r > 1.0f)
				increment = -0.01f;
			else if (r < 0.0f)
				increment = 0.01f;
			r+=increment;

			//imgui window setup
			{
				ImGui::ShowDemoWindow(&show_demo_window);

				static float f = 0.0f;
				static int counter = 0;

				ImGui::Begin("Debug Tools");                    
				ImGui::SliderFloat3("Translation", &translation.x, 0.f, ViewWidth);

				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
				ImGui::End();
				
				// Rendering
				ImGui::Render();
				ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			}
			
			
		    /* Swap front and back buffers */
		    GLCall(glfwSwapBuffers(window));

		    /* Poll for and process events */
		    GLCall(glfwPollEvents());
			
		}
	}

	//Shuts down imgui
	ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	
    glfwTerminate();
	return 0;
}
