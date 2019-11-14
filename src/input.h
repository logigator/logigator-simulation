#pragma once
#include "link.h"

class Component;

class Input
{
public:
	Input() = default;

	Input(Component* component, Link* link) :
		component(component),
		link(link)
	{
	}

	~Input() = default;

	bool getPowered() const
	{
		return *link->powered;
	};

	Component* getComponent() const
	{
		return component;
	}

	Link* getLink() const
	{
		return link;
	}
private:
	Component* component;
	Link* link;
};

