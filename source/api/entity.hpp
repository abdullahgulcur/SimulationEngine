#pragma once

#include <iostream>

#include "transform.hpp"
#include "component.hpp"
#include "collider.hpp"

class Scene;

class Entity {

private:

	void deepCopyComponents(std::vector<Component*> components, Scene& scene);

public:

	char* name;
	Transform* transform;
	std::vector<Component*> components;

	Entity(const char* name, std::vector<Entity>& entities);

	Entity(Entity* ent, Transform* parent, Scene& scene);

	Entity(const char* name, Entity* parent, Scene& scene);

	~Entity();

	template <class T>
	T* addComponent() {

		T* comp = getComponent<T>();

		if (comp != nullptr) {
			
			Collider* colliderComp = dynamic_cast<Collider*>(comp);

			if (!colliderComp) {

				std::cout << "There is existing component in the same type!" << std::endl;
				return nullptr;
			}
		}

		T* newcomp = new T;
		components.push_back(newcomp);

		return newcomp;
	}

	template <class T>
	T* getComponent() {
	
		for (auto& it : components) {
	
			T* comp = dynamic_cast<T*>(it);

			if (comp != nullptr)
				return comp;
		}
		return nullptr;
	}

	template <class T>
	std::vector<T*> getComponents() {

		std::vector<T*> compList;

		for (auto& it : components) {

			T* comp = dynamic_cast<T*>(it);

			if (comp != nullptr)
				compList.push_back(comp);
		}
		return compList;
	}

	template <class T>
	void removeComponent() {

		for (auto it = components.begin(); it < components.end(); it++) {

			T* comp = dynamic_cast<T*>(*it);

			if (comp != nullptr) {

				delete *it;
				components.erase(it);
				return;
			}
		}
	}

	template <class T>
	bool removeComponent(T* component) {

		for (auto it = components.begin(); it < components.end(); it++) {

			T* comp = dynamic_cast<T*>(*it);

			if (comp == component) {

				delete* it;
				components.erase(it);
				return true;
			}
		}
		return false;
	}

	bool destroy(Scene* scene);

	Entity copy(Scene* scene);

};