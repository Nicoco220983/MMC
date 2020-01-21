#include <iostream>
#include <vector>
#include <boost/algorithm/string.hpp>

#include "context.hpp"
#include "utils.hpp"
#include "mmc.hpp"

using namespace mmc;


const char* usage = \
  "mmc [OPTIONS]\n"
  "  -h/--help: Print this help.\n"
  "  -i/--input PATH: Input path to compress (file or dir).\n"
  "  -o/--output PATH: compression destination.\n"
  "  --overwrite: Overwrite the input content.\n"
  "  -l/--compression-level LEVEL: Compression level (Possible value: S, M, L).\n"
  "  --img-min-length: Image minimum length.\n";


/*
void exitBadArg(){
	printf("ERROR: Bad arguments.\n");
	printf("%s", usage);
	exit(1);
}*/

void _exitErr(const char* msg){
	std::cerr << "[ERROR] " << msg << std::endl;
	exit(1);
}

const char* _getArg(int i, int argc, char **argv){
	if(i>=argc) {
		std::cerr << "[ERROR] Bad arguments." << std::endl;
		std::cerr << usage << std::endl;
		exit(1);
	}
	return argv[i];
}

int main(int argc, char *argv[]){

	Context ctx;

	for(int i=1; i<argc; ++i){
		auto arg = std::string(argv[i]);
		if(arg=="-h" || arg=="--help"){
			std::cout << usage << std::endl;
			exit(0);
		}
		else if(arg=="-i" || arg=="--input"){
			ctx.inputPath = _getArg(++i, argc, argv);
		}
		else if(arg=="-o" || arg=="--output"){
			ctx.outputPath = _getArg(++i, argc, argv);
		}
		else if(arg == "--overwrite"){
			ctx.outputMode = Context::OutputMode::OVERWRITE;
		}
		else if(arg=="-l" || arg=="--compression-level"){
			const std::string val = _getArg(++i, argc, argv);
			if(boost::iequals(val, "S")){
				ctx.compressionlevel = Context::CompressionLevel::SMALL;
			} else if(boost::iequals(val, "M")){
				ctx.compressionlevel = Context::CompressionLevel::MEDIUM;
			} else if(boost::iequals(val, "L")){
				ctx.compressionlevel = Context::CompressionLevel::LARGE;
			} else {
				_exitErr(concat({"Unknown value for compression-level: ", val.c_str()}).c_str());
			}
		}
		else if(arg=="--img-min-length"){
			ctx.imgMinLength = atoi(_getArg(++i, argc, argv));
		}
		else
			_exitErr(concat({"Unknown option: ", arg.c_str()}).c_str());
	}

	try {
		mmc::execute(ctx);
	} catch(const std::exception& exc) {
		_exitErr(exc.what());
	}

	return 0;
}