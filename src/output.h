#pragma once


class Output
{
public:
	Output()
	{
	}

	Output(Component* component, Link* link)
	{
		this->component = component;
		this->link = link;
		this->powered = false;
	}

	~Output()
	{
	}

	bool getPowered()
	{
		return powered;
	}

	void setPowered(bool state);

	Component* getComponent()
	{
		return component;
	}

	Link* getLink()
	{
		return link;
	}
	
private:
	bool powered;
	Component* component;
	Link* link;
};

