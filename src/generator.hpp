/*
 * This file part of makedict - convertor from any dictionary format to any
 * http://sdcv.sourceforge.net
 * Copyright (C) 2005 Evgeniy <dushistov@mail.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef GENERATOR_HPP
#define GENERATOR_HPP

#include <expat.h>
#include <list>
#include <stack>
#include <string>
#include <map>
#include <memory>

#include "utils.hpp"

typedef std::map<std::string, std::string> StringMap;
typedef std::list<std::string> StringList;

class GeneratorBase;

class IGeneratorDictOps {
public:
	IGeneratorDictOps(GeneratorBase& generator) : generator_(generator) {}
	virtual ~IGeneratorDictOps() {}

	virtual bool get_meta_info() = 0;
	virtual bool get_info() = 0;
	virtual void set_dict_info(const std::string&, const std::string&) = 0;
	virtual const char *get_dict_info(const std::string&) const = 0;
protected:
	GeneratorBase& generator_;
};


class GeneratorDictPipeOps : public IGeneratorDictOps {
public:
	GeneratorDictPipeOps(GeneratorBase& generator);
	bool get_meta_info();
	bool get_info();
	void set_dict_info(const std::string& name, const std::string& val) {
		dict_info_[name] = val;
	}
	const char *get_dict_info(const std::string& name) const {
		StringMap::const_iterator it = dict_info_.find(name);
		if (it == dict_info_.end())
			return "";
		return it->second.c_str();
	}
private:
	enum {mmNONE, mmICON, mmBASENAME} meta_mode_;
	StringMap dict_info_;
	enum StateType { XDXF, ABBREVIATIONS, AR, ABR_DEF,
			 FULL_NAME, DESCRIPTION, K, V, OPT
	};
	typedef std::map<std::string, StateType> TagTable;
	static TagTable tag_table_;
	std::stack<StateType> state_stack_;
	std::string data_;
	struct Key {
		std::vector<std::string> parts_;
		std::vector<std::string> opts_;
		void clear() { parts_.clear(); opts_.clear(); }
	} key_;
	StringList keys_;
	static Str2StrTable xml_spec_seq_;
	std::list<std::string> sample_data_;

	static void XMLCALL on_meta_start(void *, const XML_Char *,
					  const XML_Char **);
	static void XMLCALL on_meta_end(void *, const XML_Char *);
	static void XMLCALL on_meta_data(void *, const XML_Char *, int);

	static void XMLCALL xml_start(void *, const XML_Char *,
				      const XML_Char **);
	static void XMLCALL xml_end(void *, const XML_Char *);
	static void XMLCALL xml_char_data(void *, const XML_Char *, int);

	void generate_keys();
	void sample(std::vector<std::string>::size_type n);
};

class GeneratorBase {
public:
	GeneratorBase();
	virtual ~GeneratorBase() {}
	int run(int argc, char *argv[]);

	virtual void on_abbr_begin() {}
	virtual void on_abbr_end() {}
	virtual void on_have_data(const StringList&, const std::string&) {}
	void set_dict_info(const std::string&, const std::string&);
	void generate_keys();
protected:
	void set_format(const std::string& val)	{ format_ = val; }
	void set_version(const std::string& val) { version_ = val; }
	virtual int generate() = 0;

	virtual void on_new_dict_info(const std::string&, const std::string&) {}	
	virtual bool on_prepare_generator(const std::string&,
					  const std::string&) = 0;
	const char *get_dict_info(const std::string& name) const {
		return dict_ops_->get_dict_info(name);
	}
private:
	std::string format_;
	std::string version_;
	std::auto_ptr<IGeneratorDictOps> dict_ops_;	
};

#endif//!GENERATOR_HPP