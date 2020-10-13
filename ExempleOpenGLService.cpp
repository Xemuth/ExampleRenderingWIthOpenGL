#include <Core/Core.h>
#include <GLFW_test/glfw.h>
#include <UFEngine/UFEngine.h>
#include <pluginUFE/OpenGLRenderer/OpenGLRenderer.h>
#include "vertices.h"
#include "Components.h"

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
	glEnable(GL_DEPTH_TEST); 

	Upp::StdLogSetup(Upp::LOG_COUT| Upp::LOG_FILE);
	
	try{
		
		Upp::SGLRenderer& openGL = context.GetServiceManager().CreateService<Upp::SGLRenderer>();
		
		//Both of this function are used for sending data to shader before being draw
		//One will be executed at renderer loading (just before performing draw on every object
		//which requier this renderer. and the other will be call at each object requiring to
		//being draw. Allowing you to set view and projection matrix and for each object set
		//the model matrix. It's a kind of optimisation
		auto populateShaderWhenRenderer = [&](Upp::Renderer& renderer, Upp::CGLCamera& camera) -> void{
			renderer.GetShaderProgram().Bind();
			renderer.GetShaderProgram().UniformMat4("view",camera.GetViewMatrix());
			renderer.GetShaderProgram().UniformMat4("proj",camera.GetProjectionMatrix());
		};
		
		auto populateShaderWhenMeshData = [&](Upp::Renderer& renderer, Upp::CGLCamera& camera , Upp::Object& object) -> void{
			renderer.GetShaderProgram().UniformMat4("model",object.GetTransform().GetModelMatrix());
		};
		
	/*	Upp::Renderer& renderer = openGL.CreateRenderer("basic");
		renderer.beforeRendering = populateShaderWhenRenderer;
		renderer.GenerateVAO(); //We generate the default VAO (the one corresponding to the data we passed the function above)
		Upp::ShaderProgram& shader =  renderer.GetShaderProgram();
		try{
			shader.LoadFromFile("C:\\Upp\\xemuth\\Apps\\ExempleOpenGLService\\vertex.glsl",Upp::ShaderType::VERTEX);
			shader.LoadFromFile("C:\\Upp\\xemuth\\Apps\\ExempleOpenGLService\\fragment.glsl",Upp::ShaderType::FRAGMENT);
			shader.Link();
		
			shader.RegisterUniform("view");
			shader.RegisterUniform("proj");
			shader.RegisterUniform("model");
			
		}catch(Upp::Exc& e){
			Upp::Cout() << e << Upp::EOL;
		}*/
		
		
		Upp::Renderer& rendererTexture = openGL.CreateRenderer("texture");
		rendererTexture.GenerateVAO(); //We generate the default VAO (the one corresponding to the data we passed the function above)
		rendererTexture.SetUniformProjectionMatrixName("proj");
		rendererTexture.SetUniformViewMatrixName("view");
		Upp::ShaderProgram& shaderTexture =  rendererTexture.GetShaderProgram();
		try{
			shaderTexture.LoadFromFile("C:\\Upp\\xemuth\\Apps\\ExempleOpenGLService\\vertex.glsl",Upp::ShaderType::VERTEX);
			shaderTexture.LoadFromFile("C:\\Upp\\xemuth\\Apps\\ExempleOpenGLService\\fragmentTexture.glsl",Upp::ShaderType::FRAGMENT);
			shaderTexture.Link();
		
			shaderTexture.RegisterUniform("view");
			shaderTexture.RegisterUniform("proj");
			shaderTexture.RegisterUniform("model");
			shaderTexture.RegisterUniform("tex");
			shaderTexture.RegisterUniform("tex2");
		}catch(Upp::Exc& e){
			Upp::Cout() << e << Upp::EOL;
		}
		
		
		Upp::MeshData& mesh = openGL.CreateMeshData("triangle");
		Upp::Vector<float> normals, texCoords;
		normals << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0;
		texCoords << 0.0f << 0.0f << 1.0f << 0.0f << 0.5f << 1.0f;
		mesh.AddMesh(verticesTriangle,normals,normals,texCoords); //we only use verticesTriangles for this example
		mesh.Load();
		
		openGL.CreateMeshData("carre").AddMesh(verticesCarre,Upp::Vector<float>{},Upp::Vector<float>{}, texCoordsCarre).Load();
		
		
		
		
		openGL.CreateLoadTexture("wall",Upp::StreamRaster::LoadFileAny(Upp::GetFileDirectory(__FILE__) +   "wall.jpg"));
		openGL.CreateLoadTexture("awesomeFace",Upp::StreamRaster::LoadFileAny(Upp::GetFileDirectory(__FILE__) +   "AwesomeFace.png"));

		context.GetSceneManager().CreateScene("scene1");

	/*	Upp::Object& obj = context.GetSceneManager().GetActiveScene().GetObjectManager().CreateObject("object1");
		obj.GetComponentManager().CreateComponent<Upp::CGLModel>().SetModel("triangle");
		obj.GetComponentManager().CreateComponent<Upp::CGLRenderer>().SetRenderer("basic");
		obj.GetComponentManager().CreateComponent<Upp::CGLRoutineBeforeAfterRendering>().SetBeforeRendering(populateShaderWhenMeshData);
		obj.GetComponentManager().CreateComponent<Upp::TranslationComponent>();
		obj.GetComponentManager().CreateComponent<Upp::RotationComponent>();
		*/

		Upp::Object& camera = context.GetSceneManager().GetActiveScene().GetObjectManager().CreateObject("camera");
		Upp::CGLCameraPerspective& theCamera = camera.GetComponentManager().CreateComponent<Upp::CGLCameraPerspective>();
		camera.GetTransform().SetPosition(0,0,10);
		
	//	camera.GetTransform().SetRotation(-50,glm::vec3(0,1,0));

		//Now we create another camera which will be inactive
		/*Upp::Object& camera2 = context.GetSceneManager().GetActiveScene().GetObjectManager().CreateObject("camera2");
		camera2.GetComponentManager().CreateComponent<Upp::CGLCameraPerspective>(false); //We set it inactive
		camera2.GetComponentManager().CreateComponent<Upp::LookAt>().SetTransformToLook(obj.GetTransform());
		camera2.GetComponentManager().CreateComponent<Upp::CGLModel>().SetModel("triangle");
		camera2.GetComponentManager().CreateComponent<Upp::CGLRenderer>().SetRenderer("basic");
		camera2.GetComponentManager().CreateComponent<Upp::CGLRoutineBeforeAfterRendering>().SetBeforeRendering(populateShaderWhenMeshData);
		camera2.GetTransform().SetPosition(5,0,0);*/
		
		Upp::Object& cube = context.GetSceneManager().GetActiveScene().GetObjectManager().CreateObject("cube");
		cube.GetComponentManager().CreateComponent<Upp::CGLModel>().SetModel("carre");
		cube.GetComponentManager().CreateComponent<Upp::CGLRenderer>().SetRenderer("texture").SetModelMatrixUniformName("model");
		cube.GetComponentManager().CreateComponent<Upp::CGLTexture>().SetTexture("wall").SetChannelToUse(0).SetUniformName("tex");
		cube.GetComponentManager().CreateComponent<Upp::CGLTexture>().SetTexture("awesomeFace").SetChannelToUse(1).SetUniformName("tex2");
		cube.GetComponentManager().CreateComponent<Upp::RotationComponent>();
		
		
	/*	cube.GetComponentManager().CreateComponent<Upp::CGLRoutineBeforeAfterRendering>().SetBeforeRendering(
			[&](Upp::Renderer& renderer, Upp::CGLCamera& camera , Upp::Object& object) -> void{
				renderer.GetShaderProgram().UniformMat4("model",object.GetTransform().GetModelMatrix());
			}
		);*/
				
		/*Upp::Object& skybox = context.GetSceneManager().GetActiveScene().GetObjectManager().CreateObject("SkyBox");
		Upp::CGLSkyBox& sx = skybox.GetComponentManager().CreateComponent<Upp::CGLSkyBox>();
		sx.skyboxRight = Upp::StreamRaster::LoadFileAny("C:\\Upp\\upp\\bazaar\\SurfaceCtrl\\skybox\\right.jpg");
		sx.skyboxLeft = Upp::StreamRaster::LoadFileAny("C:\\Upp\\upp\\bazaar\\SurfaceCtrl\\skybox\\left.jpg");
		sx.skyboxTop = Upp::StreamRaster::LoadFileAny("C:\\Upp\\upp\\bazaar\\SurfaceCtrl\\skybox\\top.jpg");
		sx.skyboxBottom = Upp::StreamRaster::LoadFileAny("C:\\Upp\\upp\\bazaar\\SurfaceCtrl\\skybox\\bottom.jpg");
		sx.skyboxFront = Upp::StreamRaster::LoadFileAny("C:\\Upp\\upp\\bazaar\\SurfaceCtrl\\skybox\\front.jpg");
		sx.skyboxBack = Upp::StreamRaster::LoadFileAny("C:\\Upp\\upp\\bazaar\\SurfaceCtrl\\skybox\\back.jpg");
		sx.LoadSkybox();*/
		
		//Lets now try to add Skybox to another object in the scene and see if it crash
		//obj2.GetComponentManager().CreateComponent<Upp::CGLSkyBox>(); It work well, it raise
		//exception because Skybox already exist
		
		
		DUMP(context);
	}catch(Upp::Exc& exc){
		Upp::Cout() << exc << Upp::EOL;
	}
	
	context.TimerStart();

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
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
    glViewport(0, 0, width, height);
    for(Upp::Scene& scene : context.GetSceneManager().GetScenes()){
		for(Upp::Object& object : scene.GetObjectManager().GetObjects()){
			object.GetComponentManager().SendMessageToComponent<Upp::CGLCamera>("ScreenSize",Upp::ValueArray{width,height});
		}
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	for(Upp::Scene& scene : context.GetSceneManager().GetScenes()){
		for(Upp::Object& object : scene.GetObjectManager().GetObjects()){
			object.GetComponentManager().SendMessageBroadcastOnlyActive("MouseWheel",yoffset);
		}
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if(button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS){
        Upp::Vector<Upp::Object*> allCamera = context.GetSceneManager().GetActiveScene().GetObjectManager().GetObjectsDependingOnFunction([](Upp::Object& object, Upp::Scene& scene) -> bool{
			return object.GetComponentManager().HasComponent<Upp::CGLCamera>();
        });
        for(int e = 0; e < allCamera.GetCount(); e++){
            Upp::CGLCamera& component = allCamera[e]->GetComponentManager().GetComponent<Upp::CGLCamera>();
			if(component.IsActive()){
				component.SetActive(false);
				if(allCamera.GetCount() > 0){
					try{
						if(e + 1 < allCamera.GetCount())
							allCamera[e + 1]->GetComponentManager().GetComponent<Upp::CGLCamera>().SetActive(true);
						else
							allCamera[0]->GetComponentManager().GetComponent<Upp::CGLCamera>().SetActive(true);
					}catch(Upp::Exc& exception){
						Upp::Cout() << exception << Upp::EOL;
					}
				}
				return;
			}
        }
    }else if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
        for(Upp::Object& object : context.GetSceneManager().GetActiveScene().GetObjectManager().GetObjects()){
			if(object.GetComponentManager().HasActiveComponent<Upp::CGLCamera>()){
				DUMP(object);
				return;
			}
        }
    }
}


