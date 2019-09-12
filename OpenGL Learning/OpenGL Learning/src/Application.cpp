#include <glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

static unsigned int CompileShader(unsigned int type, const std::string& source)
{
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)alloca(sizeof(char) * length);
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "Fragment") <<"shader!" << std::endl;
		std::cout << message << std::endl;

		glDeleteShader(id);
		return 0;
	}
	
	return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;
	
    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(800, 800, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

	if(glewInit() != GLEW_OK)
	{
		std::cout << "Glew Error!" << std::endl;
	}
	std::cout << glGetString(GL_VERSION) << std::endl;

	float Positions[6] = {
		-0.5f, -0.5f,
		 0.f,   0.5f,
		 0.5f, -0.5f
	};
	
	unsigned int buffer;
	glGenBuffers(1, &buffer); //This generates a buffer that the GPU can use to draw to the screen
	glBindBuffer(GL_ARRAY_BUFFER, buffer); //This Selects the buffer that we just created	
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), Positions, GL_STATIC_DRAW); //This added the Positions data to the buffer as STATIC meaning that the data won't change but can be called multiple times

	glEnableVertexAttribArray(0); //Enables the Vertex attributes array
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0); //This says that the Vertices are 2 floats for each vertex

	std::string vertexShader =
		"#version 330 core\n"
		"\n"
		"layout(location = 0) in vec4 position;\n"
		"\n"
		"void main()\n"
		"{\n"
		"	gl_Position = position;\n"
		"}\n";

	std::string fragmentShader =
		"#version 330 core\n"
		"\n"
		"layout(location = 0) out vec4 colour;\n"
		"\n"
		"void main()\n"
		"{\n"
		"	colour = vec4(1.0, 0.0, 0.0, 1.0);"
		"}\n";
	
	unsigned int shader = CreateShader(vertexShader, fragmentShader);
	glUseProgram(shader);
	
	//glBindBuffer(GL_ARRAY_BUFFER, 0); //This unbinds the buffer
	
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

		glDrawArrays(GL_TRIANGLES, 0, 3);
    	
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

	glDeleteProgram(shader);
	
    glfwTerminate();
	return 0;
}
