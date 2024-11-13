#include <parse_arguments.h>
#include <run_workload.h>
#include <db_env.h>

int main(int argc, char *argv[]) {
  DBEnv env;

  ParseArguments(argc, argv, env);
  RunWorkload(env);

  return 0;
}
