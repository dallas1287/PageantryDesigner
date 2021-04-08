#pragma once
#include <QString>

class SceneObject
{
public:
	SceneObject() {}
	virtual ~SceneObject() {}
	const QString& getName() const { return m_name; }
	void setName(const QString& name) { m_name = name; }
	
protected:
	QString m_name;
};

