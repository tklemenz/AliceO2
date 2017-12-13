#include <cstdlib>
#include <unistd.h>
#include <sstream>
#include <iostream>

// helper exec to launch the devices
int main(int argc, char* argv[]) {
	std::string binpath(getenv ("O2_ROOT"));
	binpath+="/bin";
	auto home = getenv ("HOME");

	std::stringstream configss;
	configss << home << "/alisw_new/O2/run/primary-server.json";

   // the server
   int pid = fork();
   if (pid==0) {
	  const std::string name("O2PrimaryServerDeviceRunner");
	  const std::string path = binpath + "/" + name;
	  execl(path.c_str(), name.c_str(), "--control", "static", "--id", "primary-server", "--mq-config", configss.str().c_str(),
			(char *)0);
	}

    int nworkers = 1;
    if(argc > 1) {
      nworkers = atoi(argv[1]);
    }
    for(int id = 0; id < nworkers; ++id){
    // the workers
    pid = fork();
    if(pid==0) {
    	const std::string name("O2SimDeviceRunner");
    	      const std::string path = binpath + "/" + name;
	  execl(path.c_str(), name.c_str(), "--control", "static", "--id", "worker", "--mq-config", configss.str().c_str(),
		    (char *)0);
    }
    }


}
