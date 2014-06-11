/*
	Copyright (c) 2014 Andreas Fett.
	All rights reserved.

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions
	are met:
	1. Redistributions of source code must retain the above copyright
	   notice, this list of conditions and the following disclaimer.
	2. Redistributions in binary form must reproduce the above copyright
	   notice, this list of conditions and the following disclaimer in the
	   documentation and/or other materials provided with the distribution.

	THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
	IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
	TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
	PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR
	BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
	CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
	SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
	INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
	CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
	ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
	POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef EXPATCC_H
#define EXPATCC_H

#include <map>
#include <memory>
#include <string>
#include <tr1/functional>

namespace expatcc {

typedef std::map<std::string, std::string> attribute_map;
typedef std::tr1::function<void(std::string const&, attribute_map const&)> start_element_cb;
typedef std::tr1::function<void(std::string const&)> end_element_cb;
typedef std::tr1::function<void(std::string const&)> character_data_cb;

namespace internal {
class parser;
}

class parser {
public:
	parser();
	~parser();

	void on_start_element(start_element_cb const&);
	void on_end_element(end_element_cb const&);
	void on_character_data(character_data_cb const&);
	bool parse(std::string const&, bool final = false);
	void reset();

private:
	parser(parser const&); // = deleted;
	parser & operator=(parser const&); // = deleted;

	std::auto_ptr<internal::parser> impl_;
};

}

#endif
