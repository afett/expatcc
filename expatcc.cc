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
#include <iostream>
#include "expatcc.h"

static expatcc::error::Type error_type(XML_Error);

namespace expatcc {

namespace internal {

class parser {
public:
	parser();
	~parser();

	void on_start_element(start_element_cb const&);
	void on_end_element(end_element_cb const&);
	void on_character_data(character_data_cb const&);
	void on_error(error_cb const&);
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
	error get_error() const;

	start_element_cb on_start_element_;
	end_element_cb on_end_element_;
	character_data_cb on_character_data_;
	error_cb on_error_;
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

void parser::on_error(error_cb const& cb)
{
	on_error_ = cb;
}

error parser::get_error() const
{
	error e;
	XML_Error code(XML_GetErrorCode(self_));
	e.type = ::error_type(code);
	e.line = XML_GetCurrentLineNumber(self_);
	e.column = XML_GetCurrentColumnNumber(self_);
	e.string = XML_ErrorString(code);
	return e;
}

bool parser::parse(std::string const& s, bool final = false)
{
	bool success(XML_Parse(self_, s.c_str(), s.size(), final) == XML_STATUS_OK);
	if (!success) {
		on_error_(error());
	}
	return success;
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

void parser::on_error(error_cb const& cb)
{
	impl_->on_error(cb);
}

bool parser::parse(std::string const& xml, bool final)
{
	return impl_->parse(xml, final);
}

void parser::reset()
{
	impl_->reset();
}

error::error()
:
	type(TYPE_NONE),
	line(0),
	column(0),
	string()
{ }

std::ostream & error::print(std::ostream & os) const
{
	os << string << ": +" << line << "," << column;
	return os;
}

} // expatcc

static expatcc::error::Type error_type(XML_Error code)
{
	switch (code) {
	case XML_ERROR_NONE:                             return expatcc::error::TYPE_NONE;
	case XML_ERROR_NO_MEMORY:                        return expatcc::error::TYPE_NO_MEMORY;
	case XML_ERROR_SYNTAX:                           return expatcc::error::TYPE_SYNTAX;
	case XML_ERROR_NO_ELEMENTS:                      return expatcc::error::TYPE_NO_ELEMENTS;
	case XML_ERROR_INVALID_TOKEN:                    return expatcc::error::TYPE_INVALID_TOKEN;
	case XML_ERROR_UNCLOSED_TOKEN:                   return expatcc::error::TYPE_UNCLOSED_TOKEN;
	case XML_ERROR_PARTIAL_CHAR:                     return expatcc::error::TYPE_PARTIAL_CHAR;
	case XML_ERROR_TAG_MISMATCH:                     return expatcc::error::TYPE_TAG_MISMATCH;
	case XML_ERROR_DUPLICATE_ATTRIBUTE:              return expatcc::error::TYPE_DUPLICATE_ATTRIBUTE;
	case XML_ERROR_JUNK_AFTER_DOC_ELEMENT:           return expatcc::error::TYPE_JUNK_AFTER_DOC_ELEMENT;
	case XML_ERROR_PARAM_ENTITY_REF:                 return expatcc::error::TYPE_PARAM_ENTITY_REF;
	case XML_ERROR_UNDEFINED_ENTITY:                 return expatcc::error::TYPE_UNDEFINED_ENTITY;
	case XML_ERROR_RECURSIVE_ENTITY_REF:             return expatcc::error::TYPE_RECURSIVE_ENTITY_REF;
	case XML_ERROR_ASYNC_ENTITY:                     return expatcc::error::TYPE_ASYNC_ENTITY;
	case XML_ERROR_BAD_CHAR_REF:                     return expatcc::error::TYPE_BAD_CHAR_REF;
	case XML_ERROR_BINARY_ENTITY_REF:                return expatcc::error::TYPE_BINARY_ENTITY_REF;
	case XML_ERROR_ATTRIBUTE_EXTERNAL_ENTITY_REF:    return expatcc::error::TYPE_ATTRIBUTE_EXTERNAL_ENTITY_REF;
	case XML_ERROR_MISPLACED_XML_PI:                 return expatcc::error::TYPE_MISPLACED_XML_PI;
	case XML_ERROR_UNKNOWN_ENCODING:                 return expatcc::error::TYPE_UNKNOWN_ENCODING;
	case XML_ERROR_INCORRECT_ENCODING:               return expatcc::error::TYPE_INCORRECT_ENCODING;
	case XML_ERROR_UNCLOSED_CDATA_SECTION:           return expatcc::error::TYPE_UNCLOSED_CDATA_SECTION;
	case XML_ERROR_EXTERNAL_ENTITY_HANDLING:         return expatcc::error::TYPE_EXTERNAL_ENTITY_HANDLING;
	case XML_ERROR_NOT_STANDALONE:                   return expatcc::error::TYPE_NOT_STANDALONE;
	case XML_ERROR_UNEXPECTED_STATE:                 return expatcc::error::TYPE_UNEXPECTED_STATE;
	case XML_ERROR_ENTITY_DECLARED_IN_PE:            return expatcc::error::TYPE_ENTITY_DECLARED_IN_PE;
	case XML_ERROR_FEATURE_REQUIRES_XML_DTD:         return expatcc::error::TYPE_FEATURE_REQUIRES_XML_DTD;
	case XML_ERROR_CANT_CHANGE_FEATURE_ONCE_PARSING: return expatcc::error::TYPE_CANT_CHANGE_FEATURE_ONCE_PARSING;
	case XML_ERROR_UNBOUND_PREFIX:                   return expatcc::error::TYPE_UNBOUND_PREFIX;
	case XML_ERROR_UNDECLARING_PREFIX:               return expatcc::error::TYPE_UNDECLARING_PREFIX;
	case XML_ERROR_INCOMPLETE_PE:                    return expatcc::error::TYPE_INCOMPLETE_PE;
	case XML_ERROR_XML_DECL:                         return expatcc::error::TYPE_XML_DECL;
	case XML_ERROR_TEXT_DECL:                        return expatcc::error::TYPE_TEXT_DECL;
	case XML_ERROR_PUBLICID:                         return expatcc::error::TYPE_PUBLICID;
	case XML_ERROR_SUSPENDED:                        return expatcc::error::TYPE_SUSPENDED;
	case XML_ERROR_NOT_SUSPENDED:                    return expatcc::error::TYPE_NOT_SUSPENDED;
	case XML_ERROR_ABORTED:                          return expatcc::error::TYPE_ABORTED;
	case XML_ERROR_FINISHED:                         return expatcc::error::TYPE_FINISHED;
	case XML_ERROR_SUSPEND_PE:                       return expatcc::error::TYPE_SUSPEND_PE;
	case XML_ERROR_RESERVED_PREFIX_XML:              return expatcc::error::TYPE_RESERVED_PREFIX_XML;
	case XML_ERROR_RESERVED_PREFIX_XMLNS:            return expatcc::error::TYPE_RESERVED_PREFIX_XMLNS;
	case XML_ERROR_RESERVED_NAMESPACE_URI:           return expatcc::error::TYPE_RESERVED_NAMESPACE_URI;
	}

	return expatcc::error::TYPE_NONE;
}
