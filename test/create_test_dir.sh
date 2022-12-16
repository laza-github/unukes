#!/bin/sh
temp_dir="`mktemp -d /tmp/unukes_dir.XXXXXXXXXX`"
if [ "$?" != 0 ]; then
    echo "unable to create temp file"
    exit 1
fi
text_file="`mktemp ${temp_dir}/unukes_text_file.XXXXXXXXXX`"
echo "temp dir ${temp_dir}"
echo "text file ${text_file}"

dd if=/dev/zero of=${text_file} bs=1024 count=20
cp ${text_file} ${temp_dir}/test_unukes.txt
cp ${text_file} ${temp_dir}/test_unukes.text
cd ${temp_dir}
for a in 0 1 2 3 4 5 6 7 8 9
do
  for b in 0 1 2 3 4 5 6 7 8 9
  do
    mkdir -p ${a}/${a}
    cp ${text_file} ${a}/${a}/${a}.txt
    cp ${text_file} ${a}/${a}/${b}.txt
    cp ${text_file} ${a}/${a}/${a}.text
    cp ${text_file} ${a}/${a}/${b}.text
    mkdir -p ${a}/${b}
    cp ${text_file} ${a}/${b}/${a}.txt
    cp ${text_file} ${a}/${b}/${b}.txt
    cp ${text_file} ${a}/${b}/${a}.text
    cp ${text_file} ${a}/${b}/${b}.text
    mkdir -p ${b}/${a}
    cp ${text_file} ${b}/${a}/${a}.txt
    cp ${text_file} ${b}/${a}/${b}.txt
    cp ${text_file} ${b}/${a}/${a}.text
    cp ${text_file} ${b}/${a}/${b}.text
    mkdir -p ${b}/${b}
    cp ${text_file} ${b}/${b}/${a}.txt
    cp ${text_file} ${b}/${b}/${b}.txt
    cp ${text_file} ${b}/${b}/${a}.text
    cp ${text_file} ${b}/${b}/${b}.text
    mkdir -p ${a}/${b}/${a}
    cp ${text_file} ${a}/${b}/${a}/${a}.txt
    cp ${text_file} ${a}/${b}/${a}/${b}.txt
    cp ${text_file} ${a}/${b}/${a}/${a}.text
    cp ${text_file} ${a}/${b}/${a}/${b}.text
    mkdir -p ${b}/${a}/${b}
    cp ${text_file} ${b}/${a}/${b}/${a}.txt
    cp ${text_file} ${b}/${a}/${b}/${b}.txt
    cp ${text_file} ${b}/${a}/${b}/${a}.text
    cp ${text_file} ${b}/${a}/${b}/${b}.text
  done
done

