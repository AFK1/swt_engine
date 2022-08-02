# my make and compile bash file... sorry

# add files to compile
file_arr=("renderGL" "constvars" "utils")


FILE=./.mymake/time.txt
FILE2=./.mymake/compiling.txt

touch $FILE2
if [ -f "$FILE" ]; then
    for fl in ${file_arr[@]}; do
        last_f=$(grep -n $fl $FILE|cut -d' ' -f2-)
        last_r=$(date +%s -r ./${fl}.cpp)
        if [ -z "$last_f" ]; then
            if [ $last_f -lt $last_r ]; then
                echo $fl update
                g++ ${fl}.cpp -lGL -lX11 -lglfw -lGLEW -c -o ./bin/${fl}.o
                echo "${fl} $last_r" >> $FILE2
            else
                echo "${fl} $last_f" >> $FILE2
            fi
        else
            g++ ${fl}.cpp -lGL -lX11 -lglfw -lGLEW -c -o ./bin/${fl}.o
            echo "${fl} $last_r" >> $FILE2
        fi
    done
    rm $FILE
    touch $FILE
    cat $FILE2 >> $FILE
    rm $FILE2
else
    touch $FILE
    for fl in ${file_arr[@]}; do
        last_r=$(date +%s -r ./${fl}.cpp)
        g++ ${fl}.cpp -lGL -lX11 -lglfw -lGLEW -c -o ./bin/${fl}.o
        newtext=$fl' '$last_r
        echo $newtext >> $FILE
        echo $fl
    done
fi

files=''
for fl in ${file_arr[@]}; do
    files="${files} bin/${fl}.o"
done

g++ main.cpp $files -lGL -lX11 -lglfw -lGLEW -o bin/game