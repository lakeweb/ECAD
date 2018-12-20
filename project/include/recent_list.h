
//Basic often used Boost includes........

#pragma once

#include <boost/system/error_code.hpp>

// ..................................................................
#define MRU_MAXSIZE 9 //TODO? adjustable
template < typename Type >
class recent_list
{
protected:
	typedef typename std::list< Type >::const_iterator it_t;
//	using class_type = Type;
//	typedef typename Type::value_type value_type;
	std::list< Type > list;

public:
	it_t begin( ) const { return list.begin( ); }
	it_t end( ) const { return list.end( ); }
	size_t size( ) const { return list.size( ); }
	Type& front( ) { return list.front( ); }

	void clear( ) { list.clear( ); }
	//adds to head of list, checks and does not duplicate path
	//assumes no dups, used to initialize
	void add_path_tail( Type& path ) { list.push_back( path ); }
	//void add_path_tail( value_type& path ) { list.push_back( Type( path ) ); }
	virtual void add_path_head( const Type path )
	{
		auto pit= std::find_if( list.begin( ), list.end( ),
			[ & ] ( const Type pathf ) { return pathf == path; } );

		if( pit == list.end( ) )
		{
			list.insert( list.begin( ), path );
			if( list.size( ) > MRU_MAXSIZE ) //TODO ? MRU_MAXSIZE user adjustable?
				list.pop_back( );
		}
		else
			list.splice( list.begin( ), list, pit );
	}
	void move_to_top( long pos )
	{
		assert( (size_t)pos < list.size( ) && pos >= 0 );
		auto it= list.begin( );
		std::advance( it, pos );
		list.splice( list.begin( ), list, it );
	}
	it_t find(const Type in)
	{
		return std::find_if(list.begin(), list.end(), [&in](const Type& check) { return in == check; });
	}
	void pop()
	{
		list.pop_back();
	}
};

//class bfs_recent_list : public recent_list<bfs::path>
//{
//public:
//	virtual void add_path_head( const class_type& path )
//	{
//		bfs::path tpath( path );
//		recent_list::add_path_head( tpath.normalize( ) );
//	}
//};

//typedef recent_list< bfs::path > mru_list_t;

/*
template< typename T >
struct array_container
{
	typedef typename boost::shared_array< T > container_t;
	container_t container;
	size_t size;

	typedef T value_type;
	typedef T* iterator;
	typedef const T* const_iterator;
	typedef std::reverse_iterator<iterator> reverse_iterator;
	typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
	typedef T& reference;
	typedef const T& const_reference;
	typedef std::size_t size_type;
	typedef std::ptrdiff_t difference_type;

	array_container( size_t size )
		:container( new T[ size ] )
	{
	}
	T* get( ) { return container.get( ); }
	iterator begin( ) { return container.get( ); }
	iterator end( ) { return container.get( ) + size; }
};

*/
