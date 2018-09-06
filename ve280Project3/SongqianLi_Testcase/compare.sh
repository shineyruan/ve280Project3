#!/bin/bash
for filename in `ls case`
do  
  ./p3 species case/$filename 100 v > resultv/$filename
  diff resultv/$filename ans/$filename > $filename
  if [ $? = 0 ]; then
		echo "$filename: SUCCESS!"
	else
		echo "$filename: FAIL!"
	fi
done
exit 0