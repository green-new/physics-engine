#include "geometry.hpp"

using namespace geolib;

/*
* Vertex class implementation
*/
vertex::vertex() : leaving(NULL) { }
vertex::~vertex() { }

/*
* Face class implementation
*/
face::face() : inner(NULL) { }
face::~face() { }
void face::update_normal() {

	/* Get all vertices */

	/* Get their differences */

	/* Get the cross product of D0, D1 */

	/* Add up the sum of all cross products */

	/* Normalize the sum */
}
void face::update_vertex_normals() const {
	// vec3 normal;
	// 
	
}
int face::get_edge_count() {
	/* 
	* unsigned int edge_count = 0;
	* edge* E = this->inner;
	* do 
	*	if (E)
	*		edge_count++;
	*	end if
	* while E != this->inner
	*/
}