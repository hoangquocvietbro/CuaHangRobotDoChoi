/*Chương trình chiếu sáng Blinn-Phong (Phong sua doi) cho hình lập phương đơn vị, điều khiển quay bằng phím x, y, z, X, Y, Z.*/

#include "Angel.h"  /* Angel.h là file tự phát triển (tác giả Prof. Angel), có chứa cả khai báo includes glew và freeglut*/


// remember to prototype
void generateGeometry(void);
void initGPUBuffers(void);
void shaderSetup(void);
void display(void);
void keyboard(unsigned char key, int x, int y);

typedef vec4 point4;
typedef vec4 color4;
using namespace std;

// Số các đỉnh của các tam giác
const int NumPoints = 36;

point4 points[NumPoints]; /* Danh sách các đỉnh của các tam giác cần vẽ*/
color4 colors[NumPoints]; /* Danh sách các màu tương ứng cho các đỉnh trên*/
vec3 normals[NumPoints]; /*Danh sách các vector pháp tuyến ứng với mỗi đỉnh*/

point4 vertices[8]; /* Danh sách 8 đỉnh của hình lập phương*/
color4 vertex_colors[8]; /*Danh sách các màu tương ứng cho 8 đỉnh hình lập phương*/

GLuint program;

GLfloat theta[3] = { 0, 0, 0 };
GLfloat dr = 5;

mat4 model;
GLuint model_loc;
mat4 projection;
GLuint projection_loc;
mat4 view;
GLuint view_loc;


void initCube()
{
	// Gán giá trị tọa độ vị trí cho các đỉnh của hình lập phương
	vertices[0] = point4(-0.5, -0.5, 0.5, 1.0);
	vertices[1] = point4(-0.5, 0.5, 0.5, 1.0);
	vertices[2] = point4(0.5, 0.5, 0.5, 1.0);
	vertices[3] = point4(0.5, -0.5, 0.5, 1.0);
	vertices[4] = point4(-0.5, -0.5, -0.5, 1.0);
	vertices[5] = point4(-0.5, 0.5, -0.5, 1.0);
	vertices[6] = point4(0.5, 0.5, -0.5, 1.0);
	vertices[7] = point4(0.5, -0.5, -0.5, 1.0);

	// Gán giá trị màu sắc cho các đỉnh của hình lập phương	
	vertex_colors[0] = color4(0.0, 0.0, 0.0, 1.0); // black
	vertex_colors[1] = color4(1.0, 0.0, 0.0, 1.0); // red
	vertex_colors[2] = color4(1.0, 1.0, 0.0, 1.0); // yellow
	vertex_colors[3] = color4(0.0, 1.0, 0.0, 1.0); // green
	vertex_colors[4] = color4(0.0, 0.0, 1.0, 1.0); // blue
	vertex_colors[5] = color4(1.0, 0.0, 1.0, 1.0); // magenta
	vertex_colors[6] = color4(1.0, 0.5, 0.0, 1.0); // orange
	vertex_colors[7] = color4(0.0, 1.0, 1.0, 1.0); // cyan
}
int Index = 0;
void quad(int a, int b, int c, int d)  /*Tạo một mặt hình lập phương = 2 tam giác, gán màu cho mỗi đỉnh tương ứng trong mảng colors*/
{
	vec4 u = vertices[b] - vertices[a];
	vec4 v = vertices[c] - vertices[b];
	vec3 normal = normalize(cross(u, v));

	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[b]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[c]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[c]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[d]; Index++;
}
void makeColorCube(void)  /* Sinh ra 12 tam giác: 36 đỉnh, 36 màu*/

{
	quad(1, 0, 3, 2);
	quad(2, 3, 7, 6);
	quad(3, 0, 4, 7);
	quad(6, 5, 1, 2);
	quad(4, 5, 6, 7);
	quad(5, 4, 0, 1);
}
void generateGeometry(void)
{
	initCube();
	makeColorCube();
}


void initGPUBuffers(void)
{
	// Tạo một VAO - vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Tạo và khởi tạo một buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors) + sizeof(normals), NULL, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors), sizeof(normals), normals);


}

void shaderSetup(void)
{
	// Nạp các shader và sử dụng chương trình shader
	program = InitShader("vshader1.glsl", "fshader1.glsl");   // hàm InitShader khai báo trong Angel.h
	glUseProgram(program);

	// Khởi tạo thuộc tính vị trí đỉnh từ vertex shader
	GLuint loc_vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(loc_vPosition);
	glVertexAttribPointer(loc_vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	GLuint loc_vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(loc_vColor);
	glVertexAttribPointer(loc_vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));

	GLuint loc_vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(loc_vNormal);
	glVertexAttribPointer(loc_vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points) + sizeof(colors)));

	/* Khởi tạo các tham số chiếu sáng - tô bóng*/
	point4 light_position(0.0, 0.0, 1.0, 0.0);
	color4 light_ambient(0.2, 0.2, 0.2, 1.0);
	color4 light_diffuse(1.0, 1.0, 1.0, 1.0);
	color4 light_specular(1.0, 1.0, 1.0, 1.0);

	color4 material_ambient(1.0, 0.0, 1.0, 1.0);
	color4 material_diffuse(1.0, 1, 0.0, 1.0);
	color4 material_specular(1.0, 0.8, 0.0, 1.0);
	float material_shininess = 100.0;

	color4 ambient_product = light_ambient * material_ambient;
	color4 diffuse_product = light_diffuse * material_diffuse;
	color4 specular_product = light_specular * material_specular;

	glUniform4fv(glGetUniformLocation(program, "AmbientProduct"), 1, ambient_product);
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
	glUniform4fv(glGetUniformLocation(program, "SpecularProduct"), 1, specular_product);
	glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, light_position);
	glUniform1f(glGetUniformLocation(program, "Shininess"), material_shininess);

	model_loc = glGetUniformLocation(program, "Model");
	projection_loc = glGetUniformLocation(program, "Projection");
	view_loc = glGetUniformLocation(program, "View");

	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0, 1.0, 1.0, 1.0);        /* Thiết lập màu trắng là màu xóa màn hình*/


	}

GLfloat openDoor = 0;
mat4 ins;
mat4 QuayBase;
GLfloat quayCanh = 0;
GLfloat l = -45.0, r = 45.0, b = -45.0, t = 45.0, zNear = 100.0, zFar = 300.0;
void HinhPhang(GLfloat x, GLfloat y, GLfloat z, mat4 mt) {
	point4 eye(0, 0, 200, 1.0);
	point4 at(0, 0, 0, 1.0);
	vec4 up(0, 1, 0, 1.0);

	mat4 v = LookAt(eye, at, up);
	ins = Scale(x, y, z);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, v * QuayBase * mt * ins);
	mat4 p = Frustum(l, r, b, t, zNear, zFar);
	glUniformMatrix4fv(projection_loc, 1, GL_TRUE, p);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void NenNha() {
	ins = Translate(0, -50, 0) * Scale(100, 1, 100);
	HinhPhang(1, 1, 1, ins);
}
void TranNha() {
	ins = Translate(0, 50, 0) * Scale(100, 1, 100);
	HinhPhang(1, 1, 1, ins);
}
void TuongSau() {
	ins = Translate(0, 0, -50) * Scale(100, 100, 1);
	HinhPhang(1, 1, 1, ins);
}
void TuongTrai() {
	ins = Translate(-50, 0, 0) * Scale(1, 100, 100);
	HinhPhang(1, 1, 1, ins);
}
void TuongPhai() {
	ins = Translate(50, 0, 0) * Scale(1, 100, 100);;
	HinhPhang(1, 1, 1, ins);
}
void TuongTruoc() {
	//tuong phai
	ins = Translate(35, 0, 50) * Scale(30, 100, 1);
	HinhPhang(1, 1, 1, ins);
	//tuong trai
	ins = Translate(-35, 0, 50) * Scale(30, 100, 1);
	HinhPhang(1, 1, 1, ins);
	//tuong tren canh cua
	ins = Translate(0, 20, 50) * Scale(40, 60, 1);
	HinhPhang(1, 1, 1, ins);

}
void CanhCua() {
	//canh cua trai
	ins = Translate(-20, -30, 50) * RotateY(-openDoor) * Translate(10, 0, 0) * Scale(20, 40, 1);
	HinhPhang(1, 1, 1, ins);
	//canh cua phai
	ins = Translate(20, -30, 50) * RotateY(openDoor) * Translate(-10, 0, 0) * Scale(20, 40, 1);
	HinhPhang(1, 1, 1, ins);


}
void KeSau() {
	ins = Translate(0, -40, -35) * Scale(100, 20, 30);
	HinhPhang(1, 1, 1, ins);
}
void CanPhong() {
	NenNha();
	TranNha();
	TuongSau();
	TuongTrai();
	TuongPhai();
	TuongTruoc();
	CanhCua();
	KeSau();
}

void display(void)
{
	QuayBase = RotateX(theta[0]) * RotateY(theta[1]) * RotateZ(theta[2]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	const vec3 viewer_pos(0.0, 0.0, 2.0);  /*Trùng với eye của camera*/
	CanPhong();
	glutSwapBuffers();
}
void reshape(int width, int height)
{
	vec4 eye(0, 0, 2, 1);
	vec4 at(0, 0, 0, 1);
	vec4 up(0, 1, 0, 1);

	view = LookAt(eye, at, up);
	glUniformMatrix4fv(view_loc, 1, GL_TRUE, view);

	projection = Frustum(-1, 1, -1, 1, 1, 4);
	glUniformMatrix4fv(projection_loc, 1, GL_TRUE, projection);

	glViewport(0, 0, width, height);
}
void keyboard(unsigned char key, int x, int y)
{
	// keyboard handler

	switch (key) {
	case 033:
		exit(1);
		break;
	case 'x':
		theta[0] += 5;
		break;
	case 'y':
		theta[1] += 5;
		break;
	case 'z':
		theta[2] += 5;
		break;
	case '1':
		l *= 1.1;
		r *= 1.1;
		break;
	case '2':
		l *= 0.9;
		r *= 0.9;
		break;
	case '3':
		b *= 1.1;
		t *= 1.1;
		break;
	case '4':
		b *= 0.9;
		t *= 0.9;
		break;
	case '5':
		zNear *= 1.001;
		if (zNear > 145 && zNear < 151) zNear = 151;
		zFar *= 1.001;
		break;
	case '6':
		zNear *= 0.9;
		zFar *= 0.9;
		break;
	case 'o':
		openDoor += 5;
		if (openDoor >= 170) openDoor = 170;
		break;
	case 'O':
		openDoor -= 5;
		if (openDoor < 0) openDoor = 0;
		break;
	}
	glutPostRedisplay();
}



int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(640, 640);
	glutInitWindowPosition(100, 150);
	glutCreateWindow("Robot toy shop");


	glewInit();

	generateGeometry();
	initGPUBuffers();
	shaderSetup();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);
	glutMainLoop();
	return 0;
}
