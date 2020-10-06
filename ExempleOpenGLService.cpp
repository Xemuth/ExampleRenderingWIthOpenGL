#include <Core/Core.h>
#include <GLFW_test/glfw.h>
#include <UFEngine/UFEngine.h>
#include <pluginUFE/OpenGLRenderer/OpenGLRenderer.h>

GLFWwindow* InitialisationGLFW();
void CheckForGLFW(GLFWwindow*);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

void GLAPIENTRY MessageCallback( GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam ){
   fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),type, severity, message );
}


Upp::UFEContext context;

CONSOLE_APP_MAIN{
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
	
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	
	glEnable ( GL_DEBUG_OUTPUT );
	glDebugMessageCallback( MessageCallback, 0 );
	
	
	Upp::StdLogSetup(Upp::LOG_COUT| Upp::LOG_FILE);
	
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
		
			shader.RegisterUniform("view");
			shader.RegisterUniform("proj");
			shader.RegisterUniform("model");
			
		}catch(Upp::Exc& e){
			Upp::Cout() << e << Upp::EOL;
		}
		
		Upp::MeshData& mesh = openGL.CreateMeshData("carre");
		Upp::Vector<float> normals;
		normals << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0;
		mesh.AddMesh(verticesTriangle,normals,normals,normals); //we only use verticesTriangles for this example
		mesh.Load();
		

		class RotationComponent : public Upp::Component{
			public:
				virtual Upp::String GetName()const{
					return "RotationComponent";
				}
				virtual void Update(double timeEllapsed, double deltaTime = 0.0){
					GetObject().GetTransform().Rotate(deltaTime * 100, glm::vec3(0,1,0));
					
				}
		};
		
		class TranslationComponent : public Upp::Component{
			public:
				virtual Upp::String GetName()const{
					return "TranslationComponent";
				}
				virtual void Update(double timeEllapsed, double deltaTime = 0.0){
					GetObject().GetTransform().SetPosition(glm::vec3(0,glm::cos(timeEllapsed) * 10,glm::sin(timeEllapsed) * 10 ));
				}
		};
		
		class LookAt : public Upp::Component{
			private:
				Upp::Transform* transformPtr = nullptr;
			public:
				
				LookAt& SetTransformToLook(Upp::Transform& transform){transformPtr = &transform; return *this;}
				
				virtual Upp::String GetName()const{
					return "LookAt";
				}
				virtual void Update(double timeEllapsed, double deltaTime = 0.0){
					if(transformPtr){
						GetObject().GetTransform().LookAt((*transformPtr).GetPosition());
					}
				}
		};

		
		Upp::Object& obj = context.GetSceneManager().CreateScene("scene1").GetObjectManager().CreateObject("object1");
		obj.GetComponentManager().CreateComponent<Upp::OpenGLComponentModel>().model = "carre";
		obj.GetComponentManager().CreateComponent<Upp::OpenGLComponentRenderer>().renderer = "basic";
		obj.GetComponentManager().CreateComponent<RotationComponent>();
		obj.GetComponentManager().CreateComponent<TranslationComponent>();

		Upp::Object& camera = context.GetSceneManager().GetActiveScene().GetObjectManager().CreateObject("camera");
		Upp::OpenGLComponentCameraPerspective& theCamera = camera.GetComponentManager().CreateComponent<Upp::OpenGLComponentCameraPerspective>();
		camera.GetTransform().SetPosition(0,0,5);

		//Now we create another camera which will be inactive
		Upp::Object& camera2 = context.GetSceneManager().GetActiveScene().GetObjectManager().CreateObject("camera2");
		camera2.GetComponentManager().CreateComponent<Upp::OpenGLComponentCameraPerspective>(false); //We set it inactive
		camera2.GetComponentManager().CreateComponent<LookAt>().SetTransformToLook(obj.GetTransform());
		//We add graphic to the Camera !
		camera2.GetComponentManager().CreateComponent<Upp::OpenGLComponentModel>().model = "carre";
		camera2.GetComponentManager().CreateComponent<Upp::OpenGLComponentRenderer>().renderer = "basic";
		camera2.GetTransform().SetPosition(20,0,2);
		
		Upp::Object& obj2 = context.GetSceneManager().GetActiveScene().GetObjectManager().CreateObject("object2");
		obj2.GetComponentManager().CreateComponent<Upp::OpenGLComponentModel>().model = "carre";
		obj2.GetComponentManager().CreateComponent<Upp::OpenGLComponentRenderer>().renderer = "basic";
		obj2.GetTransform().Move(2,0,0);
		
		
		//Now we create another camera which will be inactive
		Upp::Object& camera3 = context.GetSceneManager().GetActiveScene().GetObjectManager().CreateObject("camera3");
		camera3.GetComponentManager().CreateComponent<Upp::OpenGLComponentCameraPerspective>(false); //We set it inactive
		camera3.GetTransform().SetPosition(-10,0,0);
		camera3.GetTransform().LookAt(glm::vec3(0,0,0));
		
		DUMP(context);
	
	}catch(Upp::Exc& exc){
		Upp::Cout() << exc << Upp::EOL;
	}
	
	context.TimerStart();

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        try{
			context.Update();
		}catch(Upp::Exc& exception){
			Upp::Cout() << exception << Upp::EOL;
        }
		
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window){
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
    
    for(Upp::Scene& scene : context.GetSceneManager().GetScenes()){
		for(Upp::Object& object : scene.GetObjectManager().GetObjects()){
			object.GetComponentManager().SendMessageToComponent<Upp::OpenGLComponentCamera>("ScreenSize",Upp::ValueArray{width,height});
		}
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	for(Upp::Scene& scene : context.GetSceneManager().GetScenes()){
		for(Upp::Object& object : scene.GetObjectManager().GetObjects()){
			object.GetComponentManager().SendMessageToComponent<Upp::OpenGLComponentCamera>("MouseWheel",yoffset);
		}
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if(button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS){
        //Upp::Vector<Upp::Object*> allCamera = context.GetSceneManager().GetActiveScene().GetObjectManager().GetAllObjectDependingOnComponentOrInherritedComponent<Upp::OpenGLComponentCamera>(false);
        Upp::Vector<Upp::Object*> allCamera = context.GetSceneManager().GetActiveScene().GetObjectManager().GetObjectsDependingOnFunction([](Upp::Object& object, Upp::Scene& scene) -> bool{
			return object.GetComponentManager().HasComponent<Upp::OpenGLComponentCamera>();
        });
        for(int e = 0; e < allCamera.GetCount(); e++){
            Upp::OpenGLComponentCamera& component = allCamera[e]->GetComponentManager().GetComponent<Upp::OpenGLComponentCamera>();
			if(component.IsActive()){
				component.SetActive(false);
				if(allCamera.GetCount() > 0){
					try{
						if(e + 1 < allCamera.GetCount())
							allCamera[e + 1]->GetComponentManager().GetComponent<Upp::OpenGLComponentCamera>().SetActive(true);
						else
							allCamera[0]->GetComponentManager().GetComponent<Upp::OpenGLComponentCamera>().SetActive(true);
					}catch(Upp::Exc& exception){
						Upp::Cout() << exception << Upp::EOL;
					}
				}
				return;
			}
        }
    }else if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
        for(Upp::Object& object : context.GetSceneManager().GetActiveScene().GetObjectManager().GetObjects()){
			if(object.GetComponentManager().HasActiveComponent<Upp::OpenGLComponentCamera>()){
				DUMP(object);
				return;
			}
        }
    }
}


