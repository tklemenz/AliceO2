#include <cstdlib>
#include <unistd.h>
#include <sstream>
#include <iostream>
#include <cstdio>
#include <fcntl.h>
#include <sys/wait.h>

// helper exec to launch the devices
int main(int argc, char* argv[]) {
  std::string binpath(getenv("O2_ROOT"));
  binpath += "/bin";
  auto home = getenv("HOME");

  std::stringstream configss;
  configss << home << "/alisw_new/O2/run/primary-server.json";

  // the server
  int pid = fork();
  if (pid == 0) {
    int fd = open("serverlog", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    dup2(fd, 1); // make stdout go to file
    dup2(fd, 2); // make stderr go to file - you may choose to not do this
                 // or perhaps send stderr to another file
    close(fd); // fd no longer needed - the dup'ed handles are sufficient

    const std::string name("O2PrimaryServerDeviceRunner");
    const std::string path = binpath + "/" + name;
    execl(path.c_str(), name.c_str(), "--control", "static", "--id", "primary-server", "--mq-config",
          configss.str().c_str(), (char*)0);
  }

  int nworkers = 1;
  if (argc > 1) {
    nworkers = atoi(argv[1]);
  }
  for (int id = 0; id < nworkers; ++id) {
    std::stringstream ss;
    ss << "workerlog" << id;
	int fd = open(ss.str().c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    dup2(fd, 1); // make stdout go to file
    dup2(fd, 2); // make stderr go to file - you may choose to not do this
	                 // or perhaps send stderr to another file
    close(fd); // fd no longer needed - the dup'ed handles are sufficient

	  // the workers
    pid = fork();
    if (pid == 0) {
      const std::string name("O2SimDeviceRunner");
      const std::string path = binpath + "/" + name;
      execl(path.c_str(), name.c_str(), "--control", "static", "--id", "worker", "--mq-config", configss.str().c_str(),
            (char*)0);
    }
  }

  // the hit merger
  int status, cpid;
  pid = fork();
  if (pid == 0) {
    int fd = open("mergerlog", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    dup2(fd, 1); // make stdout go to file
    dup2(fd, 2); // make stderr go to file - you may choose to not do this
                 // or perhaps send stderr to another file
    close(fd);   // fd no longer needed - the dup'ed handles are sufficient

    const std::string name("O2HitMergerRunner");
    const std::string path = binpath + "/" + name;
    execl(path.c_str(), name.c_str(), "--control", "static", "--id", "hitmerger", "--mq-config", configss.str().c_str(),
          (char*)0);
  } else {
    if ((cpid = wait(&status)) == pid) {
      printf("Child %d returned\n", pid);
    }
  }
}
