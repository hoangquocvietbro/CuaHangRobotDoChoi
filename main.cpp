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

class MatrixStack {
public:
	const static int MAX = 50;
	MatrixStack() { index = 0; }
	void push(const mat4& matrix);
	mat4 pop();
private:
	int index;
	mat4 matrices[MAX];
};

void MatrixStack::push(const mat4& matrix) {
	matrices[index] = matrix;
	index++;
}

mat4 MatrixStack::pop() {
	index--;
	return matrices[index];
}

// Số các đỉnh của các tam giác
const int NumPoints = 36;

point4 points[NumPoints]; /* Danh sách các đỉnh của các tam giác cần vẽ*/
color4 colors[NumPoints]; /* Danh sách các màu tương ứng cho các đỉnh trên*/
vec3 normals[NumPoints]; /*Danh sách các vector pháp tuyến ứng với mỗi đỉnh*/

point4 vertices[8]; /* Danh sách 8 đỉnh của hình lập phương*/
color4 vertex_colors[8]; /*Danh sách các màu tương ứng cho 8 đỉnh hình lập phương*/

GLuint program;

GLuint model_loc;
GLuint projection_loc;
GLuint view_loc;
GLfloat theta[20];
GLfloat X = -10;
GLfloat Y = 0;
GLfloat Z = 35;
GLfloat PhongTo = 1;
mat4 model;
mat4 projection;
mat4 view;
GLfloat openDoor = 0;
GLfloat l = -45, r = 45, b = -45, t = 45, zNear = 100, zFar = 300;
GLfloat eyeX = 0, eyeY = 0, eyeZ = -200;
color4 c1 = color4(0, 1, 0, 1.0);
color4 c2 = color4(0.023, 0.144, 0.572, 1.0);
color4 c3 = color4(0, 1, 1, 1.0);
color4 c4 = color4(1, 0.502, 0, 1.0);
bool ActiveRobot1, ActiveRobot2, ActiveRobot3, ActiveRobot4, ActiveRobot5, ActiveRobot6, ActiveRobot7, ActiveRobot8;
GLfloat theta_Robot1[20], theta_Robot2[20], theta_Robot3[20], theta_Robot4[20], theta_Robot5[20], theta_Robot6[20], theta_Robot7[20], theta_Robot8[20];

point4 light_position(160, 200, 200, 0.0);
color4 light_ambient(0.2, 0.2, 0.2, 1.0);
color4 light_diffuse(1.0, 1.0, 1.0, 1.0);
color4 light_specular(1.0, 1.0, 1.0, 1.0);

color4 material_ambient(1.0, 0.0, 1.0, 1.0);
color4 material_diffuse(1.0, 0.8, 0.0, 1.0);
color4 material_specular(1.0, 0.8, 0.0, 1.0);
float material_shininess = 100.0;

color4 ambient_product = light_ambient * material_ambient;
color4 diffuse_product = light_diffuse * material_diffuse;
color4 specular_product = light_specular * material_specular;

GLuint diffuse_loc;
GLuint ambient_loc;
GLuint specular_loc;

color4 colorWood = color4(0.205, 0.018, 0.030, 1.0);
color4 colorGlass = color4(0.000, 0.990, 0.962, 0.5);
color4 colorWall = color4(0.701, 0.760, 0.721, 1.0);

// định nghĩa độ dày vật liệu
GLfloat dayTuong = 0.15;
GLfloat dayKhungCua = 0.05;
GLfloat dayCua = 0.1;
GLfloat dayKinh = 0.04;
GLfloat dayCanhCuaSo = dayTuong / 2;

GLfloat xCuaSoTP = dayTuong, yCuaSoTP = 1, zCuaSoTP = 1.2;
GLfloat xTuongPhai = dayTuong, yTuongPhai = 3.5, zTuongPhai = 3.5 + 2 * dayTuong;
MatrixStack modelStack;
mat4 ctm;
GLfloat dr = 5;


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

	diffuse_loc = glGetUniformLocation(program, "DiffuseProduct");
	ambient_loc = glGetUniformLocation(program, "AmbientProduct");
	specular_loc = glGetUniformLocation(program, "SpecularProduct");
	glUniform4fv(ambient_loc, 1, ambient_product);
	glUniform4fv(diffuse_loc, 1, diffuse_product);
	glUniform4fv(specular_loc, 1, specular_product);
	glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, light_position);
	glUniform1f(glGetUniformLocation(program, "Shininess"), material_shininess);

	model_loc = glGetUniformLocation(program, "Model");
	projection_loc = glGetUniformLocation(program, "Projection");
	view_loc = glGetUniformLocation(program, "View");

	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0, 1.0, 1.0, 1.0);        /* Thiết lập màu trắng là màu xóa màn hình*/
}

//void setColor(color4 color) {
//	color4 material_diffuse = color;
//	color4 light_diffuse(1, 1, 1, 1);
//	color4 diffuse_product = light_diffuse * material_diffuse;
//	glUniform4fv(diffuse_loc, 1, diffuse_product);
//}
void setColor(color4 color) {
	material_diffuse = color;
	diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(diffuse_loc, 1, diffuse_product);
}

void body(mat4 ins)
{
	setColor(c1);
	mat4 m = Translate(0, -0.5 * 0.8, 0) * Scale(0.5, 0.8, 0.2);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ins * model * m);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void upper_leg(mat4 ins)
{
	setColor(color4(1, 0, 1, 1));
	mat4 m = Translate(0, -0.5 * 0.2, 0) * Scale(0.2, 0.2, 0.1);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ins * model * m);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}

void lower_leg(mat4 ins)
{
	mat4 m = Translate(0, -0.5 * 0.4, 0) * Scale(0.1, 0.4, 0.08);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ins * model * m);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void upper_arm(mat4 ins)
{
	setColor(c4);
	mat4 m = Translate(0, -0.5 * 0.2, 0) * Scale(0.15, 0.2, 0.08);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ins * model * m);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void lower_arm(mat4 ins)
{
	setColor(c4);
	mat4 m = Translate(0, -0.5 * 0.3, 0) * Scale(0.1, 0.3, 0.07);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ins * model * m);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void neck(mat4 ins)
{
	setColor(c2);
	mat4 m = Translate(0, 0.5 * 0.1, 0) * Scale(0.1, 0.1, 0.1);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ins * model * m);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void head(mat4 ins)
{
	setColor(c3);
	mat4 m = Translate(0.0, 0.3 * 0.5, 0.0) * Scale(0.3, 0.3, 0.2);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ins * model * m);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}

void Robot(mat4 ins, GLfloat theta[])
{

	model = RotateY(theta[0]);
	body(ins);
	mat4 view_upleg1 = model * Translate(0.15, -0.825, 0) * RotateX(theta[2]);
	mat4 view_upleg2 = model * Translate(-0.15, -0.825, 0) * RotateX(theta[4]);
	mat4 view_uparm1 = model * Translate(0.35, -0.1 * 0.5, 0) * RotateX(theta[6]) * RotateZ(theta[7]);
	mat4 view_uparm2 = model * Translate(-0.35, -0.1 * 0.5, 0) * RotateX(theta[8]) * RotateZ(theta[9]);
	mat4 view_neck = model * Translate(0, 0.02, 0) * RotateX(theta[12]);
	//Chân Trái

	model = view_upleg1;
	upper_leg(ins);
	mat4 view_lowleg1 = view_upleg1 * Translate(0, -0.225, 0) * RotateX(theta[3]);
	model = view_lowleg1;
	lower_leg(ins);

	//Chân phải 
	model = view_upleg2;
	upper_leg(ins);
	mat4 view_lowleg2 = view_upleg2 * Translate(0, -0.225, 0) * RotateX(theta[5]);
	model = view_lowleg2;
	lower_leg(ins);

	//Tay Trái
	setColor(c1);
	model = view_uparm1;
	upper_arm(ins);
	mat4 view_lowarm1 = view_uparm1 * Translate(0, -0.225, 0) * RotateX(theta[10]);
	model = view_lowarm1;
	lower_arm(ins);

	//Tay Phải
	model = view_uparm2;
	upper_arm(ins);
	mat4 view_lowarm2 = view_uparm2 * Translate(0, -0.225, 0) * RotateX(theta[11]);
	model = view_lowarm2;
	lower_arm(ins);

	//Cổ
	model = view_neck;
	neck(ins);

	//Đầu 
	mat4 view_head = view_neck * Translate(0, 0.125, 0) * RotateX(theta[13]);
	model = view_head;
	head(ins);

}

void NenNha() {
	mat4 m = Translate(0, -50, 0) * Scale(100, 1, 100);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, m);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void TranNha() {
	mat4 m = Translate(0, 50, 0) * Scale(100, 1, 100);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, m);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void TuongSau() {
	mat4 m = Translate(0, 0, 50) * Scale(100, 100, 1);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, m);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void TuongTrai() {
	mat4 m = Translate(-50, 0, 0) * Scale(1, 100, 100);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, m);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void TuongPhai() {
	mat4 m = Translate(50, 0, 0) * Scale(1, 100, 100);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, m);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void TuongTruoc() {
	//tuong phai
	mat4 m = Translate(35, -14.5, -50) * Scale(30.001, 71, 1);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, m);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	m = Translate(0, 45, -50) * Scale(100, 10, 1);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, m);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	//tuong trai
	m = Translate(-35, -14.5, -50) * Scale(30, 71, 1);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, m);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	//tuong tren canh cua
	m = Translate(0, 30, -50) * Scale(54, 20, 1);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, m);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

}

void KeSau() {
	//Chân kệ
	mat4 m = Translate(0, -40, 35) * Scale(100, 20, 30);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, m);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	//các thanh ngang
	m = Translate(0, 10, 35) * Scale(100, 2, 30);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, m);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void CanPhong() {
	setColor(color4(0.8, 0.478, 0, 1.0));
	NenNha();
	setColor(color4(0.701, 0.760, 0.721, 1.0));
	TranNha();
	TuongSau();
	TuongTrai();
	TuongPhai();
	setColor(color4(0.4, 0.4, 0.2, 0.1));
	TuongTruoc();
	setColor(color4(0.701, 0.760, 0.721, 1.0));
	KeSau();

}

void RobotDatKeSauTang1() {
	//Lấy Robot có sẵn của lịch truyền vào mỗi hàm bộ phận một tham số mat4 ins để có thể biến đổi
	//vidu  Robot(Translate(40, 0, -45) * Scale(20,20,20)); là scale phóng to robot 20 lần và dịch x 40 dịch z -45 để đặt vào kệ ngăn dưới
	Robot(Translate(40, 0, 35) * Scale(20, 20, 20), theta_Robot1);
	Robot(Translate(15, 0, 35) * Scale(20, 20, 20), theta_Robot2);
	Robot(Translate(X, Y, Z) * Scale(20 * PhongTo, 20 * PhongTo, 20 * PhongTo), theta_Robot3);
	Robot(Translate(-35, 0, 35) * Scale(20, 20, 20), theta_Robot4);

}
void RobotDatKeSauTang2() {
	Robot(Translate(40, 40, 35) * Scale(20, 20, 20), theta_Robot5);
	Robot(Translate(15, 40, 35) * Scale(20, 20, 20), theta_Robot6);
	Robot(Translate(-10, 40, 35) * Scale(20, 20, 20), theta_Robot7);
	Robot(Translate(-35, 40, 35) * Scale(20, 20, 20), theta_Robot8);
}

void createElement(mat4 ins) {
	modelStack.push(model);
	model *= ins;
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	model = modelStack.pop();
}
void KhungCua() {
	setColor(colorWood);
	mat4 m = Translate(-21, -15, -50) * Scale(2, 70, 1.001);
	createElement(m);
	m = Translate(21, -15, -50) * Scale(2, 70, 1.001);
	createElement(m);

	m = Translate(0, 21, -50) * Scale(44, 2, 1.001);
	createElement(m);
}
void CanhCua() {
	KhungCua();
	setColor(colorGlass);
	//canh cua trai
	mat4 m = Translate(-20, -15, -50) * RotateY(openDoor) * Translate(10, 0, 0) * Scale(20, 70, 1);
	createElement(m);
	//canh cua phai
	m = Translate(20, -15, -50) * RotateY(-openDoor) * Translate(-10, 0, 0) * Scale(20, 70, 1);
	createElement(m);
}
void khungCuaSo() {
	setColor(colorWood);
	GLfloat x1 = dayKhungCua, y1 = yCuaSoTP, z = dayTuong;
	// Thanh bên trái khung cửa sổ
	ctm = Translate(x1 * 0.5, y1 * 0.5, -z * 0.5) * Scale(x1, y1, z);
	createElement(ctm);

	// Thanh bên phải khung cửa sổ
	ctm = Translate(zCuaSoTP - x1 * 0.5, y1 * 0.5, -z * 0.5) * Scale(x1, y1, z);
	createElement(ctm);

	GLfloat x2 = zCuaSoTP, y2 = dayKhungCua;
	// Thanh bên trên khung cửa sổ
	ctm = Translate(x2 * 0.5, yCuaSoTP - y2 * 0.5, -z * 0.5) * Scale(x2, y2, z);
	createElement(ctm);

	// Thanh bên dưới khung cửa sổ
	ctm = Translate(x2 * 0.5, y2 * 0.5, -z * 0.5) * Scale(x2, y2, z);
	createElement(ctm);
}

GLfloat xCanhCuaSo = zCuaSoTP / 2 - dayKhungCua + dayCanhCuaSo, yCanhCuaSo = yCuaSoTP - dayKhungCua * 2, zCanhCuaSo = dayCanhCuaSo;

void canhCuaSo(GLfloat tX, GLfloat tY, GLfloat tZ) {
	setColor(colorWood);
	modelStack.push(model);
	model *= Translate(tX, tY, tZ);

	// khung trên cánh cửa sổ
	GLfloat x1 = xCanhCuaSo, y1 = dayCanhCuaSo, z = dayCanhCuaSo;
	ctm = Translate(x1 * 0.5, yCanhCuaSo - y1 * 0.5, -z * 0.5) * Scale(x1, y1, z);
	createElement(ctm);

	// khung dưới cánh cửa sổ
	ctm = Translate(x1 * 0.5, y1 * 0.5, -z * 0.5) * Scale(x1, y1, z);
	createElement(ctm);

	// khung trái cánh cửa sổ
	GLfloat x2 = dayCanhCuaSo, y2 = yCanhCuaSo;
	ctm = Translate(x2 * 0.5, y2 * 0.5, -z * 0.5) * Scale(x2, y2, z);
	createElement(ctm);

	// khung phải cánh cửa sổ
	ctm = Translate(xCanhCuaSo - x2 * 0.5, y2 * 0.5, -z * 0.5) * Scale(x2, y2, z);
	createElement(ctm);

	setColor(colorGlass);
	GLfloat x3 = xCanhCuaSo - 2 * x2, y3 = yCanhCuaSo - 2 * y1, z3 = dayKinh;
	ctm = Translate(x3 * 0.5 + x2, y3 * 0.5 + y1, -z * 0.5) * Scale(x3, y3, z3);
	createElement(ctm);

	model = modelStack.pop();
}

GLfloat openWindow = 0;

void cuaSo(mat4 ins) {
	modelStack.push(model);
	model *= ins;

	khungCuaSo();

	canhCuaSo(dayKhungCua + openWindow, dayKhungCua, 0);
	canhCuaSo(zCuaSoTP - dayKhungCua - xCanhCuaSo - openWindow, dayKhungCua, -dayCanhCuaSo);

	model = modelStack.pop();
}

void Camera()
{
	vec4 eye(eyeX, eyeY, eyeZ, 1.0);
	vec4 at(0, 0, 0, 1);
	vec4 up(0, 1, 0, 1);

	view = LookAt(eye, at, up);
	glUniformMatrix4fv(view_loc, 1, GL_TRUE, view);

	projection = Frustum(l, r, b, t, zNear, zFar);
	glUniformMatrix4fv(projection_loc, 1, GL_TRUE, projection);
}
void display(void)
{
	Camera();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	RobotDatKeSauTang1();
	RobotDatKeSauTang2();
	CanPhong();
	CanhCua();
	cuaSo(Translate(26.5, 20, -50) * Scale(20, 20, 20));
	cuaSo(Translate(-50.5, 20, -50) * Scale(20, 20, 20));
	glutSwapBuffers();
}

void MouseButton(int type_button, int state, int x, int y)
{
	if (type_button == GLUT_LEFT_BUTTON && state == GLUT_UP && x > glutGet(GLUT_SCREEN_WIDTH) * 0.125 && x < glutGet(GLUT_SCREEN_WIDTH) * 0.2083 && y>glutGet(GLUT_SCREEN_HEIGHT) * 0.4907 && y < glutGet(GLUT_SCREEN_HEIGHT) * 0.625)
	{
		for (int i = 0; i < 20; i++)
			theta[i] = 0;
		ActiveRobot1 = false;
		ActiveRobot2 = false;
		ActiveRobot3 = false;
		ActiveRobot4 = false;
		ActiveRobot5 = false;
		ActiveRobot6 = false;
		ActiveRobot7 = false;
		ActiveRobot8 = false;

		ActiveRobot1 = true;
		//glutKeyboardFunc(keyboard);

	}
	if (type_button == GLUT_LEFT_BUTTON && state == GLUT_UP && x > glutGet(GLUT_SCREEN_WIDTH) * 0.3333 && x < glutGet(GLUT_SCREEN_WIDTH) * 0.4167 && y>glutGet(GLUT_SCREEN_HEIGHT) * 0.4907 && y < glutGet(GLUT_SCREEN_HEIGHT) * 0.625)
	{
		for (int i = 0; i < 20; i++)
			theta[i] = 0;
		ActiveRobot1 = false;
		ActiveRobot2 = false;
		ActiveRobot3 = false;
		ActiveRobot4 = false;
		ActiveRobot5 = false;
		ActiveRobot6 = false;
		ActiveRobot7 = false;
		ActiveRobot8 = false;

		ActiveRobot2 = true;
		//glutKeyboardFunc(keyboard);

	}
	if (type_button == GLUT_LEFT_BUTTON && state == GLUT_UP && x > glutGet(GLUT_SCREEN_WIDTH) * 0.542 && x < glutGet(GLUT_SCREEN_WIDTH) * 0.625 && y>glutGet(GLUT_SCREEN_HEIGHT) * 0.4907 && y < glutGet(GLUT_SCREEN_HEIGHT) * 0.625)
	{
		for (int i = 0; i < 20; i++)
		{

			X += 0.5;
			Y -= 0.1;
			Z = -2.25;
			PhongTo += 0.025;
			if (X > 0)X = 0;
			if (Y < -5) Y = -5;
			if (Z < -10) Z = -10;
			if (PhongTo > 1.5) PhongTo = 1.5;
			glutPostRedisplay();

		}
		for (int i = 0; i < 20; i++)
			theta[i] = 0;
		ActiveRobot1 = false;
		ActiveRobot2 = false;
		ActiveRobot3 = false;
		ActiveRobot4 = false;
		ActiveRobot5 = false;
		ActiveRobot6 = false;
		ActiveRobot7 = false;
		ActiveRobot8 = false;

		ActiveRobot3 = true;
		//glutKeyboardFunc(keyboard);

	}
	if (type_button == GLUT_LEFT_BUTTON && state == GLUT_UP && x > glutGet(GLUT_SCREEN_WIDTH) * 0.75 && x < glutGet(GLUT_SCREEN_WIDTH) * 0.8333 && y>glutGet(GLUT_SCREEN_HEIGHT) * 0.4907 && y < glutGet(GLUT_SCREEN_HEIGHT) * 0.625)
	{
		for (int i = 0; i < 20; i++)
			theta[i] = 0;
		ActiveRobot1 = false;
		ActiveRobot2 = false;
		ActiveRobot3 = false;
		ActiveRobot4 = false;
		ActiveRobot5 = false;
		ActiveRobot6 = false;
		ActiveRobot7 = false;
		ActiveRobot8 = false;

		ActiveRobot4 = true;
		//glutKeyboardFunc(keyboard);

	}
	if (type_button == GLUT_LEFT_BUTTON && state == GLUT_UP && x > glutGet(GLUT_SCREEN_WIDTH) * 0.125 && x < glutGet(GLUT_SCREEN_WIDTH) * 0.2083 && y>glutGet(GLUT_SCREEN_HEIGHT) * 0.1574 && y < glutGet(GLUT_SCREEN_HEIGHT) * 0.292)
	{
		for (int i = 0; i < 20; i++)
			theta[i] = 0;
		ActiveRobot1 = false;
		ActiveRobot2 = false;
		ActiveRobot3 = false;
		ActiveRobot4 = false;
		ActiveRobot5 = false;
		ActiveRobot6 = false;
		ActiveRobot7 = false;
		ActiveRobot8 = false;

		ActiveRobot5 = true;
		//glutKeyboardFunc(keyboard);

	}
	if (type_button == GLUT_LEFT_BUTTON && state == GLUT_UP && x > glutGet(GLUT_SCREEN_WIDTH) * 0.3333 && x < glutGet(GLUT_SCREEN_WIDTH) * 0.4167 && y>glutGet(GLUT_SCREEN_HEIGHT) * 0.1574 && y < glutGet(GLUT_SCREEN_HEIGHT) * 0.292)
	{
		for (int i = 0; i < 20; i++)
			theta[i] = 0;
		ActiveRobot1 = false;
		ActiveRobot2 = false;
		ActiveRobot3 = false;
		ActiveRobot4 = false;
		ActiveRobot5 = false;
		ActiveRobot6 = false;
		ActiveRobot7 = false;
		ActiveRobot8 = false;

		ActiveRobot6 = true;
		//glutKeyboardFunc(keyboard);

	}
	if (type_button == GLUT_LEFT_BUTTON && state == GLUT_UP && x > glutGet(GLUT_SCREEN_WIDTH) * 0.542 && x < glutGet(GLUT_SCREEN_WIDTH) * 0.625 && y>glutGet(GLUT_SCREEN_HEIGHT) * 0.1574 && y < glutGet(GLUT_SCREEN_HEIGHT) * 0.292)
	{
		for (int i = 0; i < 20; i++)
			theta[i] = 0;
		ActiveRobot1 = false;
		ActiveRobot2 = false;
		ActiveRobot3 = false;
		ActiveRobot4 = false;
		ActiveRobot5 = false;
		ActiveRobot6 = false;
		ActiveRobot7 = false;
		ActiveRobot8 = false;

		ActiveRobot7 = true;
		//glutKeyboardFunc(keyboard);

	}
	if (type_button == GLUT_LEFT_BUTTON && state == GLUT_UP && x > glutGet(GLUT_SCREEN_WIDTH) * 0.75 && x < glutGet(GLUT_SCREEN_WIDTH) * 0.8333 && y>glutGet(GLUT_SCREEN_HEIGHT) * 0.1574 && y < glutGet(GLUT_SCREEN_HEIGHT) * 0.292)
	{
		for (int i = 0; i < 20; i++)
			theta[i] = 0;
		ActiveRobot1 = false;
		ActiveRobot2 = false;
		ActiveRobot3 = false;
		ActiveRobot4 = false;
		ActiveRobot5 = false;
		ActiveRobot6 = false;
		ActiveRobot7 = false;
		ActiveRobot8 = false;

		ActiveRobot8 = true;
		//glutKeyboardFunc(keyboard);

	}
	else if (type_button == GLUT_RIGHT_BUTTON)
	{
		if (state == GLUT_UP)
		{
			ActiveRobot1 = false;
			ActiveRobot2 = false;
			ActiveRobot3 = false;
			ActiveRobot4 = false;
			ActiveRobot5 = false;
			ActiveRobot6 = false;
			ActiveRobot7 = false;
			ActiveRobot8 = false;
		}
	}


}

void keyboard(unsigned char key, int x, int y)
{
	// keyboard handler

	switch (key) {
	case 'q':
		openDoor += 5;
		if (openDoor >= 150) openDoor = 150;
		glutPostRedisplay();
		break;
	case 'Q':
		openDoor -= 5;
		if (openDoor < 0) openDoor = 0;
		glutPostRedisplay();
		break;
		// Ấn 1 để tăng left và tăng right tăng chiều ngang của căn phòng
	case '1':
		l *= 1.1;
		r *= 1.1;
		break;
		// Ấn 2 để giảm left và giảm right giảm chiều ngang của căn phòng
	case '2':
		l *= 0.9;
		r *= 0.9;
		break;
		// Ấn 3 để tăng top và buttom right tăng chiều cao của căn phòng
	case '3':
		b *= 1.1;
		t *= 1.1;
		break;
		// Ấn 4 để giảm top và giảm button giảm chiều cao của căn phòng
	case '4':
		b *= 0.9;
		t *= 0.9;
		break;
		// Ấn 5 để tăng Near của căn phòng (tiến gần vào căn phòng)
	case '5':
		zNear *= 1.005;
		if (zNear >= 145 && zNear < 151)zNear = 151;
		if (zNear > 175)zNear = 175;
		break;
		//Ấn 6 để giảm Near để đi lùi ra xa căn phòng
	case '6':
		zNear *= 0.995;
		if (zNear >= 145 && zNear < 151)zNear = 145;
		break;
	case '7':// Tăng  tọa độ x của mắt
		eyeX += 1;
		break;
	case '8':// Giảm  tọa độ x của mắt
		eyeX -= 1;
		break;
	case 'e':// Tăng  tọa độ x của mắt
		eyeY += 1;
		break;
	case 'E':// Giảm  tọa độ x của mắt
		eyeY -= 1;
	case 'y': //Quay robot theo trục y
		theta[0] += 5;

		if (theta[0] > 360) theta[0] -= 360;
		break;
	case 'Y': //Quay robot theo trục y ngược
		theta[0] -= 5;
		if (theta[0] > 360) theta[0] -= 360;
		break;
	case 'u': //Nhấc chân trái
		if (theta[2] < 90)
			theta[2] += 5;
		break;
	case 'U':
		if (theta[2] > 0)
			theta[2] -= 5;
		break;

	case 'm'://Nhấc chân phải
		if (theta[4] < 90)
			theta[4] += 5;
		break;
	case 'M':
		if (theta[4] > 0)
			theta[4] -= 5;
		break;

	case 'v'://Nhấc cổ chân trái
		if (theta[3] < 0)
			theta[3] += 5;
		break;
	case 'V':
		if (theta[3] > -90)
			theta[3] -= 5;
		break;
	case 'n'://Nhấc cổ chân phải
		if (theta[5] < 0)
			theta[5] += 5;
		break;
	case 'N':
		if (theta[5] > -90)
			theta[5] -= 5;
		break;

	case 'b'://Nhấc tay trái
		if (theta[6] < 180)
			theta[6] += 5;
		break;
	case 'B':
		if (theta[6] > 0)
			theta[6] -= 5;
		break;
	case 'k'://Dang tay trái
		if (theta[7] < 180)
			theta[7] += 5;
		break;
	case 'K':
		if (theta[7] > 0)
			theta[7] -= 5;
		break;

	case 'c'://Nhấc tay phải
		if (theta[8] < 180)
			theta[8] += 5;
		break;
	case 'C':
		if (theta[8] > 0)
			theta[8] -= 5;
		break;
	case 'h'://Dang tay phải
		if (theta[9] > -180)
			theta[9] -= 5;
		break;
	case 'H':
		if (theta[9] < 0)
			theta[9] += 5;
		break;

	case 'g'://Nhấc cổ tay trái
		if (theta[10] < 98)
			theta[10] += 5;
		break;
	case 'G':
		if (theta[10] > 0)
			theta[10] -= 5;
		break;
		//lower_arm2
	case 'f'://Nhấc cổ tay phải
		if (theta[11] < 98)
			theta[11] += 5;
		break;
	case 'F':
		if (theta[11] > 0)
			theta[11] -= 5;
		break;
		//neck
	case 'o'://ngửa cổ
		if (theta[12] < 30)
			theta[12] += 5;

		break;
	case 'O':
		if (theta[12] > -60)
			theta[12] -= 5;
		break;
		//head
	case 'j'://ngửa đầu
		if (theta[13] < 30)
			theta[13] += 5;
		break;
	case 'J':
		if (theta[13] > -60)
			theta[13] -= 5;
		break;
	case 'w':
		openWindow += dr * 0.02;
		if (openWindow > (xCanhCuaSo - 2 * dayCanhCuaSo)) openWindow = xCanhCuaSo - 2 * dayCanhCuaSo;
		break;
	case 'W':
		openWindow -= dr * 0.02;
		if (openWindow < 0) openWindow = 0;
		break;
	case 't': //cử động tay phải chân trái
		theta[13] = -15;
		if (theta[2] < 90)
		{
			theta[2] += 5;
			theta[3] -= 5;
			if (theta[3] == -45)
			{
				break;
			}
		}
		if (theta[4] > 0)
		{
			theta[4] -= 5;
			theta[5] += 5;
			if (theta[5] == -30)
			{
				Z += -0.1;
				break;
			}
		}
		if (theta[11] < 60 && theta[8] < 60)
		{
			theta[11] += 5;
			theta[8] += 5;
		}
		if (theta[6] > 0 && theta[10] > 0)
		{
			theta[6] -= 5;
			theta[10] -= 5;
		}
		if (ActiveRobot3 && theta[0] == 90) {
			if (theta[2] < 90)X -= 0.3;
			if (X < -30) {
				X = -30;
			}
		}
		if (ActiveRobot3 && X == -30) {
			theta[0] = -90;
		}
		if (ActiveRobot3 && theta[0] != 90) {
			theta[0] = -90;
			if (theta[2] < 90)X += 0.3;
			if (X > 30) {
				X = 30;
			}
		}
		if (ActiveRobot3 && X == 30) {
			theta[0] = 90;
		}


		break;
	case 'p'://cử động tay trái chân phải
		theta[13] = -15;
		if (theta[2] > 0)
		{
			theta[2] -= 5;
			theta[3] += 5;
			if (theta[3] == -30)
			{
				Z += -0.1;
				break;
			}
		}
		if (theta[4] < 90)
		{
			theta[4] += 5;
			theta[5] -= 5;
			if (theta[5] == -45)
			{
				break;
			}

		}
		if (theta[10] < 60 && theta[6] < 60)
		{
			theta[10] += 5;
			theta[6] += 5;
		}
		if (theta[8] > 0 && theta[11] > 0)
		{
			theta[8] -= 5;
			theta[11] -= 5;
		}
		if (ActiveRobot3 && theta[0] == 90) {
			if (theta[4] < 90) X -= 0.3;
			if (X < -30) {
				X = -30;
			}
		}
		if (ActiveRobot3 && X == -30) {
			theta[0] = -90;
		}
		if (ActiveRobot3 && theta[0] != 90) {
			theta[0] = -90;
			if (theta[4] < 90)X += 0.3;
			if (X > 30) {
				X = 30;
			}
		}
		if (ActiveRobot3 && X == 30) {
			theta[0] = 90;
		}

		break;
	case ' ':
		//reset view volume
		l = -45, r = 45, b = -45, t = 45, zNear = 100, zFar = 300;
		eyeX = 0, eyeY = 0, eyeZ = -200;
		//reset robot3
		X = -10, Y = 0, Z = 35, PhongTo = 1;
		//reset active robot
		ActiveRobot1 = false;
		ActiveRobot2 = false;
		ActiveRobot3 = false;
		ActiveRobot4 = false;
		ActiveRobot5 = false;
		ActiveRobot6 = false;
		ActiveRobot7 = false;
		ActiveRobot8 = false;
		//reset cánh cửa
		openDoor = 0;
		//reset robot action
		for (int i = 0; i <= 15; i++) {
			theta_Robot1[i] = 0;
			theta_Robot2[i] = 0;
			theta_Robot3[i] = 0;
			theta_Robot4[i] = 0;
			theta_Robot5[i] = 0;
			theta_Robot6[i] = 0;
			theta_Robot7[i] = 0;
			theta_Robot8[i] = 0;
		}
		break;
	default:
		break;
	}
	if (ActiveRobot1) {
		for (int i = 0; i < 20; i++)
			theta_Robot1[i] = theta[i];
	}
	if (ActiveRobot2) {
		for (int i = 0; i < 20; i++)
			theta_Robot2[i] = theta[i];
	}
	if (ActiveRobot3) {
		for (int i = 0; i < 20; i++)
			theta_Robot3[i] = theta[i];
	}
	if (ActiveRobot4) {
		for (int i = 0; i < 20; i++)
			theta_Robot4[i] = theta[i];
	}
	if (ActiveRobot5) {
		for (int i = 0; i < 20; i++)
			theta_Robot5[i] = theta[i];
	}
	if (ActiveRobot6) {
		for (int i = 0; i < 20; i++)
			theta_Robot6[i] = theta[i];
	}
	if (ActiveRobot7) {
		for (int i = 0; i < 20; i++)
			theta_Robot7[i] = theta[i];
	}
	if (ActiveRobot8) {
		for (int i = 0; i < 20; i++)
			theta_Robot8[i] = theta[i];
	}
	glutPostRedisplay();
}


int main(int argc, char** argv)
{

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(640, 640);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Draw Robot");
	glutFullScreen();

	glewInit();

	generateGeometry();
	initGPUBuffers();
	shaderSetup();

	glutDisplayFunc(display);
	glutMouseFunc(MouseButton);
	glutKeyboardFunc(keyboard);


	glutMainLoop();
	return 0;
}
