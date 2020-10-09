#ifndef _ExempleOpenGLService_Components_h_
#define _ExempleOpenGLService_Components_h_

#include <Core/Core.h>
#include <UFEngine/UFEngine.h>

namespace Upp{
	class RotationComponent : public Upp::Component{
		public:
			virtual Upp::String GetName()const{
				return "RotationComponent";
			}
			virtual void Update(double timeEllapsed, double deltaTime = 0.0){
				GetObject().GetTransform().Rotate(deltaTime * 50, glm::vec3(0,1,0));
				
			}
	};
	
	class TranslationComponent : public Upp::Component{
		public:
			virtual Upp::String GetName()const{
				return "TranslationComponent";
			}
			virtual void Update(double timeEllapsed, double deltaTime = 0.0){
				GetObject().GetTransform().SetPosition(glm::vec3(0,glm::cos(timeEllapsed) * 10,0));
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
					GetObject().GetTransform().LookAt((*transformPtr).GetPosition(),GetObject().GetTransform().GetWorldUp());
				}
			}
	};
}
#endif
