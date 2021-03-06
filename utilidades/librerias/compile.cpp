#include "compile.h"

void Compile::buscarCompilador() {
#ifdef WIN32
	
	//PROBAMOS VS19
	HKEY clave;
	LONG respuesta = RegOpenKeyEx(HKEY_CLASSES_ROOT, "VisualStudio.DTE.16.0\\CLSID", 0, KEY_READ, &clave);
	bool correcto = respuesta == ERROR_SUCCESS;
	if (!correcto) {
		bool noExiste = respuesta == ERROR_FILE_NOT_FOUND;
	}

	std::string CLSID = "";
	leerRegistro(clave, "", CLSID);
	if (!CLSID.empty()) {
		//Tenemos el ID de la clave.
		respuesta = RegOpenKeyEx(HKEY_CLASSES_ROOT, (std::string("WOW6432Node\\CLSID\\")+CLSID+"\\LocalServer32").c_str(), 0, KEY_READ, &clave);
		bool correcto = respuesta == ERROR_SUCCESS;
		if (!correcto) {
			bool noExiste = respuesta == ERROR_FILE_NOT_FOUND;
		}
		//std::string ejecutable = "";
		leerRegistro(clave, "", rutaCompilador);
		//Sacamos la ruta del 
		if (!rutaCompilador.empty()) {
			
			std::filesystem::path ruta((rutaCompilador[0]!='\"')?rutaCompilador:rutaCompilador.substr(1,rutaCompilador.size()-2));
			std::string rutaBat=ruta.parent_path().string() + "\\..\\..\\VC\\Auxiliary\\Build\\vcvarsall.bat";
			if (std::filesystem::exists(rutaBat.c_str())) {
				entornoCompilador = std::string("\"")+rutaBat + std::string("\"") + " x86_amd64";
			}
		}
	}
	



#endif // WIN32


}

void Compile::leerRegistro(HKEY clave, const char* subClave, std::string& valor) {
	char* buffer[512];
	DWORD tama�oBuffer = sizeof(buffer);
	ULONG error;
	error = RegQueryValueEx(clave, subClave, 0, NULL, (LPBYTE)buffer, &tama�oBuffer);
	if (error == ERROR_SUCCESS) {
		valor = std::string((char*)buffer);
	}
}

void Compile::leerRegistro(HKEY clave, const char* subClave, bool& valor) {
	
}

void Compile::compileProject(const char* project, Types tipo) {
	std::string comando;
	int i;
	switch (tipo) {
	case Compile::NINJA:
#ifdef WIN32
		if (!std::filesystem::exists(std::string(project)+"/build.ninja")) {
			generateProject(project, tipo, false);
		}
		comando= std::string(compilerScope()) + " && cd  \"" + std::string(project) + "\"&&ninja" + +" 1>c.txt 2>e.txt";
		for (int j = 0; j < comando.length(); j++) {
			if (comando[j] == '/') {
				comando[j] = '\\';
			}

		}

		WinExec(comando.c_str(),0);
		//std::this_thread::sleep_for(std::chrono::milliseconds(1000));
#else
		if (!system(NULL)) {
			return;
		}
		//TODO: CAMBIAR ESTO PARA QUES ALGO M�S EFICIENTE
		comando = std::string(entornoCompilador) + " && cd  " + std::string(project)+ "&&ninja" +  + " 1>c.txt 2>e.txt";
		for (int j = 0; j < comando.length(); j++) {
			if (comando[j] == '/') {
				comando[j] = '\\';
			}

		}
		i=system(comando.c_str());
#endif

		break;
	default:
		break;
	}
	


}

void Compile::generateProject(const char* project, Types tipo,bool generar) {
	std::string comando;
	int i;
	switch (tipo) {
	case Compile::NINJA:
		comando = std::string(compilerScope()) + " && cd  \"" + std::string(project) + (generar?"\\build":"") +"\"&&cmake -G \"Ninja\" .." + +" 1>c.txt 2>e.txt";
#ifdef WIN32
		
		for (int j = 0; j < comando.length(); j++) {
			if (comando[j] == '/') {
				comando[j] = '\\';
			}

		}

		WinExec(comando.c_str(), 0);
		//std::this_thread::sleep_for(std::chrono::milliseconds(1000));
#else
		if (!system(NULL)) {
			return;
		}
		//TODO: CAMBIAR ESTO PARA QUES ALGO M�S EFICIENTE
		for (int j = 0; j < comando.length(); j++) {
			if (comando[j] == '/') {
				comando[j] = '\\';
			}

		}
		i = system(comando.c_str());
#endif

		break;
	default:
		break;
	}
}
const char* Compile::pathCompiler() {
	if (rutaCompilador.empty()) {
		//Buscamos el compilador
		buscarCompilador();
	}
	return rutaCompilador.c_str();
}

const char* Compile::compilerScope() {
	if (rutaCompilador.empty()) {
		//Buscamos el compilador
		buscarCompilador();
	}
	return entornoCompilador.c_str();
}


std::string Compile::entornoCompilador = "";// "\"u:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Auxiliary/Build/vcvarsall.bat\" x86_amd64";
std::string Compile::rutaCompilador = "";