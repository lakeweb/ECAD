

#if !defined(PUGIXML_NO_XPATH) && ! defined( PUGIXML_NO_XPATH_HPP)
#define PUGIXML_NO_XPATH_HPP

namespace pugi{
	// XPath query return type
	enum xpath_value_type
	{
		xpath_type_none,	  // Unknown type (query failed to compile)
		xpath_type_node_set,  // Node set (xpath_node_set)
		xpath_type_number,	  // Number
		xpath_type_string,	  // String
		xpath_type_boolean	  // Boolean
	};

	// XPath parsing result
	struct PUGIXML_CLASS xpath_parse_result
	{
		// Error message (0 if no error)
		const char* error;

		// Last parsed offset (in char_t units from string start)
		ptrdiff_t offset;

		// Default constructor, initializes object to failed state
		xpath_parse_result();

		// Cast to bool operator
		operator bool() const;

		// Get error description
		const char* description() const;
	};

	// A single XPath variable
	class PUGIXML_CLASS xpath_variable
	{
		friend class xpath_variable_set;

	protected:
		xpath_value_type _type;
		xpath_variable* _next;

		xpath_variable(xpath_value_type type);

		// Non-copyable semantics
		xpath_variable(const xpath_variable&);
		xpath_variable& operator=(const xpath_variable&);

	public:
		// Get variable name
		const char_t* name() const;

		// Get variable type
		xpath_value_type type() const;

		// Get variable value; no type conversion is performed, default value (false, NaN, empty string, empty node set) is returned on type mismatch error
		bool get_boolean() const;
		double get_number() const;
		const char_t* get_string() const;
		const xpath_node_set& get_node_set() const;

		// Set variable value; no type conversion is performed, false is returned on type mismatch error
		bool set(bool value);
		bool set(double value);
		bool set(const char_t* value);
		bool set(const xpath_node_set& value);
	};

	// A set of XPath variables
	class PUGIXML_CLASS xpath_variable_set
	{
	private:
		xpath_variable* _data[64];

		void _assign(const xpath_variable_set& rhs);
		void _swap(xpath_variable_set& rhs);

		xpath_variable* _find(const char_t* name) const;

		static bool _clone(xpath_variable* var, xpath_variable** out_result);
		static void _destroy(xpath_variable* var);

	public:
		// Default constructor/destructor
		xpath_variable_set();
		~xpath_variable_set();

		// Copy constructor/assignment operator
		xpath_variable_set(const xpath_variable_set& rhs);
		xpath_variable_set& operator=(const xpath_variable_set& rhs);

	#ifdef PUGIXML_HAS_MOVE
		// Move semantics support
		xpath_variable_set(xpath_variable_set&& rhs) PUGIXML_NOEXCEPT;
		xpath_variable_set& operator=(xpath_variable_set&& rhs) PUGIXML_NOEXCEPT;
	#endif

		// Add a new variable or get the existing one, if the types match
		xpath_variable* add(const char_t* name, xpath_value_type type);

		// Set value of an existing variable; no type conversion is performed, false is returned if there is no such variable or if types mismatch
		bool set(const char_t* name, bool value);
		bool set(const char_t* name, double value);
		bool set(const char_t* name, const char_t* value);
		bool set(const char_t* name, const xpath_node_set& value);

		// Get existing variable by name
		xpath_variable* get(const char_t* name);
		const xpath_variable* get(const char_t* name) const;
	};

	// A compiled XPath query object
	class PUGIXML_CLASS xpath_query
	{
	private:
		void* _impl;
		xpath_parse_result _result;

		typedef void (*unspecified_bool_type)(xpath_query***);

		// Non-copyable semantics
		xpath_query(const xpath_query&);
		xpath_query& operator=(const xpath_query&);

	public:
		// Construct a compiled object from XPath expression.
		// If PUGIXML_NO_EXCEPTIONS is not defined, throws xpath_exception on compilation errors.
		explicit xpath_query(const char_t* query, xpath_variable_set* variables = 0);

		// Constructor
		xpath_query();

		// Destructor
		~xpath_query();

	#ifdef PUGIXML_HAS_MOVE
		// Move semantics support
		xpath_query(xpath_query&& rhs) PUGIXML_NOEXCEPT;
		xpath_query& operator=(xpath_query&& rhs) PUGIXML_NOEXCEPT;
	#endif

		// Get query expression return type
		xpath_value_type return_type() const;

		// Evaluate expression as boolean value in the specified context; performs type conversion if necessary.
		// If PUGIXML_NO_EXCEPTIONS is not defined, throws std::bad_alloc on out of memory errors.
		bool evaluate_boolean(const xpath_node& n) const;

		// Evaluate expression as double value in the specified context; performs type conversion if necessary.
		// If PUGIXML_NO_EXCEPTIONS is not defined, throws std::bad_alloc on out of memory errors.
		double evaluate_number(const xpath_node& n) const;

	#ifndef PUGIXML_NO_STL
		// Evaluate expression as string value in the specified context; performs type conversion if necessary.
		// If PUGIXML_NO_EXCEPTIONS is not defined, throws std::bad_alloc on out of memory errors.
		string_t evaluate_string(const xpath_node& n) const;
	#endif

		// Evaluate expression as string value in the specified context; performs type conversion if necessary.
		// At most capacity characters are written to the destination buffer, full result size is returned (includes terminating zero).
		// If PUGIXML_NO_EXCEPTIONS is not defined, throws std::bad_alloc on out of memory errors.
		// If PUGIXML_NO_EXCEPTIONS is defined, returns empty  set instead.
		size_t evaluate_string(char_t* buffer, size_t capacity, const xpath_node& n) const;

		// Evaluate expression as node set in the specified context.
		// If PUGIXML_NO_EXCEPTIONS is not defined, throws xpath_exception on type mismatch and std::bad_alloc on out of memory errors.
		// If PUGIXML_NO_EXCEPTIONS is defined, returns empty node set instead.
		xpath_node_set evaluate_node_set(const xpath_node& n) const;

		// Evaluate expression as node set in the specified context.
		// Return first node in document order, or empty node if node set is empty.
		// If PUGIXML_NO_EXCEPTIONS is not defined, throws xpath_exception on type mismatch and std::bad_alloc on out of memory errors.
		// If PUGIXML_NO_EXCEPTIONS is defined, returns empty node instead.
		xpath_node evaluate_node(const xpath_node& n) const;

		// Get parsing result (used to get compilation errors in PUGIXML_NO_EXCEPTIONS mode)
		const xpath_parse_result& result() const;

		// Safe bool conversion operator
		operator unspecified_bool_type() const;

		// Borland C++ workaround
		bool operator!() const;
	};

	#ifndef PUGIXML_NO_EXCEPTIONS
	// XPath exception class
	class PUGIXML_CLASS xpath_exception: public std::exception
	{
	private:
		xpath_parse_result _result;

	public:
		// Construct exception from parse result
		explicit xpath_exception(const xpath_parse_result& result);

		// Get error message
		virtual const char* what() const throw() PUGIXML_OVERRIDE;

		// Get parse result
		const xpath_parse_result& result() const;
	};
	#endif

	// XPath node class (either xml_node or xml_attribute)
	class PUGIXML_CLASS xpath_node
	{
	private:
		xml_node _node;
		xml_attribute _attribute;

		typedef void (*unspecified_bool_type)(xpath_node***);

	public:
		// Default constructor; constructs empty XPath node
		xpath_node();

		// Construct XPath node from XML node/attribute
		xpath_node(const xml_node& node);
		xpath_node(const xml_attribute& attribute, const xml_node& parent);

		// Get node/attribute, if any
		xml_node node() const;
		xml_attribute attribute() const;

		// Get parent of contained node/attribute
		xml_node parent() const;

		// Safe bool conversion operator
		operator unspecified_bool_type() const;

		// Borland C++ workaround
		bool operator!() const;

		// Comparison operators
		bool operator==(const xpath_node& n) const;
		bool operator!=(const xpath_node& n) const;
	};

#ifdef __BORLANDC__
	// Borland C++ workaround
	bool PUGIXML_FUNCTION operator&&(const xpath_node& lhs, bool rhs);
	bool PUGIXML_FUNCTION operator||(const xpath_node& lhs, bool rhs);
#endif

	// A fixed-size collection of XPath nodes
	class PUGIXML_CLASS xpath_node_set
	{
	public:
		// Collection type
		enum type_t
		{
			type_unsorted,			// Not ordered
			type_sorted,			// Sorted by document order (ascending)
			type_sorted_reverse		// Sorted by document order (descending)
		};

		// Constant iterator type
		typedef const xpath_node* const_iterator;

		// We define non-constant iterator to be the same as constant iterator so that various generic algorithms (i.e. boost foreach) work
		typedef const xpath_node* iterator;

		// Default constructor. Constructs empty set.
		xpath_node_set();

		// Constructs a set from iterator range; data is not checked for duplicates and is not sorted according to provided type, so be careful
		xpath_node_set(const_iterator begin, const_iterator end, type_t type = type_unsorted);

		// Destructor
		~xpath_node_set();

		// Copy constructor/assignment operator
		xpath_node_set(const xpath_node_set& ns);
		xpath_node_set& operator=(const xpath_node_set& ns);

	#ifdef PUGIXML_HAS_MOVE
		// Move semantics support
		xpath_node_set(xpath_node_set&& rhs) PUGIXML_NOEXCEPT;
		xpath_node_set& operator=(xpath_node_set&& rhs) PUGIXML_NOEXCEPT;
	#endif

		// Get collection type
		type_t type() const;

		// Get collection size
		size_t size() const;

		// Indexing operator
		const xpath_node& operator[](size_t index) const;

		// Collection iterators
		const_iterator begin() const;
		const_iterator end() const;

		// Sort the collection in ascending/descending order by document order
		void sort(bool reverse = false);

		// Get first node in the collection by document order
		xpath_node first() const;

		// Check if collection is empty
		bool empty() const;

	private:
		type_t _type;

		xpath_node _storage;

		xpath_node* _begin;
		xpath_node* _end;

		void _assign(const_iterator begin, const_iterator end, type_t type);
		void _move(xpath_node_set& rhs) PUGIXML_NOEXCEPT;
	};
}//namespace pugi
#endif
