#include <Core/Core.h>
#include <GLFW_test/glfw.h>
#include <UFEngine/UFEngine.h>
#include <pluginUFE/OpenGLRenderer/OpenGLRenderer.h>

GLFWwindow* InitialisationGLFW();
void CheckForGLFW(GLFWwindow*);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

void GLAPIENTRY MessageCallback( GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam ){
   fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),type, severity, message );
}


Upp::UFEContext context;

CONSOLE_APP_MAIN
{
	// glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "ExempleOpenGLService", NULL, NULL);
    if (window == NULL)
    {
        Upp::Cout() << "Failed to create GLFW window" << Upp::EOL;
        glfwTerminate();
        Upp::Exit(-1);
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    // glew: load all OpenGL function pointers
    // ---------------------------------------
    glewExperimental=true;
	if (glewInit() != GLEW_OK) {
	    fprintf(stderr, "Failed to initialize GLEW\n");
	    Upp::Exit(-1);
	}
	
	glEnable ( GL_DEBUG_OUTPUT );
	glDebugMessageCallback( MessageCallback, 0 );
	
	//A simple triangle
	Upp::Vector<float> verticesTriangle{
        -0.5f, -0.5f, 0.0f, // left
         0.5f, -0.5f, 0.0f, // right
         0.0f,  0.5f, 0.0f  // top
    };

	try{
		
		Upp::OpenGLRendererService& openGL = context.GetServiceManager().CreateService<Upp::OpenGLRendererService>();
		
		Upp::Renderer& renderer = openGL.CreateRenderer("basic");
		renderer.GenerateVAO(); //We generate the default VAO (the one corresponding to the data we passed the function above)
		Upp::ShaderProgram& shader =  renderer.GetShaderProgram();
		try{
			shader.LoadFromFile("C:\\Upp\\UFEngine\\ExempleOpenGLService\\vertex.glsl",Upp::ShaderType::VERTEX);
			shader.LoadFromFile("C:\\Upp\\UFEngine\\ExempleOpenGLService\\fragment.glsl",Upp::ShaderType::FRAGMENT);
			shader.Link();
		}catch(Upp::Exc& e){
			Upp::Cout() << e << Upp::EOL;
		}
		
		Upp::MeshData& mesh = openGL.CreateMeshData("carre");
		Upp::Vector<float> normals;
		normals << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0;
		mesh.AddMesh(verticesTriangle,normals,normals,normals); //we only use verticesTriangles for this example
		mesh.Load();
		

		
		Upp::Object& obj = context.GetSceneManager().CreateScene("scene1").GetObjectManager().CreateObject("object1");
		obj.GetComponentManager().CreateComponent<Upp::OpenGLComponentModel>().model = "carre";
		obj.GetComponentManager().CreateComponent<Upp::OpenGLComponentRenderer>().renderer = "basic";
	
	}catch(Upp::Exc& exc){
		Upp::Cout() << exc << Upp::EOL;
	}
	

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        context.Update();
		
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}




