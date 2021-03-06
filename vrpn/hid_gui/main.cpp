/**
	@file
	@brief Implementation

	@date 2011

	@author
	Ryan Pavlik
	<rpavlik@iastate.edu> and <abiryan@ryand.net>
	http://academic.cleardefinition.com/
	Iowa State University Virtual Reality Applications Center
	Human-Computer Interaction Graduate Program
*/

//          Copyright Iowa State University 2011.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Internal Includes
#include "MainWindow.h"
#include "vrpn_HumanInterface.h"

// Library/third-party includes
#include <QtGui/QApplication>

// Standard includes
#include <sstream>


int usage(char * argv0) {
	printf("Usage:\n\n"
	       "%s -h|--help\n"
	       "	Display this help text.\n\n"

	       "%s [N]\n"
	       "	Open HID device number N (default to 0)\n\n"

	       "%s VEND PROD [N]\n"
	       "	Open HID device number N (default to 0) that matches\n"
	       "	vendor VEND and product PROD, in _decimal_\n\n"

	       ,
	       argv0, argv0, argv0);
	return 1;
}

int failedOnArgument(int argNum, const char * expected, char * argv[]) {
	fprintf(stderr, "Failed to interpret argument %d: expected %s, got '%s' - usage help follows.\n\n", argNum, expected, argv[argNum]);
	return usage(argv[0]);
}

int main(int argc, char *argv[]) {
	QApplication a(argc, argv);

	if (argc > 1 && (std::string("-h") == argv[1] || std::string("--help") == argv[1])) {
		return usage(argv[0]);
	}
	vrpn_HidAcceptor * acceptor = NULL;
	unsigned N = 0; // Which device to open?
	if (argc >= 3) {
		vrpn_uint16 vend;
		std::istringstream vendS(argv[1]);
		if (!(vendS >> vend)) {
			return failedOnArgument(1, "a decimal vendor ID", argv);
		}

		vrpn_uint16 prod;
		std::istringstream prodS(argv[2]);
		if (!(prodS >> prod)) {
			return failedOnArgument(2, "a decimal product ID", argv);
		}

		if (argc >= 4) {
			std::istringstream nS(argv[3]);
			if (!(nS >> N)) {
				return failedOnArgument(3, "a number indicating which matching device to pick, or nothing for the default '0'", argv);
			}
		}
		printf("Will accept HID device number %u that has vendor:product %04x:%04x\n", N, vend, prod);
		acceptor = new vrpn_HidProductAcceptor(vend, prod);
	} else {
		if (argc == 2) {
			std::istringstream nS(argv[1]);
			if (!(nS >> N)) {
				return failedOnArgument(1, "a number indicating which device to pick, or nothing for the default '0'", argv);
			}
		}
		printf("Will accept HID device number %u\n", N);
		acceptor =  new vrpn_HidAlwaysAcceptor;
	}

	MainWindow w(new vrpn_HidNthMatchAcceptor(N, acceptor));
	w.show();

	return a.exec();
}
