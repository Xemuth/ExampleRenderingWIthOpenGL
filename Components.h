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
			virtual String ToString()const{
				String toString;
				toString << "Object name: " << GetObject().GetName() << "\n";
				toString << "Active: " << IsActive() << "\n";
				toString << "Purpose: This Component allow object to rotate clockwise\n";
				return toString;
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
			virtual String ToString()const{
				String toString;
				toString << "Object name: " << GetObject().GetName() << "\n";
				toString << "Active: " << IsActive() << "\n";
				toString << "Purpose: This Component allow object to move upward and downward depending on time (simple cos(timeEllapsed))\n";
				return toString;
			}
	};
	
	class LookAt : public Upp::Component{
		private:
			Upp::Object* obj = nullptr;
		public:
			
			LookAt& SetObjectToLook(Object& _obj){
				if(_obj.GetScene().GetName().IsEqual(GetObject().GetScene().GetName())){
					obj = &_obj;
				}else{
					LOG("LookAt component exception: Object to focus is not on the same scene ! Parallel universe can't look at each other since they don't know about himself !");
				}
				return *this;
			}
			
			virtual Upp::String GetName()const{
				return "LookAt";
			}
			
			virtual String ToString()const{
				String toString;
				toString << "Object name: " << GetObject().GetName() << "\n";
				toString << "Active: " << IsActive() << "\n";
				if(obj){
					toString << "Actual focus: object named '" << obj->GetName() << "'\n";
				}else{
					toString <<"Actual focus: No object is actually focused";
				}
				toString << "Purpose: This Component allow object to look at another Transform object permatly\n";
				return toString;
			}

			virtual void Update(double timeEllapsed, double deltaTime = 0.0){
				if(obj){
					GetObject().GetTransform().LookAt((*obj).GetTransform().GetPosition(),GetObject().GetTransform().GetWorldUp());
				}
			}
	};
}
#endif
