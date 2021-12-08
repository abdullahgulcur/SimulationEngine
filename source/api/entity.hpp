#pragma once

#include <iostream>

#include "transform.hpp"
#include "component.hpp"

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

		if (getComponent<T>() != nullptr) {
			
			std::cout << "There is existing component in the same type!" << std::endl;
			return nullptr;
		}

		T* comp = new T;
		components.push_back(comp);

		return comp;
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

	bool destroy(Scene* scene);

};