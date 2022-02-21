#include "cltsa_runner.h"

void print_help() {
  printf("CLTS modeling and synthesis tool usage\n");
  printf("\t-h\tprints this help message\n");
  printf("\t-r [-o outputdir] [-s suffix] filename case_name [...] \t "
         "interprets specification(s) located at filename with name case_name\n"
         "\t\t if -o provided output will be sent to [outputdir]\n"
         "\t\t if -s provided output will be added suffix [suffix]\n");
  printf("\t--all-tests\t runs all tests\n");
  printf("\t--func-tests\t runs functional tests\n");
  printf("\t--load-tests\t runs load tests\n");
}

int main(int argc, char **argv) {
  int32_t i;
  if (argc > 1) {
    if (strcmp(argv[1], "-h") == 0)
      print_help();
    if (strcmp(argv[1], "--all-tests") == 0)
      run_all_tests();
    if (strcmp(argv[1], "--func-tests") == 0)
      run_functional_tests();
    if (strcmp(argv[1], "--load-tests") == 0)
      run_load_tests();
    char name_buff[700];
    if (argc > 2) {
      if (strcmp(argv[1], "-r") == 0) {
        uint32_t initial_index = 2;
        uint32_t next_index = 2;
        char const *folder;
        char *suffix = NULL;
        if (strcmp(argv[2], "-o") == 0) {
          folder = argv[3];
          initial_index = 4;
          next_index = 4;
        } else {
          folder = getenv("TMPDIR");
          if (folder == 0) {
            folder = "/tmp";
          }
        }
        if (strcmp(argv[next_index], "-s") == 0) {
          suffix = argv[next_index + 1];
          initial_index = next_index + 2;
          next_index += 2;
        }
        if (((argc - initial_index) % 2) != 0) {
          printf("Remember to provide filename and case name for each spec, "
                 "found %d elements after config. params.\n",
                 argc - initial_index);
          exit(-1);
        }

        // remove previous rep files
        DIR *di;
        char *ptr1, *ptr2, *last = NULL;
        int retn;
        struct dirent *dir;
        char curr_file[1024];
        di = opendir(folder); // specify the directory name
        if (di) {
          while ((dir = readdir(di)) != NULL) {
            last = NULL;
            snprintf(curr_file, sizeof(curr_file), "%s/%s", folder, dir->d_name);
            ;
            strtok(dir->d_name, ".");
            ptr2 = strtok(NULL, ".");
            while (ptr2 != NULL) {
              last = ptr2;
              ptr2 = strtok(NULL, ".");
            }
            if (last != NULL) {
              if (strcmp(last, "rep") == 0 || strcmp(last, "fsp") == 0) {
                remove(curr_file);
#if VERBOSE
                printf("Removing %s\n", curr_file);
#endif
              }
            }
          }
          closedir(di);
        }
        char result_buff[255], steps_buff[255];
        char *result_name;
        for (i = 0; i < ((argc - initial_index) / 2); i++) {
          result_name = strrchr(argv[initial_index + i * 2], '/');
          if (result_name == NULL)
            result_name = argv[initial_index + i * 2];
          else
            result_name++;
          if (i == 0) {
            snprintf(result_buff, sizeof(result_buff), "%s/%s%s", folder, result_name, suffix != NULL ? suffix : "");
          }
          snprintf(steps_buff, sizeof(steps_buff), "%s/%s%s_steps", folder, result_name, suffix != NULL ? suffix : "");
          run_parse_test_local(argv[initial_index + i * 2], argv[initial_index + i * 2 + 1], result_buff, steps_buff, DD_SEARCH, i != 0);
        }
      }
    }
  } else {
    print_help();
    //run_parse_test("tests/alphabet_relabel_test.fsp", "Alphabet Extend");
    //run_minimization_tests();
  }
  obdd_mgr *mgr = parser_get_obdd_mgr();
  obdd_mgr_destroy(mgr);

  return 0;
}
