#include "Includes.hpp"
#include "Defines.hpp"
#include "Config.hpp"
#include "Logger.hpp"
#include "Server.hpp"
#include "Utils.hpp"
#include "Globals.hpp"

volatile sig_atomic_t stop = 0;

std::vector<Server> initializeServers(const Config& config, Logger& logger)
{
	std::vector<Server> servers;
	std::vector<ServerConfigs> serverConfigs = config.getServers();

	for (std::vector<ServerConfigs>::const_iterator it = serverConfigs.begin(); it != serverConfigs.end(); ++it)
	{
		Server server(*it, logger);
		if (!server.initialize())
		{
			throw std::runtime_error("Failed to initialize server");
		}
		servers.push_back(server);
	}

	return servers;
}

Logger *initializeLogger()
{
	return new Logger(LOG_FILE, LOG_ACCESS_FILE, LOG_ERROR_FILE);
}

Config loadConfig(const std::string &configFilePath, Logger &logger)
{
	return Config(configFilePath, logger);
}


void firstStep(char *path, Logger *logger){
	Config config = loadConfig(path, *logger);
	std::vector<Server> servers = initializeServers(config, *logger);
	for (std::vector<Server>::iterator it = servers.begin(); it != servers.end(); ++it)
	{
		usleep(100000);
		srand(time(NULL));
		pid_t pid = fork();
		if (pid < 0)
		{
			std::cerr << "Fork failed" << std::endl;
			exit(EXIT_FAILURE);
		}
		else if (pid == 0)
		{
			setupSignalForkHandlers();
			it->run();
			exit(EXIT_SUCCESS);
		}
	}
	for (std::vector<Server>::iterator it = servers.begin(); it != servers.end(); ++it)
    {
        int status;
        wait(&status);
		std::cout << "Server " << it->getUuid() << " exited with status " << status << std::endl;
    }

}



int main(int argc, char **argv)
{
	if (argc != 2)
	{
		std::cerr << "Usage: " << argv[0] << " <config_file_path>" << std::endl;
		return EXIT_FAILURE;
	}
	Logger *logger = initializeLogger();
	stop = 0;
	setupSignalHandlers();
	firstStep(argv[1], logger);
	delete logger;
	return 0;
}