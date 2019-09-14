#include <glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
		x;\
		ASSERT(GLLogCall(#x, __FILE__, __LINE__))

static void GLClearError()
{
	while(glGetError() != GL_NO_ERROR)
	{
		
	}
}

static bool GLLogCall(const char* function, const char* file, int line)
{
	while(GLenum error = glGetError())
	{
		std::cout << "[OpenGL Error]"
				  << "\n("<< error <<")\n"
				  << function << " " << file << ":" << line
				  <<  std::endl;
		
		return false;
	}
	return true;
}

struct ShaderProgramSource
{
	std::string VertexSource;
	std::string FragmentSource;
};
static ShaderProgramSource ParseShader(const std::string& filepath)
{
	std::ifstream stream(filepath);

	enum class ShaderType
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};
	
	std::string line;
	std::stringstream ss[2];
	ShaderType type = ShaderType::NONE;
	
	while (getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos) //Checks to see if it can find #shader in line
		{
			if (line.find("vertex") != std::string::npos)
				type = ShaderType::VERTEX;			
			else if (line.find("fragment") != std::string::npos)
				type = ShaderType::FRAGMENT;
			
		}
		else
		{
			ss[(int)type] << line << "\n";	
		}
	}

	return {ss[0].str(), ss[1].str()};
}

static unsigned int CompileShader(unsigned int type, const std::string& source)
{
	GLCall(unsigned int id = glCreateShader(type));
	const char* src = source.c_str();
	GLCall(glShaderSource(id, 1, &src, nullptr));
	GLCall(glCompileShader(id));
	
	int result;
	GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));

	//If the result comes back false then an error will be thrown
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
	GLCall(unsigned int program = glCreateProgram());
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	GLCall(glAttachShader(program, vs));
	GLCall(glAttachShader(program, fs));
	GLCall(glLinkProgram(program));
	GLCall(glValidateProgram(program));

	GLCall(glDeleteShader(vs));
	GLCall(glDeleteShader(fs));

	return program;
}

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
	
	if(glewInit() != GLEW_OK)
	{
		std::cout << "Glew Error!" << std::endl;
	}
	std::cout << glGetString(GL_VERSION) << std::endl;

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

	//This code generates and binds a vertex array object
	unsigned int vao;
	GLCall(glGenVertexArrays(1, &vao));
	GLCall(glBindVertexArray(vao));
	
	unsigned int buffer;
	GLCall(glGenBuffers(1, &buffer)); //This generates a buffer that the GPU can use to draw to the screen
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer)); //This Selects the buffer that we just created	
	GLCall(glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(float), Positions, GL_STATIC_DRAW)); //This added the Positions data to the buffer as STATIC meaning that the data won't change but can be called multiple times

	GLCall(glEnableVertexAttribArray(0)); //Enables the Vertex attributes array
	GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0)); //This says that the Vertices are 2 floats for each vertex

	unsigned int ibo; //Index buffer object
	GLCall(glGenBuffers(1, &ibo)); //This generates a buffer that the GPU can use to draw to the screen
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo)); //This Selects the buffer that we just created	
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW));

	ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
	unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
	GLCall(glUseProgram(shader));

	int location = glGetUniformLocation(shader, "u_Colour");
	ASSERT(location != -1);
	GLCall(glUniform4f(location, 0.7f, 0.3f, 0.5f, 1.0f));

	//These unbinds the buffers
	GLCall(glUseProgram(0));
	GLCall(glBindVertexArray(0))
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0)); 
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));	
	
	float r = 0.0f;
	float increment = 0.05f;
	
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        GLCall(glClear(GL_COLOR_BUFFER_BIT));

		GLCall(glUseProgram(shader));
		GLCall(glUniform4f(location, r, 0.3f, 0.5f, 1.0f));
    	
    	GLCall(glBindVertexArray(vao));
    	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo)); //This binds the element array buffer	

		GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));	

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

	glDeleteProgram(shader);
	
    glfwTerminate();
	return 0;
}
