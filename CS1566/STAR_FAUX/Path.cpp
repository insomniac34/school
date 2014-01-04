//written by Tyler Raborn

#include "Path.h"

Path::Path()
{

}

Path::Path(GLfloat start[3], GLfloat end[3])
{
	this->startPoint = Vertex(start);
	this->endPoint = Vertex(end);
	hasWaypoints = false;
}

Path::Path(Vertex start, Vertex end)
{
	//printf("start: {%f, %f, %f}\nend: {%f, %f, %f}\n", start.x, start.y, start.z, end.x, end.y, end.z);
	this->startPoint = Vertex(start.x, start.y, start.z);
	this->endPoint = Vertex(end.x, end.y, end.z);

	//printf("pushing endpoint: {%f, %f, %f}\n", endPoint.x, endPoint.y, endPoint.z);
	waypoints.push_front(endPoint);
	waypoints.push_front(startPoint);
	//printf("vertices pushed onto deque. waypoints.end() = {%f, %f, %f} and waypoints.begin() = {%f, %f, %f}\n", this->waypoints.end()->x, this->waypoints.end()->y, this->waypoints.end()->z, this->waypoints.begin()->x, this->waypoints.begin()->y, this->waypoints.begin()->z);

	hasWaypoints = false;
}

Path::Path(Vertex start, Vertex end, Vertex waypointArray[], int len)
{
	this->startPoint = Vertex(start.x, start.y, start.z);
	this->endPoint = Vertex(end.x, end.y, end.z);

	for (int i = 0; i < len; i++)
	{
		this->waypoints.push_front(waypointArray[i]);
	}

	hasWaypoints = true;
}

bool Path::hasWaypointList()
{
	return hasWaypoints;
}


Path::~Path()
{

}
