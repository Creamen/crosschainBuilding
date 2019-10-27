all:
	@echo "make [arm32|arm64|x86_64|x86_32]"
	@exit 0

clean:

arm32:
	./go.sh arm-linux-gnueabi

arm64:
	./go.sh aarch64-linux-gnu 

x86_64:
	./go.sh x86_64-linux-gnu 

x86_32:
	./go.sh i386-linux-gnu

