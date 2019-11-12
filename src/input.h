#pragma once
#include "link.h"

class Component;

class Input
{
public:
	Input()
	{
	}

	Input(Component* component, Link* link) :
		component(component),
		link(link)
	{
	}

	~Input()
	{
	}

	bool getPowered()
	{
		return *link->powered;
	};

	Component* getComponent()
	{
		return component;
	}

	Link* getLink()
	{
		return link;
	}
private:
	Component* component;
	Link* link;
};

