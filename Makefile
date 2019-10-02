bundle_version  := v0.1
bundle_name     := cltsa.$(bundle_version)
bundle_dir      := ../
bundle_tmp_dir  := ../tmp

doc_file_dir    := doc/definitions

src_files_dir   := src
src_files       := henos.lex\
		henos.yacc\
		main.c\
		automaton.h\
		automaton.c\
		automaton_context.h\
		automaton_context.c\
		automaton_utils.h\
		automaton_utils.c\
		parser_utils.h\
		parser_utils.c\
		fast_pool.h\
		fast_pool.c\
		ordered_list.h\
		ordered_list.c\
		bucket_list.h\
		bucket_list.c\
		max_heap.h\
		max_heap.c\
		dictionary.h\
		dictionary.c\
		obdd.h\
		obdd.c\
		obdd_cache.c\
		bool_array_hash_table.h\
		bool_array_hash_table.c\
		automaton_mu_calculus.h\
		automaton_mu_calculus.c\
		minuint.h\
		readme.md\

tmp_src_dir         := ./src_tmp
tmp_src_clean_dir   := ./src_tmp_clean

.PHONY: all doc

all: bundle

doc:
	make -C $(doc_file_dir)

create_tmp_dir: clean
	mkdir -p $(bundle_tmp_dir)
	mkdir -p $(bundle_tmp_dir)/doc
	mkdir -p $(bundle_tmp_dir)/src
	mkdir -p $(bundle_tmp_dir)/src/tests
	mkdir -p $(bundle_tmp_dir)/src/results
	mkdir -p $(tmp_src_dir)
	mkdir -p $(tmp_src_clean_dir)

copy_src_files: $(src_files)

copy_doc_files: doc
	cp $(doc_file_dir)/definitions.pdf $(bundle_tmp_dir)/doc/definitions.pdf
	cp readme.md $(bundle_tmp_dir)/readme.md
	cp readme.html $(bundle_tmp_dir)/readme.html

copy_makefile:
	cp $(src_files_dir)/Makefile_bundle $(tmp_src_dir)/Makefile

copy_result_files:
	cp -r $(src_files_dir)/tests $(bundle_tmp_dir)/src

clean_src_files:
	cp $(src_files_dir)/Makefile_bundle $(tmp_src_dir)/Makefile

	cp -r $(tmp_src_clean_dir)/ $(tmp_src_dir)/* $(bundle_tmp_dir)/src

	rm -rf $(tmp_src_dir)
	rm -rf $(tmp_src_clean_dir)

bundle: create_tmp_dir copy_src_files copy_makefile copy_doc_files clean_src_files copy_result_files
	mv $(bundle_tmp_dir) $(bundle_dir)/$(bundle_name)

	tar -czf $(bundle_dir)/$(bundle_name).tar.gz $(bundle_dir)/$(bundle_name)

	rm -rf $(bundle_dir)/$(bundle_name)

$(src_files):
	cp $(src_files_dir)/$@ $(tmp_src_dir)/$@

clean:
	rm -rf $(bundle_tmp_dir)
	rm -rf $(bundle_dir)/$(bundle_name)
	rm -rf $(bundle_dir)/$(bundle_name).tar.gz
