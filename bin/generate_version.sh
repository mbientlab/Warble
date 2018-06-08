#!/usr/bin/env bash

describe=$(git describe --long --always)
version=${describe%-*[^-]}
if [ $describe = $version ];
then
    version="0.0.0"
fi
version=${version/-/.}

split=(${version//\./ })
step=0

if [ ${#split[@]} -eq "4" ];
then
    let step+=${split[2]}
    let step+=${split[3]}
    let split[2]=$step
fi

printf "VERSION=%s\nVERSION_MAJOR=%s\nVERSION_MINOR=%s\nVERSION_STEP=%s\n"\
        "${split[0]}.${split[1]}.${split[2]}"\
        "${split[0]}" "${split[1]}" "${split[2]}" > $1
