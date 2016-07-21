
#include "main.h"

//typedef struct { double x; double y; double z; }vector3;



// returns the magnitude of a vector
float v3_magnitude(vector3 *v){

	return sqrt(v->x * v->x + v->y * v->y + v->z * v->z);
}

// returns the square of the magnitude of a vector
float v3_magnitude2(vector3 *v){

	return (v->x * v->x + v->y * v->y + v->z * v->z);
}

// normalise a vector
// |v| = 1
void v3_normalise(vector3 *v){

	float m = sqrt(v->x * v->x + v->y * v->y + v->z * v->z);

	if (m != 0){
		v->x /= m;
		v->y /= m;
		v->z /= m;
	}

}

// reverse a vector; make it point in the oppisite direction.
// aka the conjugate.
void v3_reverse(vector3 *v){

	v->x = -v->x;
	v->y = -v->y;
	v->z = -v->z;
}

// adds two vectors.
// returns the resultaing vector.
vector3 v3_add(vector3 *v1, vector3 *v2){

	vector3 result;

	result.x = v1->x + v2->x;
	result.y = v1->y + v2->y;
	result.z = v1->z + v2->z;

	return result;
}
vector3 v3_sub(vector3 *v1, vector3 *v2){

	vector3 result;

	result.x = v1->x - v2->x;
	result.y = v1->y - v2->y;
	result.z = v1->z - v2->z;

	return result;
}
// multiply by a scaler.
void v3_scaler_multiply(vector3 *v, float num){

	v->x *= num;
	v->y *= num;
	v->z *= num;
}

// divide by a scaler.
void v3_scaler_divide(vector3 *v, float num){

	v->x /= num;
	v->y /= num;
	v->z /= num;
}

// returns the cross product of two vectors. v1 X v2.
// i.e. a vector perpendicular to both v1 and v2.
vector3 v3_cross(vector3 *v1, vector3 *v2){

	vector3 result;
	
	result.x = v1->y * v2->z - v1->z * v2->y;
	result.y = v1->z * v2->x - v1->x * v2->z;
	result.z = v1->x * v2->y - v1->y * v2->x;

	return result;
}

// returns the dot product of two vectors. v1 * v2.
// represents the projection of v1 onto v2.
float v3_dot(vector3 *v1, vector3 *v2){

	return 	  v1->x * v2->x
			+ v1->y * v2->y
			+ v1->z * v2->z;	
}	



