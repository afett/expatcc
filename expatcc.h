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

#include <iosfwd>
#include <map>
#include <memory>
#include <string>
#include <tr1/functional>

namespace expatcc {

struct error {
	error();
	std::ostream & print(std::ostream &) const;

	enum Type {
		TYPE_NONE,
		TYPE_NO_MEMORY,
		TYPE_SYNTAX,
		TYPE_NO_ELEMENTS,
		TYPE_INVALID_TOKEN,
		TYPE_UNCLOSED_TOKEN,
		TYPE_PARTIAL_CHAR,
		TYPE_TAG_MISMATCH,
		TYPE_DUPLICATE_ATTRIBUTE,
		TYPE_JUNK_AFTER_DOC_ELEMENT,
		TYPE_PARAM_ENTITY_REF,
		TYPE_UNDEFINED_ENTITY,
		TYPE_RECURSIVE_ENTITY_REF,
		TYPE_ASYNC_ENTITY,
		TYPE_BAD_CHAR_REF,
		TYPE_BINARY_ENTITY_REF,
		TYPE_ATTRIBUTE_EXTERNAL_ENTITY_REF,
		TYPE_MISPLACED_XML_PI,
		TYPE_UNKNOWN_ENCODING,
		TYPE_INCORRECT_ENCODING,
		TYPE_UNCLOSED_CDATA_SECTION,
		TYPE_EXTERNAL_ENTITY_HANDLING,
		TYPE_NOT_STANDALONE,
		TYPE_UNEXPECTED_STATE,
		TYPE_ENTITY_DECLARED_IN_PE,
		TYPE_FEATURE_REQUIRES_XML_DTD,
		TYPE_CANT_CHANGE_FEATURE_ONCE_PARSING,
		TYPE_UNBOUND_PREFIX,
		TYPE_UNDECLARING_PREFIX,
		TYPE_INCOMPLETE_PE,
		TYPE_XML_DECL,
		TYPE_TEXT_DECL,
		TYPE_PUBLICID,
		TYPE_SUSPENDED,
		TYPE_NOT_SUSPENDED,
		TYPE_ABORTED,
		TYPE_FINISHED,
		TYPE_SUSPEND_PE,
		TYPE_RESERVED_PREFIX_XML,
		TYPE_RESERVED_PREFIX_XMLNS,
		TYPE_RESERVED_NAMESPACE_URI,
	} type;

	size_t line;
	size_t column;
	std::string string;
};

inline std::ostream & operator<<(std::ostream & os, error const& e)
{
	return e.print(os);
}

typedef std::map<std::string, std::string> attribute_map;
typedef std::tr1::function<void(std::string const&, attribute_map const&)> start_element_cb;
typedef std::tr1::function<void(std::string const&)> end_element_cb;
typedef std::tr1::function<void(std::string const&)> character_data_cb;
typedef std::tr1::function<void(error const&)> error_cb;

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
	void on_error(error_cb const&);
	bool parse(std::string const&, bool final = false);
	void reset();

private:
	parser(parser const&); // = deleted;
	parser & operator=(parser const&); // = deleted;

	std::auto_ptr<internal::parser> impl_;
};

}

#endif
