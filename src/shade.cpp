#include "../inc/shade.hpp"

using std::string;

namespace View {
	bool readFile(const char *fname, string &lines) {
		std::ifstream file(fname);
		if(file.is_open()) {
			string line;
			while(!file.eof() && std::getline(file, line)) {
				lines.append(line + "\r\n");
			}
			file.close();
			return true;
		}
		return false;
	}

	bool compile(const char *fname, GLuint &shader) {
		int len;
		GLint status = GL_FALSE;
		string lines = "";
		if(readFile(fname, lines)) {
			const char *cs = lines.c_str();
			glShaderSource(shader, 1, (const GLchar **)&cs, NULL);
			glCompileShader(shader);
			glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
			if(len > 1) {
				char msg[len+1];
				msg[len] = '\0';
				glGetShaderInfoLog(shader, len, NULL, msg);
				std::cout << "GL shader info log: \r\n\t"
					<< msg << std::endl;
			}
		}
		return status == GL_TRUE;
	}

	bool link(const char *vertName, 
			const char *fragName, GLuint &program) {
		bool success = false;
		GLint len, status;
		GLuint vert = glCreateShader(GL_VERTEX_SHADER),
			frag = glCreateShader(GL_FRAGMENT_SHADER);
		if((success = compile(vertName, vert))) {
			if((success = compile(fragName, frag))) {
				glAttachShader(program, vert);
				glAttachShader(program, frag);
				glLinkProgram(program);
				glGetProgramiv(program, 
						GL_LINK_STATUS, &status);
				success = status == GL_TRUE;
				glGetProgramiv(program, 
						GL_INFO_LOG_LENGTH, &len);
				if(len > 1) {
					char msg[len+1];
					msg[len] = '\0';
					glGetProgramInfoLog(program, 
							len, NULL, msg);
					std::cout << "GL program info log: \r\n\t"
						<< msg << std::endl;
				}
				glDeleteShader(frag);
			}
			glDeleteShader(vert);
		}
		return success;
	}

}