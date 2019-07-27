
#pragma once
#include "pugixml.hpp"
//as of this version of VS2017 std::filesystem is still considered experimental so using boost.
//#include <filesystem>
#include <boost/filesystem.hpp>
namespace filesystem = boost::filesystem;
//the lexical_cast below should be wrapped so as throws are caught TODO
#include <boost/lexical_cast.hpp>
/*
pugi xml programing wrapper.
Rational is first that the XML document does not exist and should be populated with
the application defaults. The first test was to use this wrapper with Microsoft's
CSettingsStore class. That class is derived so that it stays out of the registry
and uses an XML document to store app state data.

pugixml is geared for attribute use. getting/setting values of tags is clumsy. So for now
the use an attribute wrapper is left alone. The use of attributes is a better way to
go, often. But there is this registry interface that makes values primary as well as
legacy code
*/


//ok, the begining...............................
//two classes should sufice
//	CXML the document container:  derived from pugi::xml_document as public
//  XMLNODE where most of the work is done:  derived from pugi::xml_node as public
//  
namespace XML
{
	// global lexical cast
	template<typename To, typename From>
		To lexical_cast(From from) {
		To result;
		try {
			result = boost::lexical_cast<To>(from);
		}
		catch (boost::bad_lexical_cast &)
		{
			result = boost::lexical_cast<To>(0);
			//do something more????
		}
		return result;
	}
	// ......................................................
	class XMLNODE : public pugi::xml_node
	{
		pugi::xml_node find_named_node(const char* inname) {
			return find_node([&](const pugi::xml_node& node) { return std::string(inname) == node.name(); });
		}
	public:
		XMLNODE() { }
		XMLNODE(const pugi::xml_node& in) : xml_node(in) { }
		XMLNODE& operator = (pugi::xml_node& in) { _root = in.internal_object(); return *this; }
		XMLNODE AddChild(const char* name) { return append_child(name); }

		//add next child to 'this' node. 'tag name', 'element value' as default if new.
		template<typename T>
		XMLNODE SetElement(const char* inname, T val) {
			xml_node text;
			auto cval = lexical_cast<std::string>(val);
			auto child = find_named_node(inname);
			if (!child)
				child = append_child(inname);
			else
				text = child.find_child([](const xml_node& node) {return node.type() == pugi::node_pcdata; });

			if (!text)
				text = child.append_child(pugi::node_pcdata);
			text.set_value(lexical_cast<std::string>(val).c_str());
			return child;
		}
		// bool.....
		template<>
		XMLNODE SetElement<bool>(const char* inname, bool val) {
			std::string str(val ? "true" : "false");
			return SetElement(inname, str);
		}
		template<typename T>
		XMLNODE SetElementArray(const char*inname, const T ar[], size_t size)
		{
			std::ostringstream os;
			std::for_each(ar, ar + size,
				[&os](T i) { os << std::hex << std::setfill('0') << std::setw(sizeof(T) * 2) << (size_t)i << ','; });
			std::string target(os.str());
			target.erase(target.end() - 1);
			return SetElement(inname, target);
		}
		template<typename T>
		XMLNODE SetElementArray(const wchar_t* inname, const T* bytes, size_t size)
		{
			return SetElementArray(pugi::as_utf8(inname).c_str(), bytes, size);
		}
		template<typename T>
		XMLNODE SetElement(const std::string& inname, const T& val)
		{
			return SetElement(inname.c_str(), val);
		}
		template<typename T>
		XMLNODE SetElement(const wchar_t* inname, const T& val)
		{
			return SetElement(pugi::as_utf8(inname), val);
		}

		template<typename T>
		void SetAttribute(const char* tag, const T& value)
		{
			auto node = attribute(tag);
			if (node.empty())
				node = append_attribute(tag);
			node = value;
		}
		//{ will have to switch by type!!?? attribute(tag,) }
		void SetAttribute(const char* tag, const char* value) { append_attribute(tag) = value; }
		//		void SetAttribute(const char* tag, uint32_t value) { if( append_attribute(tag) = value; }
		void SetAttribute(const char* tag, const std::string& value) { SetAttribute(tag, value.c_str()); }
		//Gets.....
		XMLNODE GetElement(const char* inname) //check if node exists
		{
			return find_named_node(inname);
		}

		template<typename T>
		XMLNODE GetElement(const char* inname, T& val, bool bCanCreate = true)
		{
			auto node = find_named_node(inname);
			if (!node || !node.first_child())
			{
				if (!bCanCreate)
					return XMLNODE();

				auto child = append_child(inname);
				child.append_child(pugi::node_pcdata).set_value(lexical_cast<std::string>(val).c_str());
				return child;
			}
			//else
			val = lexical_cast<T>(node.first_child().value());
			return node;
		}
		//bool..
		XMLNODE GetElement(const char* inname, bool& val, bool bCanCreate = true)
		{
			auto node = find_named_node(inname);
			if (!node)
			{
				node = append_child(inname).append_child(pugi::node_pcdata);
				node.set_value(val ? "true" : "false");
				return node;
			}
			//else
			val = std::string(node.first_child().value()) == "true" ? true : false;
			return node;
		}
		template<typename T>
		XMLNODE GetElementArray(const char* inname, T** ar, size_t* size)
		{
			auto node = find_named_node(inname);
			if (node)
			{
				auto to_num = [](const char** p)
				{
					const char* e;
					for (e = *p; isalnum(*e); ++e);//stops at comma or null
					std::stringstream str;
					str << std::hex << std::string(*p, e++);
					uint64_t result;
					str >> result;
					*p = e;
					return result;
				};
				auto p = node.first_child().value();
				//we will count the commas to get the size
				//TODO, assuming there is at least one value for now
				size_t cnt = 1;
				for (auto t = p; *t; ++t)
					if (*t == ',')	++cnt;

				*ar = new T[cnt];
				size_t pos = 0;
				for (size_t i = 0; i < cnt; ++i)
				{
					(*ar)[pos++] = (T)to_num(&p);
				}
				*size = cnt;
			}
			return node;
		}
		template<typename T>
		XMLNODE GetElement(const std::string& inname, T& val, bool bCanCreate = true)
		{
			return GetElement(inname.c_str(), val, bCanCreate);
		}
		template<typename T>
		XMLNODE GetElement(const wchar_t* inname, T& val, bool bCanCreate = true)
		{
			return GetElement(pugi::as_utf8(inname), val, bCanCreate);
		}
		const pugi::char_t* GetElementValue(pugi::char_t* name) {
			return GetElement(name).find_child([](const xml_node& node) {return node.type() == pugi::node_pcdata; }).value();
		}
		int GetElementValueInt(pugi::char_t* name) {
			return  lexical_cast<int>(GetElement(name).find_child([](const xml_node& node)
			{return node.type() == pugi::node_pcdata; }).value());
		}
		int GetElementValueBool(pugi::char_t* name) {
			auto val =GetElement(name).find_child([](const xml_node& node)
				{return node.type() == pugi::node_pcdata; }).value();
			return ! _stricmp(val, "true");
		}
		const pugi::char_t* GetAttribute(const pugi::char_t* name) { return attribute(name).value(); }
		int GetAttributeInt(const pugi::char_t* name) { return attribute(name).as_int(); }
		double GetAttributeDouble(const pugi::char_t* name) { return attribute(name).as_double(); }
		const pugi::char_t* GetValue() { return child_value(); }
		bool SetValue(const pugi::char_t* ps_val) {
			xml_node node = this->find_child([](const xml_node& node) {return node.type() == pugi::node_pcdata; });
			if(! node)
				node = append_child(pugi::node_pcdata);
			return node.set_value(ps_val);
		}
		bool SetValue(const std::string& str) { return SetValue(str.c_str()); }
		//delete all node children
		//void clear() { for (auto it = begin(); it != end(); ++it) it-> }
	};

	// ......................................................
	class CXML : public pugi::xml_document
	{
		filesystem::path doc_path;
		XMLNODE current_node; //this may be too much, not needed...
		pugi::xml_parse_status status;
		XMLNODE GetXMLRoot()
			{return root().find_child([](const pugi::xml_node& node) {return node.type() == pugi::node_element; });}
	public:
		const char* file_name()const { return doc_path.string().c_str(); }
		//if root name not specified, will take from open existing or on create file, set to 'root'
		XMLNODE Open(const filesystem::path& path, size_t flags = 0/* TODO flags; like create and program*/, const char* rootname = nullptr)
		{
			if (!rootname) rootname = "root";
			doc_path = path;
			if (filesystem::exists(doc_path))
			{
				status = load_file(doc_path.string().c_str(),pugi::parse_full).status;//check 'root', etc..?
				xml_node node = GetXMLRoot();
				if (!node)//we don't have a root document node!!!!!
					node = root().append_child(rootname);
				current_node = node;
				return current_node;
			}
			current_node = append_child(rootname);
			assert(current_node);
			return current_node;
		}
		XMLNODE Open(const char* path, size_t flags = 0, const char* rootname = "root") { return Open(filesystem::path(path), flags, rootname); }
		//not that this should be done here, but.....
		void CreateSVG()
		{
			Clear("svg");
			xml_node decl = prepend_child(pugi::node_declaration);
			decl.append_attribute("version") = "1.0";
			decl.append_attribute("encoding") = "UTF-8";
			decl.append_attribute("standalone") = "no";
			insert_child_after(pugi::node_doctype, decl).set_value("svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\"");
		}
		//Does a save, but the document is still valid and can be 'Closed' again
		void Close()
		{
			save_file(doc_path.string().c_str(), PUGIXML_TEXT("  "));
		}
		XMLNODE get_current_node() { return current_node; }
		XMLNODE Clear(const char* name = "root") { reset(); return current_node = root().append_child(name); }
		XMLNODE AddChild(char* name) { append_child(name); }//?????

		//XPath node creation, done here cause of current node
		//only supports 'from the root' so far, as: /root/this/that
		//root must exist; 'this' and 'that' would be created if they don't exist
		//'that' node is returned. if bCanCreate is set to false, no path is created.
		//this can be extended as pugi xpath headers are move up from the .cpp file
		//pugi development is in: 
		XMLNODE GetNode(const char* pstr, bool bCanCreate = true)
		{
			assert(*pstr == '/');
			++pstr;
			auto nextp = [](const char* p) { for (; *p && *p != '/'; ++p); return p; };
			const char* lp = nextp(pstr);
			std::string name(pstr, lp);
			XMLNODE node = GetXMLRoot();// .first_child();
			assert(name == node.name());
			//from here we can walk down and create as needed.
			for (pstr = lp + 1; lp = nextp(pstr), *pstr; pstr = *lp ? lp + 1 : lp)
			{
				name.assign(pstr, lp);
				std::string nouse;
				node = node.GetElement(name, nouse, bCanCreate);
				if (!node)
					break;
			}
			return node;
		}
		XMLNODE GetNode(const std::string& str, bool bCanCreate = true) { return GetNode(str.c_str(), bCanCreate); }
		XMLNODE LoadString(const pugi::char_t* in) { load_string(in); return current_node = root().first_child(); }
		XMLNODE LoadString(const std::string in) { load_string(in.c_str()); return current_node = root().first_child(); }
		XMLNODE operator >>(std::istream& is) { load(is); return current_node = root().first_child(); }
		XMLNODE LoadSteam(std::istream&in) { load(in); return current_node = root().first_child(); }
	};
}//namespace XML

 //for XMLSettingsStore, etc...
using SP_XML = boost::shared_ptr<XML::CXML>;
using SP_XMLNODE = boost::shared_ptr<XML::XMLNODE>;
