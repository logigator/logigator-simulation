#pragma once

class Component;
class Link;

class Output
{
public:
	Output() :
		component(nullptr),
		link(nullptr),
		powered(false)
	{
	}

	Output(Component* component, Link* link) :
		component(component),
		link(link),
		powered(false)
	{
	}

	~Output() = default;

	bool getPowered() const
	{
		return powered;
	}

	void setPowered(bool state);

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
	bool powered;
};

