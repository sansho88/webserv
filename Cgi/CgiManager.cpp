//
// Created by tgriffit on 4/3/23.
//

#include <fstream>
#include <cstdlib>
#include <cstring>
#include "CgiManager.hpp"

CgiManager::CgiManager() {

}

CgiManager::CgiManager(std::string cgiToExecute, std::map<std::string, std::string> &ServerVariablesMap)
                       : serVarMap(ServerVariablesMap) {
    execute(cgiToExecute);
}

CgiManager::CgiManager(const CgiManager &copy) {
    *this = copy;
}

CgiManager::~CgiManager() {

}

CgiManager &CgiManager::operator=(const CgiManager &toAssign) {
    this->cgiResponse = toAssign.cgiResponse;
    this->serVarMap = toAssign.serVarMap;
    return *this;
}

void CgiManager::execute(const std::string &cgiName) { //todo: modifier le env original
    int pid = fork();
    int fds_in[2], fds_out[2];
	const std::string cgiFileName("cgiFile.txt");
    convertEnvMap();
    char **env = convertEnvMap();


	std::ofstream cgiFile(cgiFileName.c_str());
	std::streambuf *oldCout = std::cout.rdbuf();
    pipe(fds_in), pipe(fds_out);

    if (pid == 0)
    {
        dup2(fds_in[0], STDIN_FILENO);
        dup2(fds_out[0], STDOUT_FILENO);
        char *args[] ={const_cast<char *>(cgiName.c_str()), NULL};
       	std::cout.rdbuf(cgiFile.rdbuf()); //dup2 like
        execve(CGI_PATH, args, env);
        std::cerr << "CGI EXCEPTION\n";
        exit(2);
    } //todo: recuperer les informations du CGI et les enregistrer dans cgiResponse;
	std::cout.rdbuf(oldCout);
	convertCgiFileToCgiResponse(cgiFileName);

    for (size_t i = 0; env[i]; i++)
        delete[] env[i];
    delete[] env;
	cgiFile.close();
}

char ** CgiManager::convertEnvMap() {
    {
        { //todo: remove quand j'aurais la vrai map post-parsing/recuperation de la methode par le serveur
            serVarMap["REQUEST_METHOD="] = "[PH]Rqst Meth"; //contient la méthode HTTP utilisée pour effectuer la requête (par exemple, GET, POST, etc.)
            serVarMap["QUERY_STRING"] = "[PH]QUERY STR";    // contient la chaîne de requête de l'URL de la requête HTTP
            serVarMap["CONTENT_TYPE"] = "[PH]text/html";    // contient le type de contenu de la requête HTTP (par exemple, application/json, multipart/form-data, etc.)
            serVarMap["CONTENT_LENGTH"] = "42";         // contient la longueur du contenu de la requête HTTP
        }
    }

    char **env = new char *[serVarMap.size()] ;
    std::map<std::string, std::string>::iterator itMap = serVarMap.begin();
    for (int i = 0; itMap != serVarMap.end() ; ++itMap, ++i) {
        env[i] = new char[itMap->second.length() + 1];
        std::strcpy(env[i], itMap->second.c_str());
    }
    return env;
}

void CgiManager::convertCgiFileToCgiResponse(const std::string &cgiFileName)
{
	std::ifstream cgiFile(cgiFileName.c_str());
	char buffer[BUFSIZ];

	if (cgiFile.is_open())
	{
		while (cgiFile.getline(buffer, BUFSIZ))
		{
			cgiResponse += buffer;
			cgiResponse += '\n';
		}
		cgiFile.close();
	}
}

const std::string &CgiManager::getCgiResponse() const
{
	return cgiResponse;
}
