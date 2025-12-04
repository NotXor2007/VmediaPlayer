program = "./VmediaPlayer"
src = "./VmediaPlayer.c"
./VmediaPlayer:./VmediaPlayer.c
	gcc $(src) -I./include -L./lib -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -o $(program)
run:
	$(program) ./drawing.vmedia
clean:
	rm $(program)
	rm ./drawing.vmedia
edit:
	nano ./drawing.txt
bytecode_gen:
	xxd -r -p ./drawing.txt ./drawing.vmedia
