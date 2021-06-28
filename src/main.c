#include "cltsa_runner.h"

void print_help(){
	printf("CLTS modeling and synthesis tool usage\n");
	printf("\t-h\tprints this help message\n");
	printf("\t-r [-o outputdir] [-s suffix] filename case_name [...] \t interprets specification(s) located at filename with name case_name\n" \
			"\t\t if -o provided output will be sent to [outputdir]\n" \
			"\t\t if -s provided output will be added suffix [suffix]\n");
	printf("\t--all-tests\t runs all tests\n");
	printf("\t--func-tests\t runs functional tests\n");
	printf("\t--load-tests\t runs load tests\n");
}

int main (int argc, char** argv){
	int32_t i;
	if(argc > 1){
		if(strcmp(argv[1], "-h") == 0)
			print_help();
		if(strcmp(argv[1], "--all-tests") == 0)
			run_all_tests();
		if(strcmp(argv[1], "--func-tests") == 0)
			run_functional_tests();
		if(strcmp(argv[1], "--load-tests") == 0)
			run_load_tests();
		char name_buff[700];
		if(argc > 2){
			if(strcmp(argv[1], "-r") == 0){
				uint32_t initial_index	= 2;
				uint32_t next_index = 2;
				char const *folder;
				char *suffix = NULL;
				if(strcmp(argv[2], "-o") == 0){
					folder	= argv[3];
					initial_index = 4;
					next_index	= 4;
				}else{
					folder = getenv("TMPDIR");
					if (folder == 0){
						folder = "/tmp";
					}
				}
				if(strcmp(argv[next_index], "-s") == 0){
					suffix	= argv[next_index + 1];
					initial_index	= next_index + 2;
					next_index += 2;
				}
				if(((argc - initial_index) % 2) != 0){
					printf("Remember to provide filename and case name for each spec, found %d elements after config. params.\n", argc - initial_index);
					exit(-1);
				}

				//remove previous rep files
			    DIR *di;
			    char *ptr1,*ptr2,*last = NULL;
			    int retn;
			    struct dirent *dir;
			    char curr_file[1024];
			    di = opendir(folder); //specify the directory name
			    if (di)
			    {
			        while ((dir = readdir(di)) != NULL)
			        {
			        	last = NULL;
			        	snprintf(curr_file, sizeof(curr_file),"%s/%s", folder, 	dir->d_name);;
			            strtok(dir->d_name,".");
			            ptr2=strtok(NULL,".");
			            while(ptr2!=NULL)
			            {
			            	last = ptr2;
			            	ptr2=strtok(NULL,".");
			            }
			            if(last != NULL){
			                if(strcmp(last,"rep")==0 || strcmp(last,"fsp") == 0)
			                {
			                    remove(curr_file);
			                    printf("Removing %s\n", curr_file);
			                }
			            }
			        }
			        closedir(di);
			    }
				char result_buff[255], steps_buff[255];
				char *result_name;
				for(i = 0; i < ((argc - initial_index) / 2); i++){
					result_name = strrchr(argv[initial_index + i * 2], '/');
					if(result_name == NULL)result_name = argv[initial_index + i * 2];
					else result_name++;
					if(i == 0){
						snprintf(result_buff, sizeof(result_buff),"%s/%s%s", folder, 	result_name,
								suffix != NULL? suffix : "");
					}
					snprintf(steps_buff, sizeof(steps_buff),"%s/%s%s_steps", folder, 	result_name,
													suffix != NULL? suffix : "");
					run_parse_test_local(argv[initial_index + i * 2], argv[initial_index + i * 2 + 1],
							result_buff, steps_buff, DD_SEARCH, i != 0);
				}
			}
		}
	}else{
		//ONGOING
		//run_parse_test("tests/genbuf_2_sndrs_simplified.fsp", "GenBuf 2 sndrs (simplified)");//GENBUF 2 Sndrs
		//run_parse_test("tests/genbuf_2_sndrs.fsp", "GenBuf 2 sndrs");//GENBUF 2 Sndrs
		//run_parse_test("tests/genbuf_2_sndrs_no_automaton.fsp", "GenBuf 2 sndrs(no automaton)");//GENBUF 2 sndrs
		//run_parse_test("tests/genbuf_4_sndrs_no_automaton.fsp", "GenBuf 4 sndrs(no automaton)");//GENBUF 4 sndrs
		//run_parse_test("tests/genbuf_8_sndrs_no_automaton.fsp", "GenBuf 8 sndrs(no automaton)");//GENBUF 8 sndrs
		//run_parse_test("tests/genbuf_4_sndrs.fsp", "GenBuf 4 sndrs");//GENBUF 4 sndrs
		//run_parse_test("tests/automata_load_test_1.fsp",  "automata load test");
		//run_parse_test("tests/automata_load_test_2.fsp",  "automata load test");
		//run_parse_test("diagnoses/ahb/ahb_2_masters/ahb_2_masters_v1.fsp", "AHB 2 Masters");//AHB 2 Masters
		//run_parse_test("diagnoses/ahb/ahb_2_masters/ahb_2_masters_simplified_v1.fsp", "AHB 2 Masters");//AHB 2 Masters
		//run_parse_test("diagnoses/syntcomp/collector/collector_2_in_v1.fsp", "Collector 2 inputs");//Collector 2 inputs
		//run_parse_test("diagnoses/syntcomp/collector/collector_3_in_v1.fsp", "Collector 3 inputs");//Collector 3 inputs
		//run_parse_test("diagnoses/syntcomp/collector/collector_4_in_v1.fsp", "Collector 4 inputs");//Collector 4 inputs
		//run_parse_test("diagnoses/syntcomp/collector/collector_5_in_v1.fsp", "Collector 5 inputs");//Collector 5 inputs
		//run_parse_test("diagnoses/syntcomp/collector/collector_6_in_v3.fsp", "Collector 6 inputs");//Collector 6 inputs
		//run_parse_test("diagnoses/syntcomp/collector/collector_9_in_v1.fsp", "Collector 9 inputs");//Collector 6 inputs
		//run_parse_test("diagnoses/syntcomp/collector/collector_3_in_missing_assumption_v1.fsp", "Collector 3 inputs");//Collector 3 inputs missing 1 assumtpion
		//run_parse_test("diagnoses/syntcomp/collector/collector_4_in_missing_assumption_v1.fsp", "Collector 4 inputs");//Collector 4 inputs missing 1 assumption
		//run_parse_test("tests/genbuf_2_sndrs_unreal_missing_assumption.fsp", "GenBuf 2 sndrs (missing assumption)");//GENBUF 2 Sndrs
		//run_parse_test("tests/genbuf_2_sndrs_unreal_missing_assumption_simplified.fsp", "GenBuf 2 sndrs (missing assumption_simplified)");
		//run_parse_test("tests/genbuf_2_sndrs_unreal_missing_assumption_no_queue.fsp", "GenBuf 2 sndrs (missing assumption no queue)");//GENBUF 2 Sndrs
		//run_parse_test("tests/genbuf_2_sndrs_no_queue.fsp", "GenBuf 2 sndrs (no queue)");//GENBUF 2 Sndrs
		//run_parse_test("tests/genbuf_2_sndrs_no_automaton_missing_assumption_simplified.fsp", "GenBuf 2 sndrs(no automaton)");
		//run_parse_test("diagnoses/mtsa/exploration_robot/exploration-robot-v1.lts", "Exploration robot");
		//run_parse_test("diagnoses/ahb/ahb_2_masters/ahb_2_masters_v1.fsp", "AHB 2 Masters");//AHB 2 Masters
		//run_parse_test("tests/genbuf_2_sndrs_no_automaton_missing_assumption_v2.fsp", "GenBuf 2 sndrs V2");
		//run_parse_test("tests/chu/shifter_2.fsp", "shifter 2 (CHU)");
		//run_parse_test("tests/chu/shifter_6.fsp", "shifter 6 (CHU)");
		//run_parse_test("tests/chu/self_correcting_2.fsp", "self correcting ring counter 2 (CHU)");
		//run_parse_test("tests/chu/self_correcting_2_v2.fsp", "self correcting ring counter 2 (CHU)");
		//run_parse_test("tests/nonreal_test_1.fsp", "non realizable test 1");
		//run_parse_test("tests/genbuf_2_sndrs_no_automaton_removed_controllable.fsp", "GenBuf 2 sndrs V2 removed controllable");
		//run_parse_test("tests/genbuf_1_sndrs_no_automaton.fsp", "GenBuf 1 sndrs V2");
		//run_parse_test("tests/img_test_1.fsp", "Img test 1");
		//run_parse_test("tests/genbuf_1_sndrs_no_automaton.fsp", "GenBuf 1 sndrs");
		//run_parse_test("tests/genbuf_2_sndrs_no_automaton.fsp", "GenBuf 2 sndrs");
		//run_parse_test_local("tests/genbuf_3_sndrs_no_automaton.fsp", "GenBuf 3 sndrs V2", "tests/genbuf_3_sndrs_no_automaton.dat", 0, true);
		//run_parse_test("tests/genbuf_4_sndrs_no_automaton.fsp", "GenBuf 3 sndrs V2");
		//run_parse_test_local("tests/genbuf_3_sndrs_no_automaton.fsp", "GenBuf 3 sndrs V2", "tests/genbuf_3_sndrs_no_automaton.dat", DD_SEARCH, false);
		//run_parse_test_local("tests/genbuf_4_sndrs_no_automaton_missing_assumption.fsp", "GenBuf 3 sndrs V2", "tests/genbuf_4_sndrs_no_automaton_missing_assumption.dat", DD_SEARCH, false);
		//run_parse_test("tests/genbuf_5_sndrs_no_automaton.fsp", "GenBuf 5 sndrs");
		//run_parse_test("tests/genbuf_6_sndrs_no_automaton.fsp", "GenBuf 6 sndrs");
		//run_parse_test("tests/genbuf_1_sndrs_simplified.fsp", "GenBuf 1 sndrs (simplified)");
		//run_parse_test("tests/mixed_3_signals_2_labels.fsp", "mixed model 3 signals 2 labels");
		//run_parse_test("tests/two_floors_lift.fsp", "lift 2 floors");//lift 2 floors
		//run_parse_test("tests/nonreal_test_1.fsp", "non realizable test 1");
		//run_parse_test("tests/genbuf_2_sndrs_no_automaton_missing_assumption.fsp", "GenBuf 2 sndrs V2 missing assumption");
		//run_parse_test("tests/genbuf_1_sndrs_no_automaton_joined_goals.fsp", "GenBuf 1 sndrs V2 joined goals");
		//run_parse_test("tests/genbuf_2_sndrs_no_automaton_joined_goals.fsp", "GenBuf 2 sndrs V2 joined goals");
		//run_parse_test_linear("tests/genbuf_2_sndrs_no_automaton_joined_goals.fsp", "GenBuf 2 sndrs V2 joined goals");
		//run_parse_test("tests/genbuf_2_sndrs_no_automaton_removed_controllable.fsp", "GenBuf 2 sndrs V2 removed controllable");
		//run_parse_test("tests/genbuf_2_sndrs_no_automaton_removed_env_safety.fsp", "GenBuf 2 sndrs V2 removed env safety");
		//run_parse_test("tests/genbuf_1_sndrs_no_automaton_removed_env_safety.fsp", "GenBuf 2 sndrs V2 removed env safety");
		//run_parse_test("tests/nonreal_test_2_konig.fsp", "non realizable test 1 konig");
		//run_parse_test("tests/genbuf_3_sndrs_no_automaton_missing_assumption_diff.fsp", "Genbuf 1 miss. ass. diff");
		//run_parse_test("tests/konighoefer_examples.fsp", "non realizable test 1 konig");
		//GENERAL TESTS
		//run_all_tests();

		//run_functional_tests();
		run_parse_test("tests/collector_1_in_v1.fsp", "Collector 2 inputs");//Collector 2 inputs

		//run_parse_test("tests/lift_controller_2.fsp", "Lift controller 2");
		//run_parse_test("tests/lift_controller_8.fsp", "Lift controller 2");
		//run_parse_test("tests/ltl_serialization_composition_test.fsp", "Serialization composition test");
		//run_parse_test("tests/seq_tests/exploration-robot-v1_12.fsp", "Exploration robot v1 (6) sequentialization test");
		//run_parse_test("tests/seq_tests/collector_6_in_v1.fsp", "Collector v1 (5) sequentialization test");
		//run_parse_test("tests/seq_tests/exploration-robot-v1_6.fsp", "Exploration robot v1 (6) sequentialization test");
		//run_parse_test("tests/seq_tests/genbuf_1_sndrs_no_automaton.fsp", "GenBuf 1 sndrs V2");
		//run_parse_test("tests/seq_tests/genbuf_4_sndrs_no_automaton.fsp", "GenBuf 4 sndrs V2");
		//run_parse_test("tests/seq_tests/lift_controller_2.fsp", "Lift controller 2");
		//run_parse_test("tests/ahb_arbiter_2_sched_automaton.fsp", "Ahb 2 aut arbiter");
		//run_parse_test("tests/ahb_arbiter_2_sched.fsp", "Ahb 2 arbiter");
		//run_parse_test("tests/seq_tests/ahb_arbiter_3_sched.fsp", "Ahb 3 arbiter");
		//run_parse_test("tests/seq_tests/ahb_arbiter_4_sched.fsp", "Ahb 4 arbiter");
		//run_parse_test("tests/seq_tests/ahb_arbiter_6_sched.fsp", "Ahb 6 arbiter");
		//run_parse_test("tests/ltl_fsp_composition_test.fsp", "LTL composition test");
		//run_parse_test("tests/ltl_serialization_composition_test.fsp", "Serialization composition test");
		//run_parse_test("tests/seq_tests/exploration-robot-v1_50.fsp", "Exploration robot v1 (6) sequentialization test");
		//run_parse_test("tests/exploration-robot-realizable_test_debug.fsp", "unrealizable exploration robot test");
		//run_parse_test("tests/exploration-robot-realizable_test_debug.fsp", "unrealizable exploration robot test");
		//run_parse_test("tests/exploration-robot-realizable_test.fsp", "unrealizable exploration robot test");
		//run_parse_test("tests/genbuf_4_sndrs_no_automaton.fsp", "GenBuf 4 sndrs(no automaton)");//GENBUF 4 sndrs
		//run_parse_test("tests/nonreal_test_1.fsp", "non realizable test 1");
		//run_parse_test("tests/mixed_3_signals_2_labels.fsp", "mixed model 3 signals 2 labels");//mixed model 3 signals 2 labels
		//run_parse_test("tests/two_floors_lift.fsp", "lift 2 floors");//lift 2 floors
		//run_parse_test("tests/nonreal_test_1_realizable_version.fsp", "non realizable test 1 (realizable version)");
		//run_parse_test("tests/nonreal_test_2_realizable_version.fsp", "non realizable test 2 (realizable version)");
		//run_parse_test("tests/nonreal_test_1.fsp", "non realizable test 1");
		//run_parse_test("tests/exploration-robot-unrealizable_test.fsp", "unrealizable exploration robot test");


		//run_parse_test("tests/biscotti.fsp", "biscotti");
		//run_parse_test("tests/current_sut.fsp", "current_SUT");
		//run_automaton_composite_hash_table_tests();
		//run_parse_test("tests/genbuf_2_sndrs_no_automaton.fsp", "GenBuf 2 sndrs");
		//TODO
		//run_parse_test("tests/half_adder_to_full_adder.fsp",  "half adder to full adder test");
		//run_parse_test("tests/two_full_adders.fsp",  "two full adders test");
		//run_parse_test("tests/automata_load_test_1.fsp",  "automata load test");
		//run_parse_test("tests/receiver_asynch_test.fsp",  "receiver asynch test");
		//run_parse_test("tests/test18.fsp",  "test18");
		//run_parse_test("tests/test26.fsp", "test26");
		//run_parse_test("tests/test27.fsp", "test27");
		//run_parse_test("tests/test32.fsp", "test32");
		//run_parse_test("tests/test34.fsp", "test34");
		//run_parse_test("tests/test40.fsp", "compositions type");
		//SHOWCASE
		//run_parse_test("tests/test21.fsp", "test21");
		//run_parse_test("tests/test29.fsp", "lift 3 floors");//lift 3 floors
		//run_parse_test("tests/test36.fsp", "lift 5 floors");//lift 5 floors
		//IN PROGRESS
		//run_parse_test("tests/test37.fsp", "lts load test 1");
		//run_parse_test("tests/test38.fsp", "lift 5 floors + 10 variables");//lift 5 floors + 10 variables
	}
	obdd_mgr* mgr	= parser_get_obdd_mgr();
	obdd_mgr_destroy(mgr);

	return 0;    
}


