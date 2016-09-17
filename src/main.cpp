#include <iostream>

#include "main.hpp"
#include "util.hpp"
#include "system/printer.hpp"

#ifndef OBJ_PATH
#define OBJ_PATH "share/sphere.obj"
#endif
/*#ifndef MTL_PATH
#define MTL_PATH "share/cube.mtl"
#endif*/
#ifndef VERT_PATH
#define VERT_PATH "share/fallback.vert"
#endif
#ifndef FRAG_PATH
#define FRAG_PATH "share/shade.frag"
#endif

void printErrors(std::ostream &oss) {}

template<typename T1, typename... TN>
void printErrors(std::ostream &oss, T1 &t1, TN &... tn) {
	for(auto e : t1) {
		oss << e << std::endl;
	}
	printErrors(oss, tn...);
}

int main(int argc, const char **argv) {
	using namespace Util;
	using namespace System;
	if(glfwInit() == 0) {
		std::cout << "Failed to initialize GLFW "
			<< glfwGetVersionString() << std::endl;
		return 1;
	}
	
	const char *obj_fname, *vert_fname, *frag_fname;

	if(argc >= 2) obj_fname = argv[1];
	else obj_fname = OBJ_PATH;
	/*if(argc >= 3) mtl_fname = argv[2];
	else mtl_fname = MTL_PATH;*/
	if(argc >= 4) vert_fname = argv[2];
	else vert_fname = VERT_PATH;
	if(argc >= 5) frag_fname = argv[3];
	else frag_fname = FRAG_PATH;

	std::atomic_bool alive(true);
	Control::control ctl(alive, obj_fname);
	// Control::control ctl(alive, 0); // Use mesh instead
	if(!alive) {
		std::cout << "Control construction failed." << std::endl;
		printErrors(std::cout, ctl.viewer.errors, ctl.errors);
		return 1;
	}
	if(!ctl.viewer.setProg(alive, vert_fname, frag_fname)) {
		std::cout << "Failed to compile or link shaders." << std::endl;
		printErrors(std::cout, ctl.viewer.errors, ctl.errors);
		return 1;
	}
	using namespace System;
	Printer<5> printer;
	std::string cols[]{"GLFW", "OpenGL", "Path"},
		rows[]{"", "Major", "Minor", "Revision", "",
			"", "Wavefront obj", "Vertex shader", "Fragment shader", ""},
		paths[]{obj_fname, vert_fname, frag_fname};
	int versions[6]{0};
	glfwGetVersion(&versions[0], &versions[2], &versions[4]);
	glGetIntegerv(GL_MAJOR_VERSION, &versions[1]);
	glGetIntegerv(GL_MINOR_VERSION, &versions[3]);
	printer.push(&rows[5], &rows[5]+5)
		.level().insert(0, " ").level()
		.push<std::string, 3, 1, 31>(paths, &cols[2], &cols[3]+1)
		.level().insert(0, "   ").level()
		.push(&rows[0], &rows[5])
		.level().insert(0, " ").level()
		.push<int, 3, 2>(versions, &cols[0], &cols[2]);
	std::cout << printer << std::endl;

	if(!task::init(alive, &ctl)) {
		std::cout << "Control Initialization failed." << std::endl;
		printErrors(std::cout, ctl.viewer.errors, ctl.errors);
		return 1;
	}
	if(!task::run(alive, &ctl)) {
		printErrors(std::cout, ctl.viewer.errors, ctl.errors);
	}

	glfwTerminate();

	return 0;
}
