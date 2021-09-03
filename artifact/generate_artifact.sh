#!/bin/bash
CONF_NAME="ICSE2022"
AUTHORS="marian[^ ]*|seba[^ ]*|naha[^ ]*|vict[^ ]*|lafhis[^ ]*|[^ ]*ippo[^ ]*|dani[^ ]*|ciolek[^ ]*|maoz[^ ]*|vscorza[^ ]*"

cd ..

echo "[[Creating  ${CONF_NAME} artifact]]"
echo "[[Removing old files]]"
rm -r "${CONF_NAME}_artifact" 2> /dev/null
rm "${CONF_NAME}"_artifact.tar.gz 2> /dev/null

echo "[[Creating tmp folders]]"
mkdir "${CONF_NAME}_artifact" "${CONF_NAME}_artifact/tests" "${CONF_NAME}_artifact/results" "${CONF_NAME}_artifact/tests/expected_output"

src_folders=(src src/)

echo "[[Copying benchmark script]]"
cp artifact/run_test_cases.sh "${CONF_NAME}"_artifact/run_test_cases.sh
sed -i "s/CONFNAME/${CONF_NAME}/g" "${CONF_NAME}"_artifact/run_test_cases.sh
chmod +x "${CONF_NAME}"_artifact/run_test_cases.sh

echo "[[Copying readme]]"
cp artifact/artifact_readme.md "${CONF_NAME}"_artifact/readme.md
sed -i "s/CONFNAME/${CONF_NAME}/g" "${CONF_NAME}"_artifact/readme.md
cp artifact/artifact_readme.md "${CONF_NAME}"_artifact/README
sed -i "s/CONFNAME/${CONF_NAME}/g" "${CONF_NAME}"_artifact/README
cp artifact/artifact_requirements.md "${CONF_NAME}"_artifact/REQUIREMENTS
sed -i "s/CONFNAME/${CONF_NAME}/g" "${CONF_NAME}"_artifact/REQUIREMENTS
cp artifact/artifact_install.md "${CONF_NAME}"_artifact/INSTALL
sed -i "s/CONFNAME/${CONF_NAME}/g" "${CONF_NAME}"_artifact/INSTALL


echo "[[Creating syntax files]]"
cp src/henos.lex "${CONF_NAME}"_artifact/henos.lex
cp src/henos.yacc "${CONF_NAME}"_artifact/henos.yacc
lex "${CONF_NAME}"_artifact/henos.lex 2> /dev/null
yacc -d -v "${CONF_NAME}"_artifact/henos.yacc 2> /dev/null

echo "[[Copying files to artifact folder]]"
for i in src/*.{c,h,md}; do
	[ -f "$i" ] || break
	cp $i "${CONF_NAME}"_artifact/$(basename $i)
	sed -i "s/mariano/XXX/g" "${CONF_NAME}_artifact/"$(basename $i)
done
for i in src/tests/*.fsp; do
	[ -f "$i" ] || break
	cp $i "${CONF_NAME}"_artifact/tests/$(basename $i)
	sed -i "s/mariano/XXX/g" "${CONF_NAME}_artifact/tests/"$(basename $i)
done
for i in src/tests/expected_output/*.exp; do
	[ -f "$i" ] || break
	cp $i "${CONF_NAME}"_artifact/tests/expected_output/$(basename $i)
	sed -i "s/mariano/XXX/g" "${CONF_NAME}_artifact/tests/expected_output/"$(basename $i)
done

cp src/Makefile "${CONF_NAME}"_artifact/Makefile

echo "[[Creating tars]]"
tar czf "${CONF_NAME}"_artifact.tar.gz "${CONF_NAME}_artifact"

#rm -r "${CONF_NAME}_artifact" 2> /dev/null
#rm "${CONF_NAME}"_artifact.tar.gz 2> /dev/null

