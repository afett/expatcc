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

#include <expat.h>

#include "expatcc.h"

namespace expatcc {

namespace internal {

class parser {
public:
	parser();
	~parser();

	void on_start_element(start_element_cb const&);
	void on_end_element(end_element_cb const&);
	void on_character_data(character_data_cb const&);
	bool parse(std::string const&, bool);
	void reset();

private:
	void handle_start_element(const XML_Char *, const XML_Char **);
	void handle_end_element(const XML_Char *);
	void handle_character_data(const XML_Char *, int);
	static parser & self(void *);
	static void start_element_handler(void *, const XML_Char *, const XML_Char **);
	static void end_element_handler(void *, const XML_Char *);
	static void character_data_handler(void *, const XML_Char *, int);

	start_element_cb on_start_element_;
	end_element_cb on_end_element_;
	character_data_cb on_character_data_;
	XML_Parser self_;
};

parser::parser()
:
	self_(XML_ParserCreate(NULL))
{
	XML_SetUserData(self_, this);
}

void parser::on_start_element(start_element_cb const& cb)
{
	on_start_element_ = cb;
	XML_SetStartElementHandler(self_,
		&parser::start_element_handler);
}

void parser::on_end_element(end_element_cb const& cb)
{
	on_end_element_ = cb;
	XML_SetEndElementHandler(self_,
		&parser::end_element_handler);
}

void parser::on_character_data(character_data_cb const& cb)
{
	on_character_data_ = cb;
	XML_SetCharacterDataHandler(self_,
		&parser::character_data_handler);
}

bool parser::parse(std::string const& s, bool final = false)
{
	return XML_Parse(self_, s.c_str(), s.size(), final) == XML_STATUS_OK;
}

void parser::reset()
{
	XML_ParserReset(self_, NULL);
}

parser::~parser()
{
	XML_ParserFree(self_);
	self_ = 0;
}

void parser::handle_start_element(const XML_Char *name, const XML_Char **atts)
{
	attribute_map attr;
	const XML_Char *key(0);
	for (size_t i(0); atts[i] != 0; ++i) {
		if (!key) {
			key = atts[i];
		} else {
			attr[key] = atts[i];
			key = 0;
		}
	}

	on_start_element_(name, attr);
}

void parser::handle_end_element(const XML_Char *name)
{
	on_end_element_(name);
}

void parser::handle_character_data(const XML_Char *s, int len)
{
	on_character_data_(std::string(s, len));
}

parser & parser::self(void *data)
{
	return *(static_cast<parser *>(data));
}

void parser::start_element_handler(void *data, const XML_Char *name, const XML_Char **atts)
{
	self(data).handle_start_element(name, atts);
}

void parser::end_element_handler(void *data, const XML_Char *name)
{
	self(data).handle_end_element(name);
}

void parser::character_data_handler(void *data, const XML_Char *s, int len)
{
	self(data).handle_character_data(s, len);
}

} // internal

parser::parser()
:
	impl_(new internal::parser())
{ }

parser::~parser()
{
	impl_.reset();
}

void parser::on_start_element(start_element_cb const& cb)
{
	impl_->on_start_element(cb);
}

void parser::on_end_element(end_element_cb const& cb)
{
	impl_->on_end_element(cb);
}

void parser::on_character_data(character_data_cb const& cb)
{
	impl_->on_character_data(cb);
}

bool parser::parse(std::string const& xml, bool final)
{
	return impl_->parse(xml, final);
}

void parser::reset()
{
	impl_->reset();
}

}
