#ifndef GEOMETRIC_MESH_LIBRARY
#define GEOMETRIC_MESH_LIBRARY
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
	float x;
	float y;
	float z;
} gml_vec3f;

typedef struct {
	unsigned int i0;
	unsigned int i1;
	unsigned int i2;
} gml_vec3i;

typedef struct {
	gml_vec3f points[3];
	gml_vec3f normal;
} gml_triangle;

typedef struct {
	gml_triangle* m_triangles;
	unsigned int* m_indices;
	char* m_name;
	unsigned int used_triangles;
	unsigned int used_indices;
	unsigned int n_triangles;
	unsigned int n_indices;
} gml_mesh;

/* Subtracts two vectors and returns by value a new gml_vec3f. */
gml_vec3f gml_subtract(const gml_vec3f* left, const gml_vec3f* right);

/* Returns by value a new gml_vec3f by cross product. */
gml_vec3f gml_cross_product(const gml_vec3f* left, const gml_vec3f* right);

/* Normalizes the given vector. */
void gml_normalize(gml_vec3f* normal);

/* Gets the normal of the given triangle (plane). */
gml_vec3f gml_get_normal(gml_vec3f q, gml_vec3f r, gml_vec3f s);

/* Initiates the given mesh pointer. init is the expected number of triangles. */
void gml_create_mesh(gml_mesh* m, unsigned int init);

/* Deletes the mesh and its elements. */
void gml_delete_mesh(gml_mesh* m);

/* Returns by value a new triangle struct given 3 vectors. */
gml_triangle gml_build_triangle(gml_vec3f* x, gml_vec3f* y, gml_vec3f* z);

/* Generates an index for the given vertex.
If the vertex already exists, use that index. */
unsigned int gml_get_idx(const gml_mesh* m, const gml_vec3f* v);

/* Adds a triangle to the mesh given 3 vectors. */
void gml_add_triangle(gml_mesh* m, gml_vec3f* x, gml_vec3f* y, gml_vec3f* z);

/* Adds a quad to the mesh given 4 vectors. It creates two triangles with identical normals with the 4 vectors. */
void gml_add_quad(gml_mesh* m, gml_vec3f* x, gml_vec3f* y, gml_vec3f* z, gml_vec3f* w);

/* Prints the elements of the mesh using printf. */
void gml_print_mesh(const gml_mesh* m);

/* Golden ratio (1.0f + sqrtf(5.0f)) / 2.0f.) */
float gml_tau();

#ifdef GEOMETRIC_MESH_LIBRARY_IMPLEMENTATION_H

gml_vec3f gml_subtract(const gml_vec3f* left, const gml_vec3f* right) {
	gml_vec3f res = {
		(left->x - right->x),
		(left->y - right->y),
		(left->z - right->z)
	};
	return res;
}

gml_vec3f gml_cross_product(const gml_vec3f* left, const gml_vec3f* right) {
	gml_vec3f v = {
		(left->y * right->z) - (left->z * right->y),
		(left->z * right->x) - (left->x * right->z),
		(left->x * right->y) - (left->y * right->x)
	};
	return v;
}

void gml_normalize(gml_vec3f* normal) {
	float f = sqrtf((normal->x * normal->x) + (normal->y * normal->y) + (normal->z * normal->z));
	if (f <= 0.0f) {
		printf("[ERROR] normalize: Vector length is 0 and cannot be normalized\n");
		return;
	}
	normal->x = normal->x / f;
	normal->y = normal->y / f;
	normal->z = normal->z / f;
}

gml_vec3f gml_get_normal(gml_vec3f q, gml_vec3f r, gml_vec3f s) {
	gml_normalize(&q);
	gml_normalize(&r);
	gml_normalize(&s);
	gml_vec3f qr = gml_subtract(&r, &q);
	gml_vec3f qs = gml_subtract(&s, &q);
	gml_vec3f normal = gml_cross_product(&qr, &qs);
	gml_normalize(&normal);
	return normal;
}

void gml_create_mesh(gml_mesh* m, unsigned int init) {
	m->m_triangles = (gml_triangle*)malloc(init * sizeof(gml_triangle));
	m->m_indices = (unsigned int*)malloc(init * sizeof(unsigned int) * 3);
	m->n_triangles = init;
	m->n_indices = init * 3;
	m->used_indices = 0;
	m->used_triangles = 0;
}

void gml_delete_mesh(gml_mesh* m) {
	free(m);
}

gml_triangle gml_build_triangle(gml_vec3f* x, gml_vec3f* y, gml_vec3f* z) {
	/*printf("(%.2f, %.2f, %.2f)\n", x.x, x.y, x.z);
	printf("(%.2f, %.2f, %.2f)\n", y.x, y.y, y.z);
	printf("(%.2f, %.2f, %.2f)\n", z.x, z.y, z.z);*/
	gml_vec3f n = gml_get_normal(*x, *y, *z);
	gml_triangle t = { *x, *y, *z, n };
	return t;
}

unsigned int gml_get_idx(const gml_mesh* m, const gml_vec3f* v) {
	for (unsigned int i = 0; i < m->n_triangles; i++) {
		for (unsigned int j = 0; j < 3; j++) {
			if (m->m_triangles[i].points[j].x == v->x &&
				m->m_triangles[i].points[j].y == v->y &&
				m->m_triangles[i].points[j].z == v->z
				) {
				return m->m_indices[(3 * i) + j];
			}
		}
	}
	if (m->used_indices >= 1) {
		unsigned int largest = 0;
		for (unsigned int i = 0; i < m->used_indices; i++) {
			if (m->m_indices[i] > largest) {
				largest = m->m_indices[i];
			}
		}
		// return largest + 1;
		return largest + 1;
	}
	else {
		return 0;
	}
}

void gml_add_triangle(gml_mesh* m, gml_vec3f* x, gml_vec3f* y, gml_vec3f* z) {
	if (m == NULL) {
		printf("[ERROR] add_triangle: Mesh is null or uninitalized\n");
		return;
	}
	if (m->used_triangles >= m->n_triangles) {
		m->n_triangles *= 2;
		gml_triangle* buffer = (gml_triangle*)realloc(m->m_triangles, m->n_triangles * sizeof(gml_triangle));
		if (buffer == NULL) {
			printf("[ERROR] add_triangle: Error allocating memory for m_triangles\n");
			return;
		}
		m->m_triangles = buffer;
	}
	if (m->used_indices + 3 >= m->n_indices) {
		m->n_indices *= 2;
		unsigned int* buffer = (unsigned int*)realloc(m->m_indices, m->n_indices * sizeof(unsigned int));
		if (buffer == NULL) {
			printf("[ERROR] add_triangle: Error allocating memory for m_indices\n");
			return;
		}
		m->m_indices = buffer;
	}
	m->m_indices[m->used_indices++] = gml_get_idx(m, x);
	m->m_indices[m->used_indices++] = gml_get_idx(m, y);
	m->m_indices[m->used_indices++] = gml_get_idx(m, z);
	gml_triangle t = gml_build_triangle(x, y, z);
	m->m_triangles[m->used_triangles++] = t;
}

void gml_add_quad(gml_mesh* m, gml_vec3f* x, gml_vec3f* y, gml_vec3f* z, gml_vec3f* w) {
	// Don't Repeat Yourself
	gml_add_triangle(m, x, y, z);
	gml_add_triangle(m, x, z, w);
}

void gml_print_mesh(const gml_mesh* m) {
	printf("Mesh [%p] has [%u] triangles and [%u] indices\n", (void*)m, m->used_triangles, m->used_indices);
	for (unsigned int i = 0; i < m->used_triangles; i++) {
		printf("Triangle [%d]\t--- (%.2f, %.2f, %.2f), ", i, m->m_triangles[i].points[0].x, m->m_triangles[i].points[0].y, m->m_triangles[i].points[0].z);
		printf("(%.2f, %.2f, %.2f), ", m->m_triangles[i].points[1].x, m->m_triangles[i].points[1].y, m->m_triangles[i].points[1].z);
		printf("(%.2f, %.2f, %.2f)\n", m->m_triangles[i].points[2].x, m->m_triangles[i].points[2].y, m->m_triangles[i].points[2].z);
		printf("Normal [%d]\t--- (%.2f, %.2f, %.2f)\n", i, m->m_triangles[i].normal.x, m->m_triangles[i].normal.y, m->m_triangles[i].normal.z);
	}
	for (unsigned int i = 0; i < m->used_indices; i++) {
		printf("Index [%d]\t--- %d\n", i, m->m_indices[i]);
	}
	printf("\n");
}

float gml_tau() {
	return (1.0f + sqrtf(5.0f)) / 2.0f;
}

#endif /* GEOMETRIC_MESH_LIBRARY_IMPLEMENTATION_H */

#endif /* GEOMETRIC_MESH_LIBRARY */